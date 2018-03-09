#include "AnalysisDEA.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QChartView>
#include <QScatterSeries>
#include <QFuture>
#include <QtConcurrent>

#include <QMenu>
#include <QClipboard>

#include "math/RInterface.h"

#include "ui_analysisDEA.h"

AnalysisDEA::AnalysisDEA(const QList<STData::STDataFrame> &datasetsA,
                         const QList<STData::STDataFrame> &datasetsB,
                         const QString &nameA,
                         const QString &nameB,
                         QWidget *parent,
                         Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisDEA)
    , m_data()
    , m_conditions()
    , m_nameA(nameA)
    , m_nameB(nameB)
    , m_normalization(SettingsWidget::NormalizationMode::DESEQ)
    , m_reads_threshold(-1)
    , m_genes_threshold(-1)
    , m_ind_reads_treshold(-1)
    , m_spots_threshold(-1)
{
    m_ui->setupUi(this);

    // default values
    m_ui->fdr->setValue(0.1);
    m_ui->foldchange->setValue(1.0);
    m_ui->normalization_deseq->setChecked(true);
    m_ui->exportTable->setEnabled(false);
    m_ui->searchField->setEnabled(false);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    m_ui->searchField->setClearButtonEnabled(true);
    m_ui->conditionA->setText(m_nameA);
    m_ui->conditionB->setText(m_nameB);
    m_proxy.reset(new QSortFilterProxyModel());

    // merge datasets
    m_data = STData::aggregate(datasetsA + datasetsB);

    // create connections
    connect(m_ui->searchField,
            &QLineEdit::textChanged,
            m_proxy.data(),
            &QSortFilterProxyModel::setFilterFixedString);
    connect(m_ui->run, &QPushButton::clicked, this, &AnalysisDEA::run);
    connect(m_ui->exportTable, &QPushButton::clicked, this, &AnalysisDEA::slotExportTable);
    connect(m_ui->tableview,
            &QTableView::clicked,
            this,
            &AnalysisDEA::slotGeneSelected);
    connect(&m_watcher, &QFutureWatcher<void>::finished, this, &AnalysisDEA::slotDEAComputed);
    connect(m_ui->exportPlot, &QPushButton::clicked, this, &AnalysisDEA::slotExportPlot);
    // allow to copy the content of the table
    m_ui->tableview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->tableview, &QTableView::customContextMenuRequested,
            this, &AnalysisDEA::customMenuRequested);
}

AnalysisDEA::~AnalysisDEA()
{

}

void AnalysisDEA::slotExportTable()
{
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Export DE Genes"),
                                                          QDir::homePath(),
                                                          QString("%1").arg(tr("TXT Files (*.txt *.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        QMessageBox::critical(this, tr("Export DE Genes"), tr("The directory is not writable"));
        return;
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        // write columns (1st row)
        stream << "Gene" << "\t" << "FDR" << "\t" << "p-value" << "\t" << "log2FoldChange" << endl;
        // write values
        for (uword i = 0; i < m_results.n_rows; ++i) {
            const QString gene = QString::fromStdString(m_results_rows.at(i));
            const double fdr = m_results.at(i, 5);
            const double pvalue = m_results.at(i, 4);
            const double foldchange = m_results.at(i, 1);
            if (fdr <= m_ui->fdr->value() && std::abs(foldchange) >= m_ui->foldchange->value()) {
                stream << gene << "\t" << fdr << "\t" << pvalue << "\t" << foldchange << endl;
            }
        }
    } else {
        QMessageBox::critical(this, tr("Export DE Genes"), tr("Coult not open the file"));
    }
    file.close();
}

void AnalysisDEA::slotGeneSelected(QModelIndex index)
{
    // Check if the selection is valid
    if (!index.isValid() || m_proxy.isNull()) {
        m_gene_highlight = QPointF();
        return;
    }

    const QItemSelection &selected = m_ui->tableview->selectionModel()->selection();
    const QModelIndexList &selected_indexes = m_proxy->mapSelectionToSource(selected).indexes();

    // Check if only elements are selected
    if (selected_indexes.empty()) {
        m_gene_highlight = QPointF();
        return;
    }

    // update the highlight coordinate and refresh the plot
    const int row_index = selected_indexes.first().row();
    const double pvalue = -log10(m_results.at(row_index, 4) + std::numeric_limits<double>::epsilon());
    const double foldchange = m_results.at(row_index, 1);
    m_gene_highlight = QPointF(foldchange, pvalue);
    updatePlot();
}

void AnalysisDEA::updatePlot()
{
    QScatterSeries *series1 = new QScatterSeries();
    QScatterSeries *series2 = new QScatterSeries();

    series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series1->setMarkerSize(5.0);
    series1->setColor(Qt::gray);
    series1->setUseOpenGL(false);

    series2->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series2->setMarkerSize(5.0);
    series2->setColor(Qt::red);
    series2->setUseOpenGL(false);

    // populate
    for (uword i = 0; i < m_results.n_rows; ++i) {
        const double fdr = m_results.at(i, 5);
        const double pvalue = -log10(m_results.at(i, 4) + std::numeric_limits<double>::epsilon());
        const double foldchange = m_results.at(i, 1);
        if (fdr <= m_ui->fdr->value() && std::abs(foldchange) >= m_ui->foldchange->value()) {
            series2->append(foldchange, pvalue);
        } else {
            series1->append(foldchange, pvalue);
        }
    }

    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    m_ui->plot->chart()->addSeries(series1);
    m_ui->plot->chart()->addSeries(series2);

    if (!m_gene_highlight.isNull()) {
        QScatterSeries *series3 = new QScatterSeries();
        series3->setMarkerSize(8.0);
        series3->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        series3->setColor(Qt::darkMagenta);
        series3->setUseOpenGL(false);
        *series3 << m_gene_highlight;
        m_ui->plot->chart()->addSeries(series3);
    }

    m_ui->plot->chart()->setTitle("Volcano plot");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axisX()->setTitleText("Log2FoldChange");
    m_ui->plot->chart()->axisY()->setTitleText("-log10(p-value)");
    m_ui->plot->chart()->axisX()->setGridLineVisible(false);
    m_ui->plot->chart()->axisX()->setLabelsVisible(true);
    m_ui->plot->chart()->axisY()->setGridLineVisible(false);
    m_ui->plot->chart()->axisY()->setLabelsVisible(true);
}

void AnalysisDEA::updateTable()
{   
    // data model
    const int columns = 4;
    const int rows = m_results_rows.size();
    QStandardItemModel *model = new QStandardItemModel(rows, columns, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Gene")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("FDR")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("p-value")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("log2FoldChange")));

    int high_confidence_de = 0;
    // populate
    for (uword i = 0; i < m_results.n_rows; ++i) {
        const QString gene = QString::fromStdString(m_results_rows.at(i));
        const double fdr = m_results.at(i, 5);
        const QString fdr_str = QString::number(fdr);
        const double pvalue = m_results.at(i, 4);
        const QString pvalue_str = QString::number(pvalue);
        const double foldchange = m_results.at(i, 1);
        const QString foldhchange_str = QString::number(foldchange);
        QStandardItem *gene_item = new QStandardItem(gene);
        gene_item->setData(gene, Qt::DisplayRole);
        gene_item->setData(gene, Qt::UserRole);
        QStandardItem *fdr_item = new QStandardItem(fdr_str);
        fdr_item->setData(fdr_str, Qt::DisplayRole);
        fdr_item->setData(fdr_str, Qt::UserRole);
        QStandardItem *pvalue_item = new QStandardItem(pvalue_str);
        pvalue_item->setData(pvalue_str, Qt::DisplayRole);
        pvalue_item->setData(pvalue_str, Qt::UserRole);
        QStandardItem *foldchange_item = new QStandardItem(foldhchange_str);
        foldchange_item->setData(foldhchange_str, Qt::DisplayRole);
        foldchange_item->setData(foldhchange_str, Qt::UserRole);
        if (fdr <= m_ui->fdr->value() && std::abs(foldchange) >= m_ui->foldchange->value()) {
            gene_item->setBackground(Qt::red);
            fdr_item->setBackground(Qt::red);
            pvalue_item->setBackground(Qt::red);
            foldchange_item->setBackground(Qt::red);
            ++high_confidence_de;
        }
        model->setItem(i, 0, gene_item);
        model->setItem(i, 1, fdr_item);
        model->setItem(i, 2, pvalue_item);
        model->setItem(i, 3, foldchange_item);
    }

    // update total number of DE genes
    m_ui->total_genes->setText(QString::number(high_confidence_de));

    // sorting model
    m_proxy->setSourceModel(model);
    m_proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setSortRole(Qt::UserRole);
    m_ui->tableview->setModel(m_proxy.data());

    // settings for the table
    m_ui->tableview->setSortingEnabled(true);
    m_ui->tableview->setShowGrid(true);
    m_ui->tableview->setWordWrap(true);
    m_ui->tableview->setAlternatingRowColors(true);
    m_ui->tableview->sortByColumn(1, Qt::AscendingOrder);

    m_ui->tableview->setFrameShape(QFrame::StyledPanel);
    m_ui->tableview->setFrameShadow(QFrame::Sunken);
    m_ui->tableview->setGridStyle(Qt::SolidLine);
    m_ui->tableview->setCornerButtonEnabled(false);
    m_ui->tableview->setLineWidth(1);

    m_ui->tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->tableview->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_ui->tableview->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSortIndicatorShown(true);
    m_ui->tableview->verticalHeader()->hide();

    m_ui->tableview->model()->submit(); // support for caching (speed up)
}

void AnalysisDEA::run()
{
    bool recomputeFilter = false;
    bool recomputeNorm = false;

    if (m_ui->normalization_deseq->isChecked()
            && m_normalization != SettingsWidget::NormalizationMode::DESEQ) {
        m_normalization = SettingsWidget::NormalizationMode::DESEQ;
        recomputeNorm = true;
    }

    if (m_ui->normalization_scran->isChecked() &&
            m_normalization != SettingsWidget::NormalizationMode::SCRAN) {
        m_normalization = SettingsWidget::NormalizationMode::SCRAN;
        recomputeNorm = true;
    }

    if (m_reads_threshold != m_ui->reads_threshold->value()) {
        m_reads_threshold = m_ui->reads_threshold->value();
        recomputeFilter = true;
    }

    if (m_genes_threshold != m_ui->genes_threshold->value()) {
        m_genes_threshold = m_ui->genes_threshold->value();
        recomputeFilter = true;
    }

    if (m_ind_reads_treshold != m_ui->individual_reads_threshold->value()) {
        m_ind_reads_treshold = m_ui->individual_reads_threshold->value();
        recomputeFilter = true;
    }

    if (m_spots_threshold != m_ui->spots_threshold->value()) {
        m_spots_threshold = m_ui->spots_threshold->value();
        recomputeFilter = true;
    }

    STData::STDataFrame data = m_data;
    if (recomputeFilter || recomputeNorm) {
        // filter the data
        data = STData::filterDataFrame(data,
                                       m_ind_reads_treshold,
                                       m_reads_threshold,
                                       m_genes_threshold,
                                       m_spots_threshold);
        // recompute conditions
        m_conditions.clear();
        for (const auto &spot : data.spots) {
            if (spot.split("_").first().toInt() == 0) {
                m_conditions.push_back("A");
            } else {
                m_conditions.push_back("B");
            }
        }
        // clear plot and table
        m_ui->plot->chart()->removeAllSeries();
        m_proxy->clear();
        m_ui->tableview->reset();
        m_ui->tableview->update();
        // initialize progress bar
        m_ui->progressBar->setRange(0,0);
        // disable controls
        m_ui->run->setEnabled(false);
        m_ui->exportTable->setEnabled(false);
        m_ui->searchField->setEnabled(false);
        // initialize worker
        QFuture<void> future = QtConcurrent::run(this, &AnalysisDEA::runDEAAsync, data);
        m_watcher.setFuture(future);
    } else {
        slotDEAComputed();
    }
}

void AnalysisDEA::runDEAAsync(const STData::STDataFrame &data)
{
    // Convert rows and columns to a format that R understands
    std::vector<std::string> rows;
    std::vector<std::string> cols;
    std::transform(data.spots.begin(), data.spots.end(), std::back_inserter(rows),
                   [](auto spot) {return spot.toStdString();});
    std::transform(data.genes.begin(), data.genes.end(), std::back_inserter(cols),
                   [](auto gene) {return gene.toStdString();});

    qDebug() << "Computing DEA Asynchronously. Rows="
             << data.counts.n_rows << ", columns=" << data.counts.n_cols;

    // Make the DEA call
    RInterface::computeDEA(data.counts, rows, cols, m_conditions, m_normalization,
                           m_results, m_results_rows, m_results_cols);
}

void AnalysisDEA::slotDEAComputed()
{
    // stop progress bar
    m_ui->progressBar->setMaximum(10);
    // enable run button
    m_ui->run->setEnabled(true);

    // check that the DE genes were computed
    if (m_results_rows.empty() || m_results_cols.empty() || m_results.empty()) {
        QMessageBox::critical(this,
                              tr("DEA Analysis"),
                              tr("There was an error performing the DEA"));
        return;
    }

    // enable controls
    m_ui->exportTable->setEnabled(true);
    m_ui->exportPlot->setEnabled(true);
    m_ui->searchField->setEnabled(true);

    // update table with fdr and foldchange
    updateTable();

    // update plot with fdr and foldchange
    updatePlot();
}

void AnalysisDEA::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("Save Volcano Plot"));
}

void AnalysisDEA::customMenuRequested(const QPoint &pos)
{
    const QModelIndex index = m_ui->tableview->indexAt(pos);
    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        menu->addAction(new QAction(tr("Copy"), this));
        if (menu->exec(m_ui->tableview->viewport()->mapToGlobal(pos))) {
            const QString text = m_proxy->mapToSource(index).data().toString();
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(text);
        }
    }
}

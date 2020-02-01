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

#include "math/Common.h"
#include "ui_analysisDEA.h"

#include <random>

AnalysisDEA::AnalysisDEA(const STData::STDataFrame &datasetsA,
                         const STData::STDataFrame &datasetsB,
                         const QString &nameA,
                         const QString &nameB,
                         QWidget *parent,
                         Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisDEA)
    , m_dataA(datasetsA)
    , m_dataB(datasetsB)
    , m_reads_threshold(-1)
    , m_genes_threshold(-1)
    , m_spots_threshold(-1)
    , m_results()
{
    m_ui->setupUi(this);

    // default values
    m_ui->adj_pvalue->setValue(0.1);
    m_ui->foldchange->setValue(1.0);
    m_ui->exportTable->setEnabled(false);
    m_ui->searchField->setEnabled(false);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    m_ui->log_scale->setChecked(false);
    m_ui->searchField->setClearButtonEnabled(true);
    m_ui->conditionA->setText(nameA);
    m_ui->conditionB->setText(nameB);
    m_ui->normalization_raw->setChecked(true);
    m_ui->reads_threshold->setValue(1);
    m_ui->genes_threshold->setValue(10);
    m_ui->spots_threshold->setValue(10);
    m_proxy.reset(new QSortFilterProxyModel());

    // create connections
    connect(m_ui->searchField,
            &QLineEdit::textChanged,
            m_proxy.data(),
            &QSortFilterProxyModel::setFilterFixedString);
    connect(m_ui->run, &QPushButton::clicked, this, &AnalysisDEA::slotRun);
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
        stream << "Gene" << "\t" << "adj_pvalue" << "\t" << "pvalue" << "\t" << "logFoldChange" << endl;
        // write values
        for (const auto &res : m_results) {
            if (res.adj_pvalue <= m_ui->adj_pvalue->value()
                    && std::fabs(res.logfc) >= m_ui->foldchange->value()) {
                stream << res.gene << "\t" << res.adj_pvalue << "\t" << res.pvalue << "\t" << res.logfc << endl;
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

    // Check if any elements are selected
    if (selected_indexes.empty()) {
        m_gene_highlight = QPointF();
        return;
    }

    // update the highlight coordinate and refresh the plot
    const int row_index = selected_indexes.first().row();
    const double pvalue = m_results.at(row_index).log_pvalue;
    const double foldchange = m_results.at(row_index).logfc;
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
    for (const auto &res : m_results) {
        if (res.adj_pvalue <= m_ui->adj_pvalue->value()
                && std::fabs(res.logfc) >= m_ui->foldchange->value()) {
            series2->append(res.logfc, res.log_pvalue);
        } else {
            series1->append(res.logfc, res.log_pvalue);
        }
    }
    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    m_ui->plot->chart()->addSeries(series1);
    m_ui->plot->chart()->addSeries(series2);

    // If any gene must be highlighted
    if (!m_gene_highlight.isNull()) {
        QScatterSeries *series3 = new QScatterSeries();
        series3->setMarkerSize(8.0);
        series3->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        series3->setColor(Qt::darkMagenta);
        series3->setUseOpenGL(false);
        *series3 << m_gene_highlight;
        m_ui->plot->chart()->addSeries(series3);
    }

    m_ui->plot->chart()->setTitle(tr("Volcano plot"));
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setTitleText(tr("LogFoldChange"));
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setGridLineVisible(false);
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setLabelsVisible(true);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setTitleText(tr("-log10(p-value)"));
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setGridLineVisible(false);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setLabelsVisible(true);
}

//TODO create a model for the table so we only need to update the reference
void AnalysisDEA::updateTable()
{   
    // data model
    const int columns = 4;
    const int rows = m_results.size();
    QStandardItemModel *model = new QStandardItemModel(rows, columns, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Gene")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Adj. p-value")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("p-value")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("logFoldChange")));

    // populate
    int high_confidence_de = 0;
    #pragma omp parallel for reduction(+ : high_confidence_de)
    for (int i = 0; i < rows; ++i) {
        const auto res = m_results.at(i);
        const QString gene = res.gene;
        const QString adj_pvalue_str = QString::number(res.adj_pvalue);
        const QString pvalue_str = QString::number(res.pvalue);
        const QString foldchange_str = QString::number(res.logfc);
        QStandardItem *gene_item = new QStandardItem(gene);
        gene_item->setData(gene, Qt::DisplayRole);
        gene_item->setData(gene, Qt::UserRole);
        QStandardItem *adj_pvalue_item = new QStandardItem(adj_pvalue_str);
        adj_pvalue_item->setData(res.adj_pvalue, Qt::DisplayRole);
        adj_pvalue_item->setData(res.adj_pvalue, Qt::UserRole);
        QStandardItem *pvalue_item = new QStandardItem(pvalue_str);
        pvalue_item->setData(res.pvalue, Qt::DisplayRole);
        pvalue_item->setData(res.pvalue, Qt::UserRole);
        QStandardItem *foldchange_item = new QStandardItem(foldchange_str);
        foldchange_item->setData(res.logfc, Qt::DisplayRole);
        foldchange_item->setData(res.logfc, Qt::UserRole);
        if (res.adj_pvalue <= m_ui->adj_pvalue->value()
                && std::fabs(res.logfc) >= m_ui->foldchange->value()) {
            gene_item->setBackground(Qt::red);
            adj_pvalue_item->setBackground(Qt::red);
            pvalue_item->setBackground(Qt::red);
            foldchange_item->setBackground(Qt::red);
            ++high_confidence_de;
        }
        model->setItem(i, 0, gene_item);
        model->setItem(i, 1, adj_pvalue_item);
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

void AnalysisDEA::slotRun()
{
    bool recompute = false;

    if (m_reads_threshold != m_ui->reads_threshold->value()) {
        m_reads_threshold = m_ui->reads_threshold->value();
        recompute = true;
    }
    if (m_genes_threshold != m_ui->genes_threshold->value()) {
        m_genes_threshold = m_ui->genes_threshold->value();
        recompute = true;
    }
    if (m_spots_threshold != m_ui->spots_threshold->value()) {
        m_spots_threshold = m_ui->spots_threshold->value();
        recompute = true;
    }

    SettingsWidget::NormalizationMode normalization = SettingsWidget::RAW;
    if (m_ui->normalization_rel->isChecked()) {
        normalization = SettingsWidget::REL;
    } else if (m_ui->normalization_cpm->isChecked()) {
        normalization = SettingsWidget::CPM;
    }

    if (m_normalization != normalization) {
        m_normalization = normalization;
        recompute = true;
    }

    if (recompute) {
        // Filter data
        STData::STDataFrame dataA = m_dataA;
        STData::STDataFrame dataB = m_dataB;
        dataA = STData::filterCounts(dataA,
                                 m_reads_threshold,
                                 m_genes_threshold,
                                 m_spots_threshold);
        dataB = STData::filterCounts(dataB,
                                     m_reads_threshold,
                                     m_genes_threshold,
                                     m_spots_threshold);
        // Intersect genes
        QSet<QString> genesA = QSet<QString>(dataA.genes.begin(), dataA.genes.end());
        QSet<QString> genesB = QSet<QString>(dataB.genes.begin(), dataB.genes.end());
        const QList<QString> shared_genes = genesA.intersect(genesB).values();
        const int num_shared_genes = shared_genes.size();
        if (num_shared_genes > 10) {
            // keep only the shared genes in the data matrix (same order)
            uvec to_keepA(num_shared_genes);
            uvec to_keepB(num_shared_genes);
            #pragma omp parallel for
            for (int i = 0; i < num_shared_genes; ++i) {
                const QString &shared_gene = shared_genes.at(i);
                to_keepA.at(i) = dataA.genes.indexOf(shared_gene);
                to_keepB.at(i) = dataB.genes.indexOf(shared_gene);
            }
            dataA.counts = dataA.counts.cols(to_keepA);
            dataA.genes = shared_genes;
            dataB.counts = dataB.counts.cols(to_keepB);
            dataB.genes = shared_genes;
        } else {
            QMessageBox::critical(this,
                                  tr("DEA Analysis"),
                                  tr("There are not enough shared genes between the selections."));
            return;
        }

        // Normalize and log matrix of counts
        mat A = STData::normalizeCounts(dataA, m_normalization).counts;
        mat B = STData::normalizeCounts(dataB, m_normalization).counts;
        if (m_ui->log_scale) {
            A = log1p(A);
            B = log1p(B);
        }

        // clear plot and table
        m_ui->plot->chart()->removeAllSeries();
        m_proxy->invalidate();
        m_ui->tableview->reset();
        m_ui->tableview->update();
        // initialize progress bar
        m_ui->progressBar->setRange(0,0);
        // disable controls
        m_ui->run->setEnabled(false);
        m_ui->exportTable->setEnabled(false);
        m_ui->searchField->setEnabled(false);
        // initialize worker
        QFuture<void> future = QtConcurrent::run(this,
                                                 &AnalysisDEA::runDEA,
                                                 A,
                                                 B,
                                                 shared_genes);
        m_watcher.setFuture(future);
    } else {
        slotDEAComputed();
    }
}

void AnalysisDEA::runDEA(const mat &A, const mat &B, const QList<QString> genes)
{
    qDebug() << "Computing DEA for " << A.n_cols << " genes and " << A.n_rows
             << " spots in A and " << B.n_rows << " spots in B";

    // Compute p-values
    std::vector<double> pvals(A.n_cols);
    #pragma omp parallel for
    for (uword j = 0; j < A.n_cols; ++j) {
        const vec Avec = conv_to<vec>::from(A.col(j));
        const vec Bvec = conv_to<vec>::from(B.col(j));
        const double pval = STMath::wilcoxon_rank_test(Avec, Bvec);
        pvals.at(j) = pval;
    }

    // compute adjusted p-values
    const std::vector<double> adj_pvalues = STMath::p_adjustBH(pvals);

    // compute log fold-changes
    const vec rowmeansA = conv_to<vec>::from(mean(expm1(A),0));
    const vec rowmeansB = conv_to<vec>::from(mean(expm1(B),0));
    const double pseudocount = std::numeric_limits<double>::epsilon();
    const vec logfc = log(rowmeansA + pseudocount) -  log(rowmeansB + pseudocount);

    m_results.resize(pvals.size());
    // populate results
    #pragma omp parallel for
    for (size_t i = 0; i < pvals.size(); ++i) {
        DEResult res;
        res.pvalue = pvals.at(i);
        if (std::isfinite(res.pvalue)) {
            res.adj_pvalue = std::clamp(adj_pvalues.at(i), 0.0, 1.0);
            res.gene = genes.at(i);
            res.logfc = logfc.at(i);
            res.log_pvalue = -std::log10(res.pvalue + pseudocount);
            m_results[i] = res;
        }
    }
}

void AnalysisDEA::slotDEAComputed()
{
    // stop progress bar
    m_ui->progressBar->setMaximum(10);
    // enable run button
    m_ui->run->setEnabled(true);

    // check that the DE genes were computed
    if (m_results.empty()) {
        QMessageBox::critical(this,
                              tr("DEA Analysis"),
                              tr("There was an error performing the DEA"));
        return;
    }

    // enable controls
    m_ui->exportTable->setEnabled(true);
    m_ui->exportPlot->setEnabled(true);
    m_ui->searchField->setEnabled(true);

    // update table
    updateTable();

    // update plot
    updatePlot();
}

void AnalysisDEA::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("Save Volcano Plot"));
}

void AnalysisDEA::customMenuRequested(const QPoint &pos)
{
    // to allow to copy the gene name from the table
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

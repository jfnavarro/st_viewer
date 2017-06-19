#include "AnalysisDEA.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QChartView>
#include <QScatterSeries>
#include <QValueAxis>
#include <QLogValueAxis>

#include <string>

#include "math/RInterface.h"

#include "ui_analysisDEA.h"

AnalysisDEA::AnalysisDEA(const STData::STDataFrame &data1,
                         const STData::STDataFrame &data2,
                         QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisDEA)
    , m_initialized(false)
    , m_normalization(SettingsWidget::NormalizationMode::DESEQ)
{
    m_ui->setupUi(this);

    m_dataA = data1.counts;
    m_dataB = data2.counts;
    std::transform(data1.spots.begin(), data1.spots.end(), std::back_inserter(m_rowsA),
                   [](auto spot) {return std::to_string(spot.first) + "x" + std::to_string(spot.second);});
    std::transform(data2.spots.begin(), data2.spots.end(), std::back_inserter(m_rowsB),
                   [](auto spot) {return std::to_string(spot.first) + "x" + std::to_string(spot.second);});
    std::transform(data1.genes.begin(), data1.genes.end(), std::back_inserter(m_colsA),
                   [](auto gene) {return gene.toStdString();});
    std::transform(data2.genes.begin(), data2.genes.end(), std::back_inserter(m_colsB),
                   [](auto gene) {return gene.toStdString();});

    m_ui->fdr->setValue(0.1);
    m_ui->foldchange->setValue(1.0);
    m_ui->normalization_deseq->setChecked(true);
    m_ui->exportTable->setEnabled(false);
    m_ui->searchField->setEnabled(false);
    connect(m_ui->run, &QPushButton::clicked, this, &AnalysisDEA::run);
    connect(m_ui->exportTable, &QPushButton::clicked, this, &AnalysisDEA::exportTable);
}

AnalysisDEA::~AnalysisDEA()
{
}

void AnalysisDEA::exportTable()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Export DE Genes"),
                                                    QDir::homePath(),
                                                    QString("%1").arg(tr("Text Files (*.tsv)")));
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
            const double fdr = 1 - m_results.at(i, 5);
            const double pvalue = m_results.at(i, 4);
            const double foldchange = m_results.at(i, 1);
            if (fdr <= m_ui->fdr->value() && std::abs(foldchange) >= m_ui->foldchange->value()) {
                stream << gene << "\t" << fdr << "\t" << pvalue << "\t" << foldchange << endl;
            }
        }
    }

}

void AnalysisDEA::updatePlot()
{
    QScatterSeries *series1 = new QScatterSeries();
    QScatterSeries *series2 = new QScatterSeries();

    series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series1->setMarkerSize(5.0);
    series1->setColor(Qt::blue);
    series1->setUseOpenGL(true);

    series2->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series2->setMarkerSize(5.0);
    series2->setColor(Qt::red);
    series2->setUseOpenGL(true);

    // populate
    for (uword i = 0; i < m_results.n_rows; ++i) {
        const double fdr = 1 - m_results.at(i, 5);
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

    m_ui->plot->chart()->setTitle("Volcano plot");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axisX()->setTitleText("Log2FoldChange");
    m_ui->plot->chart()->axisY()->setTitleText("-log10(p-value)");
}

void AnalysisDEA::updateTable()
{
    m_ui->exportTable->setEnabled(true);
    m_ui->searchField->setEnabled(true);

    m_ui->searchField->setClearButtonEnabled(true);

    // data model
    const int columns = 4;
    const int rows = m_results_rows.size();
    QStandardItemModel *model = new QStandardItemModel(rows, columns, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Gene")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("FDR")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("p-value")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("log2FoldChange")));

    // populate
    for (uword i = 0; i < m_results.n_rows; ++i) {
        const QString gene = QString::fromStdString(m_results_rows.at(i));
        const double fdr = 1 - m_results.at(i, 5);
        const double pvalue = m_results.at(i, 4);
        const double foldchange = m_results.at(i, 1);
        QStandardItem *gene_item = new QStandardItem(gene);
        QStandardItem *fdr_item = new QStandardItem(QString::number(fdr));
        QStandardItem *pvalue_item = new QStandardItem(QString::number(pvalue));
        QStandardItem *foldchange_item = new QStandardItem(QString::number(foldchange));
        if (fdr <= m_ui->fdr->value() && std::abs(foldchange) >= m_ui->foldchange->value()) {
            gene_item->setBackground(Qt::red);
            fdr_item->setBackground(Qt::red);
            pvalue_item->setBackground(Qt::red);
            foldchange_item->setBackground(Qt::red);
        }
        model->setItem(i,0,gene_item);
        model->setItem(i,1,fdr_item);
        model->setItem(i,2,pvalue_item);
        model->setItem(i,3,foldchange_item);
    }

    // sorting model
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_ui->tableview->setModel(proxy);

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
    m_ui->tableview->setSelectionMode(QAbstractItemView::NoSelection);
    m_ui->tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_ui->tableview->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSortIndicatorShown(true);
    m_ui->tableview->verticalHeader()->hide();

    m_ui->tableview->model()->submit(); // support for caching (speed up)

    // Connect the search field signal
    connect(m_ui->searchField,
            &QLineEdit::textChanged,
            proxy,
            &QSortFilterProxyModel::setFilterFixedString);
}

void AnalysisDEA::run()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    if (m_ui->normalization_deseq->isChecked()
            && m_normalization != SettingsWidget::NormalizationMode::DESEQ) {
        m_normalization = SettingsWidget::NormalizationMode::DESEQ;
        m_initialized = false;
    } else if (m_ui->normalization_scran->isChecked() &&
               m_normalization != SettingsWidget::NormalizationMode::SCRAN) {
        m_normalization = SettingsWidget::NormalizationMode::SCRAN;
        m_initialized = false;
    }

    if (!m_initialized) {
        qDebug() << "Computing DEA";
        RInterface::computeDEA(m_dataA, m_dataB, m_rowsA, m_rowsB, m_colsA, m_colsB,
                               m_normalization, m_results, m_results_rows, m_results_cols);
        m_initialized = true;
    }

    // update table with fdr and foldchange
    updateTable();

    // update plot with fdr and foldchange
    updatePlot();

    QGuiApplication::restoreOverrideCursor();
}

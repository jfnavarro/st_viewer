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

constexpr int pvalue_index =  1;
constexpr int fdr_index =  2;
constexpr int fc_index = 3;

namespace {

double bimodLikData(const vec &x, const double xmin = 0) {
  // x1 and x2 are 2 vectors representing 2 models
  // x1 for 0 values -> on/off distribution model
  // x2 for positive values -> normal, continuous distribution
  const std::vector<double> x1 = conv_to<std::vector<double>>::from(x.elem(find(x <= xmin)));
  const std::vector<double> x2 = conv_to<std::vector<double>>::from(x.elem(find(x > xmin)));
  const size_t x1_ele = x1.size();
  const size_t x2_ele = x2.size();
  // estimate proportion of positive spots (count > xmin)
  // use 1e-5 as min and 1-1e-5 as max
  double xal = 1.0;
  if (x2_ele > 0 && x1_ele > 0) {
      xal = x2_ele / x1_ele;
  }
  const double min = 1e-5;
  const double max = 1 - min;
  if (xal < min) {
      xal = min;
  } else if (xal > max) {
      xal = max;
  }
  // likelihood for observing x1, 1-xal is expected ratio of 0 values
  const double likA = x1_ele * std::log(1 - xal);
  // calculate variance for x2, to be used in dnorm to calculate prob distr
  const double sd = x2_ele < 2 ? 1 : STMath::std_dev(x2);
  // likelihood for observing x2
  const double mean = STMath::mean(x2);
  // pdf for x2 (normal distribution)
  double sum_dnorm = 0;
  for (const double &x : x2) {
      sum_dnorm += std::log(STMath::normal(x, mean, sd));
  }
  const double likB = x2_ele * std::log(xal) + sum_dnorm;
  return likA + likB;
}

//This method was adapted from the method published by [McDavid
//et al. 2013](https://doi.org/10.1093/bioinformatics/bts714).
double differentialLRT(const vec &x, const vec &y, const double xmin = 0) {
  const double lrtX = bimodLikData(x, xmin);
  const double lrtY = bimodLikData(y, xmin);
  const double lrtZ = bimodLikData(arma::join_cols(x,y), xmin);
  const double lrt_diff = 2 * (lrtX + lrtY - lrtZ);
  if (std::isfinite(lrtX) && std::isfinite(lrtY) && std::isfinite(lrtZ) && lrt_diff > 0) {
      const double pvalue = STMath::chi_squared_cdf(lrt_diff, 3.0);
      return std::isfinite(pvalue) ? pvalue : 1.0;
  }
  return 1.0;
}
}

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
    m_ui->fdr->setValue(0.1);
    m_ui->foldchange->setValue(1.0);
    m_ui->exportTable->setEnabled(false);
    m_ui->searchField->setEnabled(false);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    m_ui->searchField->setClearButtonEnabled(true);
    m_ui->conditionA->setText(nameA);
    m_ui->conditionB->setText(nameB);
    m_ui->normalization_raw->setChecked(true);
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
        stream << "Gene" << "\t" << "FDR" << "\t" << "p-value" << "\t" << "log2FoldChange" << endl;
        // write values
        for (const auto &res : m_results) {
            if (res.fdr <= m_ui->fdr->value() && std::abs(res.log2fc) >= m_ui->foldchange->value()) {
                stream << res.gene << "\t" << res.fdr << "\t" << res.pvalue << "\t" << res.log2fc << endl;
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
    const double pvalue = m_results.at(row_index).log_pvalue;
    const double foldchange = m_results.at(row_index).log2fc;
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
    #pragma omp parallel for
    for (const auto &res : m_results) {
        if (res.fdr <= m_ui->fdr->value() && std::abs(res.log2fc) >= m_ui->foldchange->value()) {
            series2->append(res.log2fc, res.log_pvalue);
        } else {
            series1->append(res.log2fc, res.log_pvalue);
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

    m_ui->plot->chart()->setTitle(tr("Volcano plot"));
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setTitleText(tr("Log2FoldChange"));
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
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("FDR")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("p-value")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("log2FoldChange")));

    int high_confidence_de = 0;
    // populate
    #pragma omp parallel for
    for (int i = 0; i < rows; ++i) {
        const auto res = m_results.at(i);
        const QString gene = res.gene;
        const QString fdr_str = QString::number(res.fdr);
        const QString pvalue_str = QString::number(res.pvalue);
        const QString foldchange_str = QString::number(res.log2fc);
        QStandardItem *gene_item = new QStandardItem(gene);
        gene_item->setData(gene, Qt::DisplayRole);
        gene_item->setData(gene, Qt::UserRole);
        QStandardItem *fdr_item = new QStandardItem(fdr_str);
        fdr_item->setData(res.fdr, Qt::DisplayRole);
        fdr_item->setData(res.fdr, Qt::UserRole);
        QStandardItem *pvalue_item = new QStandardItem(pvalue_str);
        pvalue_item->setData(res.pvalue, Qt::DisplayRole);
        pvalue_item->setData(res.pvalue, Qt::UserRole);
        QStandardItem *foldchange_item = new QStandardItem(foldchange_str);
        foldchange_item->setData(res.log2fc, Qt::DisplayRole);
        foldchange_item->setData(res.log2fc, Qt::UserRole);
        if (res.fdr <= m_ui->fdr->value() && std::abs(res.log2fc) >= m_ui->foldchange->value()) {
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

    if (recompute) {
        // Filter data
        STData::STDataFrame dataA = m_dataA;
        STData::STDataFrame dataB = m_dataB;
        dataA = STData::filterCounts(dataA,
                                 m_ui->reads_threshold->value(),
                                 m_ui->genes_threshold->value(),
                                 m_ui->spots_threshold->value());
        dataB = STData::filterCounts(dataB,
                                 m_ui->reads_threshold->value(),
                                 m_ui->genes_threshold->value(),
                                 m_ui->spots_threshold->value());
        // Intersect genes
        QSet<QString> genesA = QSet<QString>::fromList(dataA.genes);
        QSet<QString> genesB = QSet<QString>::fromList(dataB.genes);
        const QList<QString> shared_genes = genesA.intersect(genesB).toList();
        const unsigned num_shared_genes = shared_genes.size();
        if (num_shared_genes > 0) {
            // keep only the shared genes in the data matrix (same order)
            uvec to_keepA(num_shared_genes);
            uvec to_keepB(num_shared_genes);
            #pragma omp parallel for
            for (unsigned i = 0; i < num_shared_genes; ++i) {
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
                                  tr("There are no common genes between the selections."));
            return;
        }
        // Normalize and log matrix of counts
        SettingsWidget::NormalizationMode normalization = SettingsWidget::RAW;
        if (m_ui->normalization_rel->isChecked()) {
            normalization = SettingsWidget::REL;
        } else if (m_ui->normalization_cpm->isChecked()) {
            normalization = SettingsWidget::CPM;
        }
        mat A = STData::normalizeCounts(dataA, normalization).counts;
        mat B = STData::normalizeCounts(dataB, normalization).counts;
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

    std::vector<double> pvals(A.n_cols);
    #pragma omp parallel for
    for (uword j = 0; j < A.n_cols; ++j) {
        const vec Avec = A.col(j);
        const vec Bvec = B.col(j);
        const double pval = differentialLRT(Avec, Bvec);
        pvals.at(j) = pval;
    }

    std::vector<double> fdrs = STMath::p_adjust(pvals);
    const vec rowmeansA = conv_to<vec>::from(mean(A,0));
    const vec rowmeansB = conv_to<vec>::from(mean(B,0));
    const vec log2fcs = log2(rowmeansA / rowmeansB);

    m_results.clear();
    #pragma omp parallel for
    for (size_t i = 0; i < pvals.size(); ++i) {
        DEResult res;
        res.pvalue = pvals.at(i);
        res.fdr = fdrs.at(i);
        res.gene = genes.at(i);
        res.log2fc = log2fcs.at(i);
        res.log_pvalue = -std::log10(res.pvalue + std::numeric_limits<double>::epsilon());
        m_results.push_back(res);
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

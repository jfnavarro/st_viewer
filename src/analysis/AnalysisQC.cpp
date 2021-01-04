#include "AnalysisQC.h"

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QDebug>
#include "ui_analysisQC.h"

AnalysisQC::AnalysisQC(const STData::STDataFrame &data,
                       QWidget *parent)
    : QWidget(parent, Qt::Window)
    , m_ui(new Ui::analysisQC)
{
    m_ui->setupUi(this);

    qDebug() << "QC computing data";

    // compute the stats
    const vec nonzero_col = conv_to<vec>::from(sum(data.counts != 0, 0));
    const vec nonzero_row = conv_to<vec>::from(sum(data.counts != 0, 1));
    const colvec rowsums = sum(data.counts, 1);
    const QString max_transcripts_spot = QString::number(rowsums.max());
    const QString max_genes_spot = QString::number(nonzero_row.max());
    const QString num_genes = QString::number(data.counts.n_cols);
    const QString num_spots = QString::number(data.counts.n_rows);
    const QString total_transcripts = QString::number(accu(data.counts));
    const QString avg_genes = QString::number(mean(nonzero_row));
    const QString avg_transcritps = QString::number(mean(rowsums));
    const QString std_genes = QString::number(stddev(nonzero_row));
    const QString std_transcripts = QString::number(stddev(rowsums));
    uvec hist_genes;
    uvec hist_spots;
    try {
        hist_genes = hist(nonzero_row);
        hist_spots = hist(nonzero_col);
    } catch (const std::logic_error &e) {
        qDebug() << "Error computing QC histograms " << e.what();
    }

    qDebug() << "QC data generated";

    // populate the line edits
    m_ui->maxTranscripts->setText(max_transcripts_spot);
    m_ui->maxGenes->setText(max_genes_spot);
    m_ui->totalGenes->setText(num_genes);
    m_ui->totalSpots->setText(num_spots);
    m_ui->totalTranscripts->setText(total_transcripts);
    m_ui->meanGenes->setText(avg_genes);
    m_ui->meanTranscripts->setText(avg_transcritps);
    m_ui->stdGenes->setText(std_genes);
    m_ui->stdTranscripts->setText(std_transcripts);

    // populate the plots
    QBarSet *genes = new QBarSet("Genes");
    QBarSeries *series_genes = new QBarSeries();
    for (const auto &value : hist_genes) {
        *genes << static_cast<int>(value);
    }

    QBarSet *spots = new QBarSet(tr("Spots"));
    QBarSeries *series_spots = new QBarSeries();
    for (const auto &value : hist_spots) {
        *spots << static_cast<int>(value);
    }

    series_genes->append(genes);
    series_spots->append(spots);

    // populate histogram genes
    m_ui->genesPlot->chart()->addSeries(series_genes);
    m_ui->genesPlot->chart()->setTitle(tr("Histogram genes"));
    m_ui->genesPlot->chart()->setAnimationOptions(QChart::SeriesAnimations);
    m_ui->genesPlot->chart()->createDefaultAxes();
    m_ui->genesPlot->chart()->axes(Qt::Horizontal).first()->setTitleText(tr("Genes detected"));
    m_ui->genesPlot->chart()->axes(Qt::Vertical).first()->setTitleText(tr("#Spots"));

    // populate histogram spots
    m_ui->spotsPlot->chart()->addSeries(series_spots);
    m_ui->spotsPlot->chart()->setTitle(tr("Histogram spots"));
    m_ui->spotsPlot->chart()->setAnimationOptions(QChart::SeriesAnimations);
    m_ui->spotsPlot->chart()->createDefaultAxes();
    m_ui->spotsPlot->chart()->axes(Qt::Horizontal).first()->setTitleText(tr("Spots detected"));
    m_ui->spotsPlot->chart()->axes(Qt::Vertical).first()->setTitleText(tr("#Genes"));

    connect(m_ui->exportGenes, &QPushButton::clicked, [=]() {slotExportPlot(1);});
    connect(m_ui->exportSpots, &QPushButton::clicked, [=]() {slotExportPlot(2);});
}

AnalysisQC::~AnalysisQC()
{
}

void AnalysisQC::slotExportPlot(const int type)
{
    if (type == 1) {
        m_ui->genesPlot->slotExportPlot(tr("QC Histogram (Genes)"));
    } else {
        m_ui->spotsPlot->slotExportPlot(tr("QC Histogram (Spots)"));
    }
}

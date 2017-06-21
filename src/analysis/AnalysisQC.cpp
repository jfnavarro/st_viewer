#include "AnalysisQC.h"

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QFileDialog>

#include "ui_analysisQC.h"

AnalysisQC::AnalysisQC(const STData::STDataFrame &data,
                       QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisQC)
{
    m_ui->setupUi(this);

    // compute the stats
    const colvec rowsums = sum(data.counts, 1);
    const colvec nonzero_row = STData::computeNonZeroRows(data.counts);
    const QString max_transcripts_spot = QString::number(rowsums.max());
    const QString max_genes_spot = QString::number(nonzero_row.max());
    const QString num_genes = QString::number(data.counts.n_cols);
    const QString num_spots = QString::number(data.counts.n_rows);
    const QString total_transcripts = QString::number(accu(data.counts));
    const QString avg_genes = QString::number(mean(nonzero_row));
    const QString avg_transcritps = QString::number(mean(rowsums));
    const QString std_genes = QString::number(stddev(nonzero_row));
    const QString std_transcripts = QString::number(stddev(rowsums));
    const uvec hist_genes = hist(nonzero_row, 10);
    const uvec hist_transcripts = hist(rowsums, 10);

    // populate the line edits
    m_ui->maxTranscripts->setText(max_transcripts_spot);
    m_ui->maxGenes->setText(max_genes_spot);
    m_ui->totalGenes->setText(num_genes);
    m_ui->totalSpots->setText(num_spots);
    m_ui->totalTranscripts->setText(total_transcripts);
    m_ui->medianGenes->setText(avg_genes);
    m_ui->medianTranscripts->setText(avg_transcritps);
    m_ui->stdGenes->setText(std_genes);
    m_ui->stdTranscripts->setText(std_transcripts);

    // populate the plots
    QBarSet *genes = new QBarSet("Genes");
    for(const auto &value : hist_genes) {
        *genes << value;
    }
    QBarSeries *series_genes = new QBarSeries();
    series_genes->append(genes);

    QBarSet *transcripts = new QBarSet("Transcripts");
    for (const auto &value : hist_transcripts) {
        *transcripts << value;
    }
    QBarSeries *series_transcripts = new QBarSeries();
    series_transcripts->append(transcripts);

    m_ui->genesPlot->chart()->addSeries(series_genes);
    m_ui->genesPlot->chart()->setTitle("Histogram genes");
    m_ui->genesPlot->chart()->setAnimationOptions(QChart::SeriesAnimations);
    m_ui->genesPlot->chart()->createDefaultAxes();

    m_ui->transcriptsPlot->chart()->addSeries(series_transcripts);
    m_ui->transcriptsPlot->chart()->setTitle("Histogram transcripts");
    m_ui->transcriptsPlot->chart()->setAnimationOptions(QChart::SeriesAnimations);
    m_ui->transcriptsPlot->chart()->createDefaultAxes();
    connect(m_ui->exportGenes, &QPushButton::clicked, [=]() {slotExportPlot(1);});
    connect(m_ui->exportTranscripts, &QPushButton::clicked, [=]() {slotExportPlot(2);});
}

AnalysisQC::~AnalysisQC()
{
}

void AnalysisQC::slotExportPlot(const int type)
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Histogram Plot"),
                                                    QDir::homePath(),
                                                    QString("%1;;%2;;%3")
                                                        .arg(tr("JPEG Image Files (*.jpg *.jpeg)"))
                                                        .arg(tr("PNG Image Files (*.png)"))
                                                        .arg(tr("BMP Image Files (*.bmp)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        qDebug() << "Saving the Histogram plot, the directory is not writtable";
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QPixmap image;
    if (type == 1) {
        image = m_ui->genesPlot->grab();
     } else {
        image = m_ui->transcriptsPlot->grab();
    }
    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        qDebug() << "Saving the Histogram plot, the image coult not be saved";
    }
}

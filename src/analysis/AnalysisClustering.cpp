#include "AnalysisClustering.h"

#include <QChartView>
#include <QScatterSeries>
#include <QValueAxis>
#include <QFuture>
#include <QtConcurrent>
#include <QFileDialog>

#include "math/RInterface.h"

#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisClustering)
{
    // setup UI
    m_ui->setupUi(this);
    m_ui->normalization_raw->setChecked(true);
    m_ui->theta->setValue(0.5);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    connect(m_ui->runClustering, &QPushButton::clicked, this, &AnalysisClustering::slotRun);
    connect(m_ui->exportPlot, &QPushButton::clicked, this, &AnalysisClustering::slotExportPlot);
    connect(&m_watcher, &QFutureWatcher<void>::finished,
            this, &AnalysisClustering::colorsComputed);
}

AnalysisClustering::~AnalysisClustering()
{
}


QVector<QColor> AnalysisClustering::getComputedClasses() const
{
    QStringList color_list;
    color_list << "red" << "green" << "blue" << "cyan" << "magenta"
               << "yellow" << "black" << "grey" << "darkBlue" << "darkGreen";
    QVector<QColor> computed_colors(m_colors.size());
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        computed_colors[i] = QColor(color_list.at(m_colors.at(i)));
    }
    return computed_colors;
}

void AnalysisClustering::loadData(const STData::STDataFrame &data)
{
    m_data = data;

    // compute size factors for normalization
    m_deseq_factors = RInterface::computeDESeqFactors(m_data.counts);
    m_scran_factors = RInterface::computeScranFactors(m_data.counts);
}

void AnalysisClustering::slotRun()
{
    qDebug() << "Computing spot colors asynchronously";
    // initialize progress bar
    m_ui->progressBar->setRange(0,0);
    // disable controls
    m_ui->runClustering->setEnabled(false);
    m_ui->exportPlot->setEnabled(false);
    QFuture<void> future = QtConcurrent::run(this, &AnalysisClustering::computeColorsAsync);
    m_watcher.setFuture(future);
}

void AnalysisClustering::slotExportPlot()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save t-SNE Plot"),
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
        qDebug() << "Saving the t-SNE plot, the directory is not writtable";
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QPixmap image = m_ui->plot->grab();
    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        qDebug() << "Saving the t-SNE plot, the image coult not be saved";
    }
}

void AnalysisClustering::computeColorsAsync()
{
    SettingsWidget::NormalizationMode normalization = SettingsWidget::RAW;
    if (m_ui->normalization_rel->isChecked()) {
        normalization = SettingsWidget::REL;
    } else if (m_ui->normalization_tpm->isChecked()) {
        normalization = SettingsWidget::TPM;
    } else if (m_ui->normalization_deseq->isChecked()) {
        normalization = SettingsWidget::DESEQ;
    } else if (m_ui->normalization_scran->isChecked()) {
        normalization = SettingsWidget::SCRAN;
    }

    // Normalize and log matrix of counts
    mat A = STData::normalizeCounts(m_data.counts, normalization,
                                    m_deseq_factors, m_scran_factors);
    if (m_ui->logScale->isChecked()) {
        A = log(A + 1.0);
    }

    const int no_dims = 2;
    const int perplexity = m_ui->perplexity->value();
    const double theta = m_ui->theta->value();
    const int max_iter = m_ui->max_iter->value();
    const int init_dim = m_ui->init_dims->value();
    const int num_clusters = m_ui->clusters->value();

    // Surprisingly it is much faster to call R's tsne than to use C++ implemtation....
    RInterface::spotClassification(A, num_clusters, init_dim, no_dims, perplexity,
                                   max_iter, theta, m_colors, m_reduced_coordinates);
}

void AnalysisClustering::colorsComputed()
{
    const int num_clusters = m_ui->clusters->value();
    Q_ASSERT(*std::min_element(std::begin(m_colors), std::end(m_colors)) == 0
             && *std::max_element(std::begin(m_colors), std::end(m_colors)) == (num_clusters - 1));

    // Create one serie for each different cluster (color)
    QStringList color_list;
    color_list << "red" << "green" << "blue" << "cyan" << "magenta"
               << "yellow" << "black" << "grey" << "darkBlue" << "darkGreen";
    QVector<QScatterSeries *> series_vector;
    for (int k = 0; k < num_clusters; ++k) {
        QScatterSeries *series = new QScatterSeries();
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(10.0);
        series->setColor(color_list.at(k));
        series->setUseOpenGL(false);
        series_vector.push_back(series);
    }

    const std::vector<double> x = conv_to<std::vector<double>>::from(m_reduced_coordinates.col(0));
    const std::vector<double> y = conv_to<std::vector<double>>::from(m_reduced_coordinates.col(1));
    // add the respective spot (t-SNE coordinates) to the serie it belongs to
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        const int k = m_colors.at(i);
        series_vector[k]->append(x.at(i), y.at(i));
    }

    // update the scatter plot
    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    for (auto series : series_vector) {
        m_ui->plot->chart()->addSeries(series);
    }
    m_ui->plot->chart()->setTitle("Spots colored by cluster");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->show();
    m_ui->plot->chart()->createDefaultAxes();

    // stop progress bar
    m_ui->progressBar->setMaximum(10);
    // enable controls
    m_ui->runClustering->setEnabled(true);
    m_ui->exportPlot->setEnabled(true);

    // notify the main view
    emit singalClusteringUpdated();
}

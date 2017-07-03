#include "AnalysisClustering.h"

#include <QChartView>
#include <QValueAxis>
#include <QFuture>
#include <QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPdfWriter>

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
    m_ui->tsne->setChecked(true);
    m_ui->kmeans->setChecked(true);


    connect(m_ui->runClustering, &QPushButton::clicked, this, &AnalysisClustering::slotRun);
    connect(m_ui->exportPlot, &QPushButton::clicked, this, &AnalysisClustering::slotExportPlot);
    connect(&m_watcher, &QFutureWatcher<void>::finished,
            this, &AnalysisClustering::colorsComputed);
}

AnalysisClustering::~AnalysisClustering()
{
}


QHash<Spot::SpotType, QColor> AnalysisClustering::getComputedClasses() const
{
    QStringList color_list;
    color_list << "red" << "green" << "blue" << "cyan" << "magenta"
               << "yellow" << "black" << "grey" << "darkBlue" << "darkGreen";
    QHash<Spot::SpotType, QColor> computed_colors;
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        const QColor color(color_list.at(m_colors.at(i)));
        computed_colors.insert(m_spots.at(i), color);
    }
    return computed_colors;
}

void AnalysisClustering::loadData(const STData::STDataFrame &data)
{
    // store the data
    m_data = data;
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
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Save Clustering Plot"),
                                                          QDir::homePath(),
                                                          QString("%1;;%2;;%3;;%4")
                                                          .arg(tr("JPEG Image Files (*.jpg *.jpeg)"))
                                                          .arg(tr("PNG Image Files (*.png)"))
                                                          .arg(tr("BMP Image Files (*.bmp)"))
                                                          .arg(tr("PDF Image Files (*.pdf)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        QMessageBox::critical(this,
                              tr("Save Clustering Plot"),
                              tr("The file is not writable"));
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = m_ui->plot->grab().toImage();
    if (format.toLower().contains("pdf")) {
        QPdfWriter writer(filename);
        const QPageSize size(image.size(), QPageSize::Unit::Millimeter, "custom");
        writer.setPageSize(size);
        writer.setResolution(25);
        writer.setPageMargins(QMarginsF(0,0,0,0));
        QPainter painter(&writer);
        painter.drawImage(0,0, image);
    } else if (!image.save(filename, format.toStdString().c_str(), quality)) {
        QMessageBox::critical(this,
                              tr("Save Clustering Plot"),
                              tr("The image could not be creted."));
    }
}

void AnalysisClustering::computeColorsAsync()
{
    // filter out
    STData::STDataFrame data = STData::filterDataFrame(m_data,
                                                       m_ui->individual_reads_threshold->value(),
                                                       m_ui->reads_threshold->value(),
                                                       m_ui->genes_threshold->value(),
                                                       m_ui->spots_threshold->value());
    m_spots = data.spots;

    SettingsWidget::NormalizationMode normalization = SettingsWidget::RAW;
    if (m_ui->normalization_rel->isChecked()) {
        normalization = SettingsWidget::REL;
    } else if (m_ui->normalization_tpm->isChecked()) {
        normalization = SettingsWidget::TPM;
    } else if (m_ui->normalization_deseq->isChecked()) {
        normalization = SettingsWidget::DESEQ;
        m_deseq_size_factors = RInterface::computeDESeqFactors(data.counts);
    } else if (m_ui->normalization_scran->isChecked()) {
        normalization = SettingsWidget::SCRAN;
        m_scran_size_factors = RInterface::computeScranFactors(data.counts);
    }

    // Normalize and log matrix of counts
    mat A = STData::normalizeCounts(data,
                                    m_deseq_size_factors,
                                    m_scran_size_factors,
                                    normalization).counts;
    if (m_ui->logScale->isChecked()) {
        A = log(A + 1.0);
    }

    const int no_dims = 2;
    const int perplexity = m_ui->perplexity->value();
    const double theta = m_ui->theta->value();
    const int max_iter = m_ui->max_iter->value();
    const int init_dim = m_ui->init_dims->value();
    const int num_clusters = m_ui->clusters->value();
    const bool scale = m_ui->scale->isChecked();
    const bool center = m_ui->center->isChecked();
    const bool tsne = m_ui->tsne->isChecked();
    const bool kmeans = m_ui->kmeans->isChecked();

    // Surprisingly it is much faster to call R's tsne/pca than to use C++ implementation....
    RInterface::spotClassification(A, tsne, kmeans, num_clusters, init_dim, no_dims, perplexity,
                                   max_iter, theta, scale, center, m_colors, m_reduced_coordinates);
}

void AnalysisClustering::colorsComputed()
{
    // stop progress bar
    m_ui->progressBar->setMaximum(10);
    // enable run button
    m_ui->runClustering->setEnabled(true);

    if (m_colors.empty() || m_reduced_coordinates.empty()) {
        QMessageBox::critical(this,
                              tr("Spot classification"),
                              tr("There was an error performing the classification"));
        return;
    }

    const int num_clusters = m_ui->clusters->value();
    Q_ASSERT(*std::min_element(std::begin(m_colors), std::end(m_colors)) == 0
             && *std::max_element(std::begin(m_colors), std::end(m_colors)) == (num_clusters - 1));

    // Create one serie for each different cluster (color)
    QStringList color_list;
    color_list << "red" << "green" << "blue" << "cyan" << "magenta"
               << "yellow" << "black" << "grey" << "darkBlue" << "darkGreen";
    m_series_vector.clear();
    for (int k = 0; k < num_clusters; ++k) {
        QScatterSeries *series = new QScatterSeries(this);
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(10.0);
        series->setColor(color_list.at(k));
        series->setUseOpenGL(false);
        m_series_vector.push_back(series);
        connect(series, &QScatterSeries::clicked, this, &AnalysisClustering::slotClickedPoint);
    }

    // add the respective spot (t-SNE coordinates) to the serie it belongs to
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        const int k = m_colors.at(i);
        const float x = m_reduced_coordinates.at(i,0);
        const float y = m_reduced_coordinates.at(i,1);
        m_series_vector[k]->append(x, y);
    }

    // update the scatter plot
    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    for (auto series : m_series_vector) {
        m_ui->plot->chart()->addSeries(series);
    }
    m_ui->plot->chart()->setTitle("Spots colored by cluster");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->show();
    m_ui->plot->chart()->createDefaultAxes();

    // enable export controls
    m_ui->exportPlot->setEnabled(true);

    // notify the main view
    emit singalClusteringUpdated();
}

void AnalysisClustering::slotClickedPoint(const QPointF point)
{
    // Find the closest point from all series
    const QPointF clickedPoint = point;
    float closest_x = INT_MAX;
    float closest_y = INT_MAX;
    float distance = INT_MAX;
    for (const auto series : m_series_vector) {
        for (const auto point : series->points()) {
            const float currentDistance = qSqrt((point.x() - clickedPoint.x())
                                                * (point.x() - clickedPoint.x())
                                                + (point.y() - clickedPoint.y())
                                                * (point.y() - clickedPoint.y()));
            if (currentDistance < distance) {
                distance = currentDistance;
                closest_x = point.x();
                closest_y = point.y();
            }
        }
    }

    // Find the spot coorespondign to the clicked point
    int spot_index = -1;
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        const float x = m_reduced_coordinates.at(i,0);
        const float y = m_reduced_coordinates.at(i,1);
        if (x == closest_x && y == closest_y) {
            spot_index = i;
            break;
        }
    }

    // Update the fied if valid spot was found
    if (spot_index != -1) {
        const auto spot = m_data.spots.at(spot_index);
        m_ui->selectedSpot->setText(
                    QString::number(spot.first) + "x" + QString::number(spot.second));
    }

}

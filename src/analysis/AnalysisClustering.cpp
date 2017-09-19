#include "AnalysisClustering.h"

#include <QChartView>
#include <QValueAxis>
#include <QFuture>
#include <QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPdfWriter>

#include "color/HeatMap.h"
#include "math/RInterface.h"

#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisClustering)
{
    // setup UI
    m_ui->setupUi(this);
    clear();

    connect(m_ui->runClustering, &QPushButton::clicked, this, &AnalysisClustering::slotRun);
    connect(m_ui->exportPlot, &QPushButton::clicked, this, &AnalysisClustering::slotExportPlot);
    connect(m_ui->computeClusters, &QPushButton::clicked,
            this, &AnalysisClustering::slotComputeClusters);
    connect(&m_watcher_colors, &QFutureWatcher<void>::finished,
            this, &AnalysisClustering::colorsComputed);
    connect(&m_watcher_classes, &QFutureWatcher<void>::finished,
            this, &AnalysisClustering::classesComputed);
    connect(m_ui->plot, &ChartView::signalLassoSelection,
            this, &AnalysisClustering::slotLassoSelection);
}

AnalysisClustering::~AnalysisClustering()
{
}

void AnalysisClustering::clear()
{
    m_ui->normalization_raw->setChecked(true);
    m_ui->theta->setValue(0.5);
    m_ui->center->setChecked(false);
    m_ui->scale->setChecked(false);
    m_ui->perplexity->setValue(30);
    m_ui->max_iter->setValue(1000);
    m_ui->init_dims->setValue(50);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    m_ui->runClustering->setEnabled(true);
    m_ui->tab->setCurrentIndex(0);
    m_ui->kmeans->setChecked(true);
    m_ui->individual_reads_threshold->setValue(0);
    m_ui->reads_threshold->setValue(0);
    m_ui->genes_threshold->setValue(5);
    m_ui->spots_threshold->setValue(5);
    m_ui->clusters->setValue(5);
    m_ui->logScale->setChecked(false);
    m_ui->plot->chart()->removeAllSeries();
    m_colors.clear();
    m_selected_spots.clear();
    m_reduced_coordinates.clear();
}

QHash<Spot::SpotType, QColor> AnalysisClustering::getComputedClasses() const
{
    QHash<Spot::SpotType, QColor> computed_colors;
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        const QColor color = Color::color_list.at(m_colors.at(i));
        computed_colors.insert(m_spots.at(i), color);
    }
    return computed_colors;
}

QList<Spot::SpotType> AnalysisClustering::selectedSpots() const
{
    return m_selected_spots;
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
    m_ui->computeClusters->setEnabled(false);
    m_ui->exportPlot->setEnabled(false);
    m_selected_spots.clear();
    // make the call
    QFuture<void> future = QtConcurrent::run(this, &AnalysisClustering::computeColorsAsync);
    m_watcher_colors.setFuture(future);
}

void AnalysisClustering::slotComputeClusters()
{
    qDebug() << "Estimating the number of clusters";
    // initialize progress bar
    m_ui->progressBar->setRange(0,0);
    // disable controls
    m_ui->runClustering->setEnabled(false);
    m_ui->computeClusters->setEnabled(false);
    m_ui->exportPlot->setEnabled(false);
    m_selected_spots.clear();
    // make the call
    QFuture<unsigned> future = QtConcurrent::run(this, &AnalysisClustering::computeClustersAsync);
    m_watcher_classes.setFuture(future);
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

mat AnalysisClustering::filterMatrix()
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
        m_scran_size_factors = RInterface::computeScranFactors(data.counts, true);
    }

    // Normalize and log matrix of counts
    mat A = STData::normalizeCounts(data,
                                    m_deseq_size_factors,
                                    m_scran_size_factors,
                                    normalization).counts;
    if (m_ui->logScale->isChecked()) {
        A = log(A + 1.0);
    }

    return A;
}

unsigned AnalysisClustering::computeClustersAsync()
{
    const mat &A = filterMatrix();
    return RInterface::computeSpotClasses(A);
}

void AnalysisClustering::computeColorsAsync()
{

    QWidget *tsne_tab = m_ui->tab->findChild<QWidget *>("tab_tsne");
    QWidget *pca_tab = m_ui->tab->findChild<QWidget *>("tab_pca");

    const int no_dims = 2;
    const int perplexity = tsne_tab->findChild<QSpinBox *>("perplexity")->value();
    const double theta = tsne_tab->findChild<QDoubleSpinBox *>("theta")->value();
    const int max_iter = tsne_tab->findChild<QSpinBox *>("max_iter")->value();
    const int init_dim = tsne_tab->findChild<QSpinBox *>("init_dims")->value();
    const bool kmeans = m_ui->kmeans->isChecked();
    const int num_clusters = m_ui->clusters->value();
    const bool scale = pca_tab->findChild<QCheckBox *>("scale")->isChecked();
    const bool center = pca_tab->findChild<QCheckBox *>("center")->isChecked();
    const bool tsne = m_ui->tab->currentIndex() == 0;

    const mat &A = filterMatrix();
    if (A.n_rows * 2 < perplexity) {
        QMessageBox::warning(this, tr("Spot classification"),
                             tr("Your perplexity is much higher than the number of spots"));
    }

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
    // enable the estimate button
    m_ui->computeClusters->setEnabled(true);

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
    m_series_vector.clear();
    for (int k = 0; k < num_clusters; ++k) {
        QScatterSeries *series = new QScatterSeries(this);
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(10.0);
        series->setColor(Color::color_list.at(k));
        series->setUseOpenGL(false);
        m_series_vector.push_back(series);
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
    emit signalClusteringUpdated();
}

void AnalysisClustering::classesComputed()
{
    // stop progress bar
    m_ui->progressBar->setMaximum(10);
    // enable run button
    m_ui->runClustering->setEnabled(true);
    // enable the estimate button
    m_ui->computeClusters->setEnabled(true);

    const unsigned n_clusters = m_watcher_classes.result();
    if (n_clusters == 0) {
        QMessageBox::critical(this,
                              tr("Spot classification"),
                              tr("There was an error estimating the number of clusters"));
    } else {
        m_ui->clusters->setValue(n_clusters);
    }
}

void AnalysisClustering::slotLassoSelection(const QPainterPath path)
{
    QList<QPointF> selected_points;
    for (const auto series : m_series_vector) {
        for (const QPointF point : series->points()) {
            const QPointF scene_point = m_ui->plot->chart()->mapToPosition(point, series);
            const QPoint view_point = m_ui->plot->mapFromScene(scene_point);
            if (path.contains(view_point)) {
                selected_points.append(point);
            }
        }
    }

    m_selected_spots.clear();
    for (unsigned i = 0; i < m_colors.size(); ++i) {
        const float x = m_reduced_coordinates.at(i,0);
        const float y = m_reduced_coordinates.at(i,1);
        if (selected_points.contains(QPointF(x,y))) {
            m_selected_spots.append(m_spots.at(i));
        }
    }

    if (!m_selected_spots.empty()) {
        emit signalClusteringSpotsSelected();
    }
}

#include "AnalysisClustering.h"

#include <QChartView>
#include <QValueAxis>
#include <QFuture>
#include <QtConcurrent>
#include <QMultiHash>
#include <QHash>

#include "color/HeatMap.h"

#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisClustering)
{
    // setup UI
    m_ui->setupUi(this);
    clear();

    connect(m_ui->runClustering, &QPushButton::clicked,
            this, &AnalysisClustering::slotRun);
    connect(m_ui->exportPlot, &QPushButton::clicked,
            this, &AnalysisClustering::slotExportPlot);
    connect(m_ui->createSelections, &QPushButton::clicked,
            this, &AnalysisClustering::signalClusteringExportSelections);
    connect(&m_watcher_clusters, &QFutureWatcher<void>::finished,
            this, &AnalysisClustering::clustersComputed);
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
    m_ui->max_iter->setValue(500);
    m_ui->init_dims->setValue(50);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    m_ui->runClustering->setEnabled(true);
    m_ui->createSelections->setEnabled(false);
    m_ui->tab->setCurrentIndex(0);
    m_ui->reads_threshold->setValue(0);
    m_ui->genes_threshold->setValue(5);
    m_ui->spots_threshold->setValue(5);
    m_ui->clusters->setValue(5);
    m_ui->logScale->setChecked(false);
    m_ui->plot->chart()->removeAllSeries();
    m_selected_spots.clear();
    m_clusters.clear();
}

QMultiHash<unsigned, QString> AnalysisClustering::getClustersSpot() const
{
    QMultiHash<unsigned, QString> computed_colors;
    #pragma omp parallel for
    for (const auto &item : m_clusters) {
        computed_colors.insert(item.second, item.first);
    }
    return computed_colors;
}

QHash<QString, QColor> AnalysisClustering::getSpotClusters() const
{
    QHash<QString, QColor> computed_colors;
    const int min = 0;
    const int max = m_ui->clusters->value();
    #pragma omp parallel for
    for (const auto &item : m_clusters) {
        const QColor color = Color::createCMapColor(item.second + 1,
                                                    min,
                                                    max,
                                                    QCPColorGradient::gpSpectrum);
        computed_colors.insert(item.first, color);
    }
    return computed_colors;
}

const QList<QString>& AnalysisClustering::selectedSpots() const
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
    m_ui->exportPlot->setEnabled(false);
    m_ui->createSelections->setEnabled(false);
    // clear the selected spots
    m_selected_spots.clear();
    // make the call
    QFuture<void> future = QtConcurrent::run(this, &AnalysisClustering::computeClustersAsync);
    m_watcher_clusters.setFuture(future);
}

void AnalysisClustering::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("Clustering plot"));
}

void AnalysisClustering::computeClustersAsync()
{
    QWidget *tsne_tab = m_ui->tab->findChild<QWidget *>("tab_tsne");
    QWidget *pca_tab = m_ui->tab->findChild<QWidget *>("tab_pca");

    const int NO_DIMS = 2;
    const int perplexity = tsne_tab->findChild<QSpinBox *>("perplexity")->value();
    const double theta = tsne_tab->findChild<QDoubleSpinBox *>("theta")->value();
    const int max_iter = tsne_tab->findChild<QSpinBox *>("max_iter")->value();
    const int init_dim = tsne_tab->findChild<QSpinBox *>("init_dims")->value();
    const int num_clusters = m_ui->clusters->value();
    const bool scale = pca_tab->findChild<QCheckBox *>("scale")->isChecked();
    const bool center = pca_tab->findChild<QCheckBox *>("center")->isChecked();
    const bool tsne = m_ui->tab->currentIndex() == 0;

    // Filter data
    STData::STDataFrame data = STData::filterCounts(m_data,
                                                    m_ui->reads_threshold->value(),
                                                    m_ui->genes_threshold->value(),
                                                    m_ui->spots_threshold->value());
    // Normalize and log matrix of counts
    SettingsWidget::NormalizationMode normalization = SettingsWidget::RAW;
    if (m_ui->normalization_rel->isChecked()) {
        normalization = SettingsWidget::REL;
    } else if (m_ui->normalization_cpm->isChecked()) {
        normalization = SettingsWidget::CPM;
    }
    mat A = STData::normalizeCounts(data, normalization).counts;
    if (m_ui->logScale->isChecked()) {
        A = log1p(A);
    }

    mat results;
    // Run dimensionality reduction
    if (tsne) {
        results = STMath::tSNE(A, theta, perplexity, max_iter, NO_DIMS, init_dim, -1, true);
    } else {
        results = STMath::PCA(A, NO_DIMS, center, scale, true);
    }

    // Run clustering
    mat results_clustering = STMath::kmeans_clustering(results, num_clusters, true);
    m_clusters.clear();
    m_reduced_coordinates.clear();
    #pragma omp parallel for collapse(2)
    for (uword i = 0; i < results.n_rows; ++i) {
        double min_dist = std::numeric_limits<double>::max();
        unsigned min_index = 0;
        const double x1 = results.at(i,0);
        const double y1 = results.at(i,1);
        for (uword j = 0; j < results_clustering.n_cols; ++j) {
            const double x2 = results_clustering.at(0,j);
            const double y2 = results_clustering.at(1,j);
            const double dist = STMath::euclidean(x1, y1, x2, y2);
            if (dist < min_dist) {
                min_dist = dist;
                min_index = j;
            }
        }
        m_clusters.push_back(QPair<QString, unsigned>(data.spots.at(i), min_index));
        m_reduced_coordinates.append(QPointF(x1,y1));
    }
}

void AnalysisClustering::clustersComputed()
{
    // stop progress bar
    m_ui->progressBar->setMaximum(10);
    // enable run button
    m_ui->runClustering->setEnabled(true);
    // enable the save clusters buttton
    m_ui->createSelections->setEnabled(true);

    // Quick saniry check
    if (m_clusters.empty() || m_reduced_coordinates.empty()) {
        QMessageBox::critical(this,
                              tr("Spot classification"),
                              tr("There was an error performing the clustering\n."
                                 "Perhaps too high perplexity?"));
        return;
    }

    const int min = 0;
    const int num_clusters = m_ui->clusters->value();

    // Create one serie for each different cluster (color)
    m_series_vector.clear();
    for (int k = 0; k < num_clusters; ++k) {
        QScatterSeries *series = new QScatterSeries(this);
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(10.0);
        const QColor color = Color::createCMapColor(k + 1,
                                                    min,
                                                    num_clusters,
                                                    QCPColorGradient::gpSpectrum);
        series->setColor(color);
        series->setUseOpenGL(false);
        m_series_vector.push_back(series);
    }

    // add the respective spot (t-SNE coordinates) to the serie it belongs to
    #pragma omp parallel for
    for (unsigned i = 0; i < m_clusters.size(); ++i) {
        const unsigned k = m_clusters.at(i).second;
        m_series_vector[k]->append(m_reduced_coordinates.at(i));
    }

    // update the scatter plot
    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    for (auto series : m_series_vector) {
        m_ui->plot->chart()->addSeries(series);
    }

    double xMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::min();
    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::min();
    for (const auto &p : m_reduced_coordinates) {
        xMin = qMin(xMin, p.x());
        xMax = qMax(xMax, p.x());
        yMin = qMin(yMin, p.y());
        yMax = qMax(yMax, p.y());
    }
    m_ui->plot->chart()->setTitle("Spots colored by cluster");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->show();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setGridLineVisible(false);
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setLabelsVisible(true);
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setRange(xMin - 1, xMax + 1);
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setTitleText(tr("DIM 1"));
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setGridLineVisible(false);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setLabelsVisible(true);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setRange(yMin - 1, yMax + 1);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setTitleText(tr("DIM 2"));

    // enable export controls
    m_ui->exportPlot->setEnabled(true);

    // notify the main view
    emit signalClusteringUpdated();
}

void AnalysisClustering::slotLassoSelection(const QPainterPath &path)
{
    m_selected_spots.clear();
    #pragma omp parallel for collapse(2)
    for (const auto series : m_series_vector) {
        for (const QPointF &point : series->points()) {
            const QPointF scene_point = m_ui->plot->chart()->mapToPosition(point, series);
            const QPoint view_point = m_ui->plot->mapFromScene(scene_point);
            if (path.contains(view_point)) {
                const int index = m_reduced_coordinates.indexOf(point);
                if (index != -1) {
                    m_selected_spots.push_back(m_clusters.at(index).first);
                }
            }
        }
    }

    if (!m_selected_spots.empty()) {
        emit signalClusteringSpotsSelected();
    }
}

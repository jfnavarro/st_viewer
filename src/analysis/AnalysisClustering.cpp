#include "AnalysisClustering.h"

#include <QChartView>
#include <QValueAxis>
#include <QFuture>
#include <QtConcurrent>
#include <QMultiHash>
#include <QHash>

#include "color/HeatMap.h"

#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent)
    : QWidget(parent, Qt::Window)
    , m_ui(new Ui::analysisClustering)
{
    // setup UI
    m_ui->setupUi(this);

    // resent to default
    clear();

    connect(m_ui->runClustering, &QPushButton::clicked,
            this, &AnalysisClustering::slotRun);
    connect(m_ui->exportPlot, &QPushButton::clicked,
            this, &AnalysisClustering::slotExportPlot);
    connect(m_ui->createSelections, &QPushButton::clicked,
            this, &AnalysisClustering::signalExportSelections);
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
    m_ui->max_iter->setValue(100);
    m_ui->genes_keep->setValue(5000);
    m_ui->init_dims->setValue(50);
    m_ui->progressBar->setTextVisible(true);
    m_ui->exportPlot->setEnabled(false);
    m_ui->runClustering->setEnabled(true);
    m_ui->createSelections->setEnabled(false);
    m_ui->tab->setCurrentIndex(0);
    m_ui->reads_threshold->setValue(1);
    m_ui->genes_threshold->setValue(10);
    m_ui->spots_threshold->setValue(10);
    m_ui->clusters->setValue(5);
    m_ui->logScale->setChecked(false);
    m_ui->plot->chart()->removeAllSeries();
    m_selected_spots.clear();
    m_clusters.clear();
}

QMultiHash<int, QString> AnalysisClustering::getClustersHash() const
{
    QMultiHash<int, QString> computed_colors;
    for (const auto &item : m_clusters) {
        computed_colors.insert(item.second, item.first);
    }
    return computed_colors;
}

const QVector<QPair<QString,int>> &AnalysisClustering::getClusters() const
{
    return m_clusters;
}

const QVector<QString>& AnalysisClustering::selectedSpots() const
{
    return m_selected_spots;
}

void AnalysisClustering::loadData(const STData::STDataFrame &data)
{
    m_data = data;
}

void AnalysisClustering::slotRun()
{
    qDebug() << "Initializing the computation of spot clusters";

    // initialize progress bar
    m_ui->progressBar->setRange(0,0);

    // disable controls
    m_ui->runClustering->setEnabled(false);
    m_ui->exportPlot->setEnabled(false);
    m_ui->createSelections->setEnabled(false);

    // clear the selected spots
    m_selected_spots.clear();

    // make the call on another thread
    QFuture<void> future = QtConcurrent::run(this, &AnalysisClustering::computeClustersAsync);
    m_watcher_clusters.setFuture(future);
}

void AnalysisClustering::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("Spots clustering"));
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
    const int num_genes_keep = m_ui->genes_keep->value();
    const bool scale = pca_tab->findChild<QCheckBox *>("scale")->isChecked();
    const bool center = pca_tab->findChild<QCheckBox *>("center")->isChecked();
    const bool tsne = m_ui->tab->currentIndex() == 0;

    // filter data
    STData::STDataFrame data = STData::filterCounts(m_data,
                                                    m_ui->reads_threshold->value(),
                                                    m_ui->genes_threshold->value(),
                                                    m_ui->spots_threshold->value());

    // quick sanity check
    if (data.counts.n_rows < 10 || data.counts.n_cols < 10) {
        QMessageBox::critical(this,
                              tr("Spots clustering"),
                              tr("The number of spots or genes is too little"),
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }


    // normalize and log matrix of counts
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

    // keep top variance genes
    if (num_genes_keep < data.counts.n_cols) {
        const auto var_genes = var(data.counts, 1);
        const auto idx = conv_to<uvec>::from(sort_index(var_genes, "descend"));
        const auto idx2 = conv_to<ucolvec>::from(idx.head(num_genes_keep));
        data.counts = data.counts.cols(idx2);
        qDebug() << "Keeping " << data.counts.n_cols << " genes";
    }

    // run dimensionality reduction
    qDebug() << "Performing dimensionality reduction";
    //TODO add a try-catch here
    const mat results = tsne ? STMath::tSNE(A, theta, perplexity, max_iter, NO_DIMS, init_dim, -1, false) :
                               STMath::PCA(A, NO_DIMS, center, scale, false);

    // run clustering
    qDebug() << "Performing k-means clustering";
    //TODO add a try-catch here
    const mat results_clustering = STMath::kmeans_clustering(results, num_clusters, false);
    Q_ASSERT((results.n_rows == A.n_rows) &&
             (results.n_cols == results_clustering.n_rows) &&
             (results_clustering.n_cols == num_clusters));

    // compile results by obtaining the closest spots to each centroid
    const int n_ele = results.n_rows;
    m_clusters.resize(n_ele);
    m_reduced_coordinates.resize(n_ele);
    #pragma omp parallel for
    for (uword i = 0; i < n_ele; ++i) {
        double min_dist = std::numeric_limits<double>::max();
        int min_index = -1;
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
        // store the spots for each centroid/cluster (i)
        if (min_index != -1) {
            m_clusters[i] = QPair<QString, int>(data.spots.at(i), min_index + 1);
            m_reduced_coordinates[i] = QPointF(x1,y1);
        }
    }
}

void AnalysisClustering::clustersComputed()
{
    qDebug() << "Spots clustering completed";

    // stop progress bar
    m_ui->progressBar->setMaximum(10);

    // enable run button
    m_ui->runClustering->setEnabled(true);

    // enable the save clusters buttton
    m_ui->createSelections->setEnabled(true);

    // quick sanity check
    if (m_clusters.empty() || m_reduced_coordinates.empty()) {
        QMessageBox::critical(this,
                              tr("Spots clustering"),
                              tr("There was an error performing the unsupervied clustering"),
                              QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }

    const int min = 1;
    const int num_clusters = m_ui->clusters->value();

    // create one QScatterSeries for each different cluster (one color per cluster too)
    m_series_vector.clear();
    for (int k = 1; k <= num_clusters; ++k) {
        QScatterSeries *series = new QScatterSeries();
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(5.0);
        const QColor color = Color::createCMapColor(k,
                                                    min,
                                                    num_clusters,
                                                    QCPColorGradient::gpJet);
        series->setColor(color);
        series->setUseOpenGL(false);
        m_series_vector.push_back(series);
    }

    // add the respective spot (manifold 2D coordinates) to the series it belongs
    // also obtain the min-max values of each axes
    double xMin = std::numeric_limits<double>::max();
    double xMax = std::numeric_limits<double>::min();
    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::min();
    for (int i = 0; i < m_clusters.size(); ++i) {
        const int k = m_clusters.at(i).second;
        m_series_vector[k-1]->append(m_reduced_coordinates.at(i));
        const auto p = m_reduced_coordinates.at(i);
        xMin = qMin(xMin, p.x());
        xMax = qMax(xMax, p.x());
        yMin = qMin(yMin, p.y());
        yMax = qMax(yMax, p.y());
    }

    // Update the scatter plot
    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    for (auto series : m_series_vector) {
        m_ui->plot->chart()->addSeries(series);
    }
    m_ui->plot->chart()->setTitle(tr("Spots colored by cluster"));
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

    // notify that clustering has been completed
    emit signalUpdated();
}

void AnalysisClustering::slotLassoSelection(const QPainterPath &path)
{
    // obtain the spots (in dim. reduced space) that intersects with the selection area
    m_selected_spots.clear();
    for (const auto &series : m_series_vector) {
        const auto &points = series->points();
        #pragma omp parallel
        {
            QVector<QString> selected_spots_private;
            for (int j = 0; j < points.size(); ++j) {
                const QPointF &point = points.at(j);
                const QPointF scene_point = m_ui->plot->chart()->mapToPosition(point, series);
                const QPoint view_point = m_ui->plot->mapFromScene(scene_point);
                if (path.contains(view_point)) {
                    const int index = m_reduced_coordinates.indexOf(point);
                    if (index != -1) {
                        selected_spots_private.append(m_clusters.at(index).first);
                    }
                }
            }

            #pragma omp critical
            m_selected_spots.append(selected_spots_private);
        }
    }
    if (!m_selected_spots.empty()) {
        // send a signal if the list is not empty
        emit signalSpotsSelected();
    }
}

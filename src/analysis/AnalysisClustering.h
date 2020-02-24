#ifndef ANALYSISCLUSTERING_H
#define ANALYSISCLUSTERING_H

#include <QDialog>
#include <QFutureWatcher>
#include <QScatterSeries>

#include "data/STData.h"

namespace Ui {
class analysisClustering;
}

QT_CHARTS_USE_NAMESPACE

// A Widget that is used to classify spots based on gene expression profiles
// using dimensionality reduction and clustering algorithms
// It allows users to interact with the scatter plot and make and export selections
// based on the clustered spots in the manifold space
class AnalysisClustering : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisClustering(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
    virtual ~AnalysisClustering();

    // One color (color representation of cluster number) for each spot
    const QVector<QPair<QString,int>> &getSpotClusters() const;

    // List of spots for each cluster
    QMultiHash<int, QString> getClustersSpot() const;

    // assigns the dataset
    void loadData(const STData::STDataFrame &data);

    // resets to default
    void clear();

    // the user selected spots if any
    const QVector<QString> &selectedSpots() const;

public slots:

signals:

    void signalClusteringUpdated();
    void signalClusteringSpotsSelected();
    void signalClusteringExportSelections();

private slots:

    // Performs a dimensionality reduction (t-SNE or PCA) on the data matrix and then
    // clusters the reduced coordinates (2D) using KMeans so to compute classes/colors
    // for each spot
    void slotRun();

    // exports the scatter plot to a file
    void slotExportPlot();

    // when the user makes a lasso selection on the scatter plot
    void slotLassoSelection(const QPainterPath &path);

private:

    // helper function to do the heavy computations on a different thread
    void computeClustersAsync();
    void clustersComputed();

    // the data
    STData::STDataFrame m_data;

    // the results
    QVector<QPair<QString,int>> m_clusters;
    QVector<QPointF> m_reduced_coordinates;

    // the computational thread
    QFutureWatcher<void> m_watcher_clusters;

    // the user selected spots
    QVector<QString> m_selected_spots;

    // the splot serie's
    QVector<QScatterSeries *> m_series_vector;

    // The UI object
    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H

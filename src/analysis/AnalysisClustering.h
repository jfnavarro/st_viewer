#ifndef ANALYSISCLUSTERING_H
#define ANALYSISCLUSTERING_H

#include <QDialog>
#include <QFutureWatcher>
#include <QScatterSeries>

#include "data/STData.h"

namespace Ui {
class analysisClustering;
}

//QT_CHARTS_USE_NAMESPACE

// A Widget that is used to cluster spots in a dataset based on their gene expression profiles
// using dimensionality reduction and clustering algorithms.
// The clustered spots are projected onto a 2D manifold and plotted in a scatter plot.
// It allows users to interact with the scatter plot in real time and to create and export selections
// based on the clustered spots in the manifold space.
class AnalysisClustering : public QWidget
{
    Q_OBJECT

public:

    explicit AnalysisClustering(QWidget *parent = nullptr);
    virtual ~AnalysisClustering();

    // hash table of spots for each cluster (integer value)
    QMultiHash<int, QString> getClustersHash() const;

    // list of spots with their cluster (pair(spot,cluster))
    const QVector<QPair<QString,int>> &getClusters() const;

    // assigns the dataset
    void loadData(const STData::STDataFrame &data);

    // resets to default
    void clear();

    // the user selected spots if any
    const QVector<QString> &selectedSpots() const;

public slots:

signals:

    // when the clustering has been performed
    void signalUpdated();

    // when the user has selected spots trough the scatter plot
    void signalSpotsSelected();

    // when the user wants to export clusters as selections
    void signalExportSelections();

private slots:

    // performs a dimensionality reduction (t-SNE or PCA) on the data matrix and then
    // clusters the reduced coordinates (2D) using k-means to assign a class/cluster
    // to each spot. UI elements are updated when finished. This is run on a different thread.
    void slotRun();

    // exports the scatter plot to a file
    void slotExportPlot();

    // when the user makes a lasso selection on the scatter plot the spots inside
    // the selection are added to a list and a signal is emitted
    void slotLassoSelection(const QPainterPath &path);

private:

    // helper function to do the heavy computations on a different thread
    void computeClustersAsync(QPromise<void> &promise);
    void clustersComputed();

    // the dataset
    STData::STDataFrame m_data;

    // the results
    QVector<QPair<QString,int>> m_clusters;
    QVector<QPointF> m_reduced_coordinates;

    // the computational thread
    QFutureWatcher<void> m_watcher_clusters;

    // the user selected spots
    QVector<QString> m_selected_spots;

    // the scatter plot serie's (UI elements)
    QVector<QScatterSeries *> m_series_vector;

    // The UI object
    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H

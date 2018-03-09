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
// It allows users to interact with the scatter plot and make selections
// based on the clustered spots
class AnalysisClustering : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisClustering(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
    virtual ~AnalysisClustering();

    // One color (color representation of cluster number) for each spot
    QHash<QString, QColor> getSpotClusters() const;

    // List of spots for each cluster
    QMultiHash<unsigned, QString> getClustersSpot() const;

    // assigns the dataset
    void loadData(const STData::STDataFrame &data);

    // resets to default
    void clear();

    // the user selected spots if any
    QList<QString> selectedSpots() const;

public slots:

signals:

    void signalClusteringUpdated();
    void signalClusteringSpotsSelected();
    void signalClusteringExportSelections();

private slots:

    // Performs a dimensionality reduction (t-SNE or PCA) on the data matrix and then
    // cluster the reduced coordinates (2D) using KMeans or HClust so to compute classes/colors
    // for each spot
    void slotRun();

    // exports the scatter plot to a file
    void slotExportPlot();

    // when the user makes a lasso selection on the scatter plot
    void slotLassoSelection(const QPainterPath &path);

    // when the user wants to estimate the number of clusters from the data
    void slotComputeClusters();

private:

    // helper function to do the heavy computations on a different thread
    void computeColorsAsync();
    unsigned computeClustersAsync();

    // function to udpate the widget once the computation of the spot classes is done
    void colorsComputed();

    // function to update the num clusters field once the estimation of the number of classes is done
    void classesComputed();

    // helper function to filter the matrix of counts
    mat filterMatrix();

    // the data
    STData::STDataFrame m_data;

    // the results
    std::vector<int> m_colors;
    mat m_reduced_coordinates;
    QList<QString> m_spots;

    // the computational threads
    QFutureWatcher<void> m_watcher_colors;
    QFutureWatcher<unsigned> m_watcher_classes;

    // the user selected spots
    QList<QString> m_selected_spots;

    // the splot serie's
    QList<QScatterSeries *> m_series_vector;

    // The UI object
    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H

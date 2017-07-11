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

// A Widget used to classify the spots based on gene expression profiles
// using dimensionality reduction and clustering
class AnalysisClustering : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisClustering(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisClustering();

    // One color for each spot
    QHash<Spot::SpotType, QColor> getComputedClasses() const;

    // assigns the dataset
    void loadData(const STData::STDataFrame &data);

    // reset to default
    void clear();

    // the user selected spots if any
    QList<Spot::SpotType> selectedSpots() const;

public slots:

signals:

    void signalClusteringUpdated();
    void signalClusteringSpotsSelected();

private slots:

    // Performs a dimensionality reduction (t-SNE or PCA) on the data matrix and then
    // cluster the reduced coordinates (2D) using KMeans or HClust so to compute classes/colors
    // for each spot
    void slotRun();

    // exports the plot to a file
    void slotExportPlot();

    // when the user makes a lasso selection
    void slotLassoSelection(const QPainterPath path);

private:

    void computeColorsAsync();
    void colorsComputed();

    // the data
    STData::STDataFrame m_data;

    // the size factors
    rowvec m_deseq_size_factors;
    rowvec m_scran_size_factors;

    // the results
    std::vector<int> m_colors;
    mat m_reduced_coordinates;
    QList<Spot::SpotType> m_spots;

    // the computational thread
    QFutureWatcher<void> m_watcher;

    // the user selected spots
    QList<Spot::SpotType> m_selected_spots;

    // the splot serie's
    QList<QScatterSeries *> m_series_vector;

    // The UI object
    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H

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

    // One color for each spot (the same order as in the table loaded)
    QVector<QColor> getComputedClasses() const;

    // assigns the dataset
    void loadData(const STData::STDataFrame &data);

public slots:

signals:

    void singalClusteringUpdated();

private slots:

    // Performs a dimensionality reduction (t-SNE or PCA) on the data matrix and then
    // cluster the reduced coordinates (2D) using KMeans or HClust so to compute classes/colors
    // for each spot
    void slotRun();

    // exports the plot to a file
    void slotExportPlot();

    // when the user clicks a point in the plot
    void slotClickedPoint(const QPointF point);

private:

    void computeColorsAsync();
    void colorsComputed();

    STData::STDataFrame m_data;

    // store the size factors to save computational time
    rowvec m_deseq_factors;
    rowvec m_scran_factors;

    // the results
    std::vector<int> m_colors;
    mat m_reduced_coordinates;

    // the computational thread
    QFutureWatcher<void> m_watcher;

    // store the plotting series to allow interaction with the plot
    QVector<QScatterSeries *> m_series_vector;

    // The UI object
    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H

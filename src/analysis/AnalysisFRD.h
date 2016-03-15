#ifndef ANALYSISHISTOGRAM_H
#define ANALYSISHISTOGRAM_H

#include <memory>

#include "data/DataProxy.h"
#include "qcustomplot/qcustomplot.h"

namespace Ui
{
class frdWidget;
}

// Analysis FDR is a Widget that is used to show to the users
// the reads distributions of the features of a dataset
// and also the threshold bars, threshold bars are
// interactive so the will get updated if the user
// modifies the threshold control

// TODO add option to compute the plots and threshold bars
// using genes as the function of features and genes threshold
// TODO perhaps separate computation and visualization
class AnalysisFRD : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisFRD(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisFRD();

    // Computes the FRD values from the features and min max values given as input
    void computeData(const DataProxy::FeatureList& features, const int min, const int max);

signals:

public slots:

    // Slots to adjust the boundaries when the thresholds are changed
    void setLowerLimit(const int limit);
    void setUpperLimit(const int limit);

private slots:

    // To control plot interaction
    void mousePress();
    void mouseWheel();

private:
    // Helper functions to create and configure the plotting objects
    void initializePlotNormal();
    void initializePlotLog();

    std::unique_ptr<Ui::frdWidget> m_ui;

    // Plotting object for normal reads
    QCustomPlot* m_customPlotNormal;
    QCPItemLine* m_upperThresholdBarNormal;
    QCPItemLine* m_lowerThresholdBarNormal;

    // Plotting object for normal reads in log space
    QCustomPlot* m_customPlotLog;
    QCPItemLine* m_upperThresholdBarLog;
    QCPItemLine* m_lowerThresholdBarLog;

    // To keep track of the min-max of the Y axes to adjust the threshold bars
    int m_minY;
    int m_maxY;

    Q_DISABLE_COPY(AnalysisFRD)
};
#endif // ANALYSISHISTOGRAM_H

#ifndef ANALYSISHISTOGRAM_H
#define ANALYSISHISTOGRAM_H

#include "data/DataProxy.h"
#include "qcustomplot/qcustomplot.h"
#include <memory>

namespace Ui
{
class frdWidget;
}

// Analysis FDR is a Widget that is used to show to the users
// the counts distributions of the features of a dataset
// (as histogram of the different number of counts sorted)
// and also the threshold bars, threshold bars are
// interactive so the will get updated if the user
// modifies the threshold control.

// TODO add option to compute the plots and threshold bars
// using genes as the function of features and show the genes thresholds bars as
// well
// TODO perhaps separate computation and visualization
class AnalysisFRD : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisFRD(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisFRD();

    // Computes the FRD values from the features and min max values given as input
    // The FDR x values will be the different number of counts
    // The FDR y values will be the accumulation of the same number of counts
    void computeData(const DataProxy::FeatureList &features,
                     const unsigned min,
                     const unsigned max);

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

    // GUI object
    QScopedPointer<Ui::frdWidget> m_ui;

    // Plotting objects for normal reads
    QScopedPointer<QCustomPlot> m_customPlotNormal;
    QScopedPointer<QCPItemLine> m_upperThresholdBarNormal;
    QScopedPointer<QCPItemLine> m_lowerThresholdBarNormal;

    // Plotting objects for normal reads in log space
    QScopedPointer<QCustomPlot> m_customPlotLog;
    QScopedPointer<QCPItemLine> m_upperThresholdBarLog;
    QScopedPointer<QCPItemLine> m_lowerThresholdBarLog;

    // To keep track of the min-max of the Y axes to adjust the threshold bars
    unsigned m_minY;
    unsigned m_maxY;

    Q_DISABLE_COPY(AnalysisFRD)
};
#endif // ANALYSISHISTOGRAM_H

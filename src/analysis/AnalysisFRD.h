/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ANALYSISHISTOGRAM_H
#define ANALYSISHISTOGRAM_H

#include <memory>

#include "data/DataProxy.h"
#include "qcustomplot/qcustomplot.h"

namespace Ui {
class frdWidget;
}

// analysis FDR is a Widget that is meant to show
// the reads distributions of the features of a dataset
// and also the threshold bars, threshold bars are
// interactive so the will get updated if the user
// modifies the threshold controls
class AnalysisFRD: public QDialog
{
    Q_OBJECT

public:

    explicit AnalysisFRD(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisFRD();

    // computes the FRD values from the features and min max values given as input
    void computeData(const DataProxy::FeatureList& features,
                     const int min,
                     const int max,
                     const int pooledMin,
                     const int pooledMax);

signals:

public slots:

    // slots to adjust the boundaries when the thresholds are changed
    void setLowerLimit(const int limit);
    void setUpperLimit(const int limit);
    void setPooledLowerLimit(const int limit);
    void setPooledUpperLimit(const int limit);

private slots:

    // to control plot interaction
    void mousePress();
    void mouseWheel();

private:

    // helper functions to create and configure the plotting objects
    void initializePlotNormal();
    void initializePlotLog();
    void initializePlotPooledNormal();
    void initializePlotPooledLog();

    std::unique_ptr<Ui::frdWidget> m_ui;

    //plotting object for normal reads
    QCustomPlot *m_customPlotNormal;
    QCPItemLine *m_upperThresholdBarNormal;
    QCPItemLine *m_lowerThresholdBarNormal;

    //plotting object for normal reads in log space
    QCustomPlot *m_customPlotLog;
    QCPItemLine *m_upperThresholdBarLog;
    QCPItemLine *m_lowerThresholdBarLog;

    //plotting object for pooled reads
    QCustomPlot *m_customPlotPooledNormal;
    QCPItemLine *m_upperThresholdBarPooledNormal;
    QCPItemLine *m_lowerThresholdBarPooledNormal;

    //plotting object for pooled reads in log space
    QCustomPlot *m_customPlotPooledLog;
    QCPItemLine *m_upperThresholdBarPooledLog;
    QCPItemLine *m_lowerThresholdBarPooledLog;

    //keep track of the min-max of the Y axes to adjust the threshold bars
    int m_minY;
    int m_maxY;
    int m_minYPooled;
    int m_maxYPooled;

};
#endif // ANALYSISHISTOGRAM_H

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
class readsHistogramWidget;
}

class AnalysisHistogram : public QDialog
{
    Q_OBJECT

public:

    explicit AnalysisHistogram(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisHistogram();

    // computes the histogram values from the features and min max values given as input
    void compute(const DataProxy::FeatureList& features, const int min, const int max);

signals:

public slots:

    // slots to adjust the boundaries when the threshold is changed
    void setLowerLimit(const int limit);
    void setUpperLimit(const int limit);

private slots:

    // to control plot interaction
    void mousePress();
    void mouseWheel();

    // saves the plot to a PDF file
    void saveToPDF();

private:

    std::unique_ptr<Ui::readsHistogramWidget> m_ui;
    QCustomPlot *m_customPlotNormal;
    QCPItemLine *m_upperThresholdBar;
    QCPItemLine *m_lowerThresholdBar;

    //keep track of the min,max values to adjust thresholds
    int m_minX;
    int m_minY;
    int m_maxX;
    int m_maxY;

};
#endif // ANALYSISHISTOGRAM_H

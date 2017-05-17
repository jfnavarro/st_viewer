#ifndef ANALYSISHISTOGRAM_H
#define ANALYSISHISTOGRAM_H

#include <QDialog>
#include <memory>

#include "qcustomplot.h"
#include "data/STData.h"

namespace Ui
{
class frdWidget;
}

// Analysis FRD is a Widget that is used to show to the users
// the counts/genes distributions of the features of a dataset (as histograms)

// TODO separate computation and visualization
// TODO add the QCustomplot objects to the UI object
// TODO use only one QCustomplot and switch log-normal in it
class AnalysisFRD : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisFRD(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisFRD();

    // Computes the histograms data from the dataset
    void computeData(const STData &dataset);

signals:

public slots:

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
    // QCustomPlot forces deleting of every added member,
    // therefore we cannot use scoped pointer here
    QPointer<QCustomPlot> m_customPlotNormal;

    // Plotting objects for normal reads in log space
    // QCustomPlot forces deleting of every added member,
    // therefore we cannot use scoped pointer here
    QPointer<QCustomPlot> m_customPlotLog;

    Q_DISABLE_COPY(AnalysisFRD)
};
#endif // ANALYSISHISTOGRAM_H

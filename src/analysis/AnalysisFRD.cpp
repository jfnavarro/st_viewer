/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AnalysisFRD.h"

#include "ui_frdWidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "dataModel/Feature.h"

#include <cmath>
#include "math/Common.h"

AnalysisFRD::AnalysisFRD(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui(new Ui::frdWidget),
    m_customPlotNormal(nullptr),
    m_upperThresholdBarNormal(nullptr),
    m_lowerThresholdBarNormal(nullptr),
    m_customPlotLog(nullptr),
    m_upperThresholdBarLog(nullptr),
    m_lowerThresholdBarLog(nullptr),
    m_customPlotPooledNormal(nullptr),
    m_upperThresholdBarPooledNormal(nullptr),
    m_lowerThresholdBarPooledNormal(nullptr),
    m_customPlotPooledLog(nullptr),
    m_upperThresholdBarPooledLog(nullptr),
    m_lowerThresholdBarPooledLog(nullptr),
    m_minY(0.0),
    m_maxY(1.0),
    m_minYPooled(0.0),
    m_maxYPooled(1.0)
{
    setModal(false);
    m_ui->setupUi(this);

    //TODO these init functions have common code
    initializePlotNormal();
    initializePlotLog();
    initializePlotPooledNormal();
    initializePlotPooledLog();
}

AnalysisFRD::~AnalysisFRD()
{
    m_customPlotNormal->deleteLater();
    m_customPlotNormal = nullptr;

    m_upperThresholdBarNormal->deleteLater();
    m_upperThresholdBarNormal = nullptr;

    m_lowerThresholdBarNormal->deleteLater();
    m_lowerThresholdBarNormal = nullptr;

    m_customPlotLog->deleteLater();
    m_customPlotLog = nullptr;

    m_upperThresholdBarLog->deleteLater();
    m_upperThresholdBarLog = nullptr;

    m_lowerThresholdBarLog->deleteLater();
    m_lowerThresholdBarLog = nullptr;

    m_customPlotPooledNormal->deleteLater();
    m_customPlotPooledNormal = nullptr;

    m_upperThresholdBarPooledNormal->deleteLater();
    m_upperThresholdBarPooledNormal = nullptr;

    m_lowerThresholdBarPooledNormal->deleteLater();
    m_lowerThresholdBarPooledNormal = nullptr;

    m_customPlotPooledLog->deleteLater();
    m_customPlotPooledLog = nullptr;

    m_upperThresholdBarPooledLog->deleteLater();
    m_upperThresholdBarPooledLog = nullptr;

    m_lowerThresholdBarPooledLog->deleteLater();
    m_lowerThresholdBarPooledLog = nullptr;
}

void AnalysisFRD::initializePlotNormal()
{
    // creating plotting object for normal reads ditribution
    m_customPlotNormal = new QCustomPlot(m_ui->plotNormalWidget);
    Q_ASSERT(m_customPlotNormal != nullptr);

    // add bars
    m_lowerThresholdBarNormal = new QCPItemLine(m_customPlotNormal);
    m_customPlotNormal->addItem(m_lowerThresholdBarNormal);
    m_lowerThresholdBarNormal->setHead(QCPLineEnding::esNone);
    m_lowerThresholdBarNormal->setPen(QPen(Qt::red));

    m_upperThresholdBarNormal = new QCPItemLine(m_customPlotNormal);
    m_customPlotNormal->addItem(m_upperThresholdBarNormal);
    m_upperThresholdBarNormal->setHead(QCPLineEnding::esNone);
    m_upperThresholdBarNormal->setPen(QPen(Qt::red));

    // add plot of data
    m_customPlotNormal->addGraph();
    m_customPlotNormal->graph(0)->setPen(QPen(Qt::blue));
    m_customPlotNormal->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    // configure right and top axis to show ticks but no labels:
    m_customPlotNormal->xAxis2->setVisible(true);
    m_customPlotNormal->xAxis2->setTickLabels(false);
    m_customPlotNormal->yAxis2->setVisible(true);
    m_customPlotNormal->yAxis2->setTickLabels(false);
    m_customPlotNormal->xAxis->setLabel("Reads counts");
    m_customPlotNormal->yAxis->setLabel("# Features");

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(m_customPlotNormal->xAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotNormal->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_customPlotNormal->yAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotNormal->yAxis2, SLOT(setRange(QCPRange)));

    // connect slots that takes care that when an axis is selected,
    // only that direction can be dragged and zoomed:
    connect(m_customPlotNormal, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(m_customPlotNormal, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    m_customPlotNormal->setFixedSize(550, 400);
    m_customPlotNormal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void AnalysisFRD::initializePlotLog()
{
    // creating plotting object for log reads ditribution
    m_customPlotLog = new QCustomPlot(m_ui->plotLogWidget);
    Q_ASSERT(m_customPlotLog != nullptr);

    // add bars
    m_lowerThresholdBarLog = new QCPItemLine(m_customPlotLog);
    m_customPlotLog->addItem(m_lowerThresholdBarLog);
    m_lowerThresholdBarLog->setHead(QCPLineEnding::esNone);
    m_lowerThresholdBarLog->setPen(QPen(Qt::red));

    m_upperThresholdBarLog = new QCPItemLine(m_customPlotLog);
    m_customPlotLog->addItem(m_upperThresholdBarLog);
    m_upperThresholdBarLog->setHead(QCPLineEnding::esNone);
    m_upperThresholdBarLog->setPen(QPen(Qt::red));

    // add plot of data
    m_customPlotLog->addGraph();
    m_customPlotLog->graph(0)->setPen(QPen(Qt::blue));
    m_customPlotLog->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    // configure right and top axis to show ticks but no labels:
    m_customPlotLog->xAxis2->setVisible(true);
    m_customPlotLog->xAxis2->setTickLabels(false);
    m_customPlotLog->yAxis2->setVisible(true);
    m_customPlotLog->yAxis2->setTickLabels(false);
    m_customPlotLog->xAxis->setLabel("Reads counts (log)");
    m_customPlotLog->yAxis->setLabel("# Features (log)");
    //TODO set log scale for axes

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(m_customPlotLog->xAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotLog->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_customPlotLog->yAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotLog->yAxis2, SLOT(setRange(QCPRange)));

    // connect slots that takes care that when an axis is selected,
    // only that direction can be dragged and zoomed:
    connect(m_customPlotLog, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(m_customPlotLog, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    m_customPlotLog->setFixedSize(550, 400);
    m_customPlotLog->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void AnalysisFRD::initializePlotPooledNormal()
{
    // creating plotting object for pooled normal reads ditribution
    m_customPlotPooledNormal = new QCustomPlot(m_ui->plotPooledNormalWidget);
    Q_ASSERT(m_customPlotPooledNormal != nullptr);

    // add bars
    m_lowerThresholdBarPooledNormal = new QCPItemLine(m_customPlotPooledNormal);
    m_customPlotPooledNormal->addItem(m_lowerThresholdBarPooledNormal);
    m_lowerThresholdBarPooledNormal->setHead(QCPLineEnding::esNone);
    m_lowerThresholdBarPooledNormal->setPen(QPen(Qt::red));

    m_upperThresholdBarPooledNormal = new QCPItemLine(m_customPlotPooledNormal);
    m_customPlotPooledNormal->addItem(m_upperThresholdBarPooledNormal);
    m_upperThresholdBarPooledNormal->setHead(QCPLineEnding::esNone);
    m_upperThresholdBarPooledNormal->setPen(QPen(Qt::red));

    // add plot of data
    m_customPlotPooledNormal->addGraph();
    m_customPlotPooledNormal->graph(0)->setPen(QPen(Qt::blue));
    m_customPlotPooledNormal->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    // configure right and top axis to show ticks but no labels:
    m_customPlotPooledNormal->xAxis2->setVisible(true);
    m_customPlotPooledNormal->xAxis2->setTickLabels(false);
    m_customPlotPooledNormal->yAxis2->setVisible(true);
    m_customPlotPooledNormal->yAxis2->setTickLabels(false);
    m_customPlotPooledNormal->xAxis->setLabel("Pooled Reads counts");
    m_customPlotPooledNormal->yAxis->setLabel("# Features");

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(m_customPlotPooledNormal->xAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotPooledNormal->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_customPlotPooledNormal->yAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotPooledNormal->yAxis2, SLOT(setRange(QCPRange)));

    // connect slots that takes care that when an axis is selected,
    // only that direction can be dragged and zoomed:
    connect(m_customPlotPooledNormal, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(m_customPlotPooledNormal, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    m_customPlotPooledNormal->setFixedSize(550, 400);
    m_customPlotPooledNormal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void AnalysisFRD::initializePlotPooledLog()
{
    // creating plotting object for normal reads ditribution
    m_customPlotPooledLog = new QCustomPlot(m_ui->plotPooledLogWidget);
    Q_ASSERT(m_customPlotPooledLog != nullptr);

    // add bars
    m_lowerThresholdBarPooledLog = new QCPItemLine(m_customPlotPooledLog);
    m_customPlotPooledLog->addItem(m_lowerThresholdBarPooledLog);
    m_lowerThresholdBarPooledLog->setHead(QCPLineEnding::esNone);
    m_lowerThresholdBarPooledLog->setPen(QPen(Qt::red));

    m_upperThresholdBarPooledLog = new QCPItemLine(m_customPlotPooledLog);
    m_customPlotPooledLog->addItem(m_upperThresholdBarPooledLog);
    m_upperThresholdBarPooledLog->setHead(QCPLineEnding::esNone);
    m_upperThresholdBarPooledLog->setPen(QPen(Qt::red));

    // add plot of data
    m_customPlotPooledLog->addGraph();
    m_customPlotPooledLog->graph(0)->setPen(QPen(Qt::blue));
    m_customPlotPooledLog->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    // configure right and top axis to show ticks but no labels:
    m_customPlotPooledLog->xAxis2->setVisible(true);
    m_customPlotPooledLog->xAxis2->setTickLabels(false);
    m_customPlotPooledLog->yAxis2->setVisible(true);
    m_customPlotPooledLog->yAxis2->setTickLabels(false);
    m_customPlotPooledLog->xAxis->setLabel("Pooled Reads counts (log)");
    m_customPlotPooledLog->yAxis->setLabel("# Features (log)");

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(m_customPlotPooledLog->xAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotPooledLog->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_customPlotPooledLog->yAxis, SIGNAL(rangeChanged(QCPRange)),
            m_customPlotPooledLog->yAxis2, SLOT(setRange(QCPRange)));

    // connect slots that takes care that when an axis is selected,
    // only that direction can be dragged and zoomed:
    connect(m_customPlotPooledLog, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(m_customPlotPooledLog, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    m_customPlotPooledLog->setFixedSize(550, 400);
    m_customPlotPooledLog->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

//TODO split and optimize this function
void AnalysisFRD::computeData(const DataProxy::FeatureList& features,
                              const int min, const int max,
                              const int pooledMin, const int pooledMax)
{

    QHash<double, double> featureCounter;
    QHash<QString, QPair<double,double> > featurePooledCounter;

    //iterate the features to compute hash tables to help to obtain the X and Y axes for the plots
    foreach(DataProxy::FeaturePtr feature, features) {
        featureCounter[feature->hits()]++;
        featurePooledCounter[feature->barcode()].first += feature->hits();
        featurePooledCounter[feature->barcode()].second++;
    };

    //x,y corresponds to normal reads
    QVector<double> x = QVector<double>::fromList(featureCounter.keys());
    QVector<double> y = QVector<double>::fromList(featureCounter.values());

    //x2,y2 corresponds to pooled reads
    QVector<QPair<double,double> > pooledPair =
            QVector<QPair<double,double> >::fromList(featurePooledCounter.values());
    QVector<double> x2;
    QVector<double> y2;
    for (const auto& pair : pooledPair){
        x2.push_back(pair.first);
        y2.push_back(pair.second);
    }

    //TODO probably no need to keep max values of Y as it can be obtained form
    //the plotting object
    m_maxY = *std::max_element(y.begin(), y.end());
    m_minY = *std::min_element(y.begin(), y.end());
    m_maxYPooled = *std::max_element(y2.begin(), y2.end());
    m_minYPooled = *std::min_element(y2.begin(), y2.end());

    // update bars
    setLowerLimit(min);
    setUpperLimit(max);
    setPooledLowerLimit(pooledMin);
    setPooledUpperLimit(pooledMax);

    // update plot data
    m_customPlotNormal->graph(0)->setData(x, y);
    m_customPlotNormal->graph(0)->rescaleAxes();
    m_customPlotNormal->replot();

    m_customPlotLog->graph(0)->setData(STMath::logVectorValues(x), STMath::logVectorValues(y));
    m_customPlotLog->graph(0)->rescaleAxes();
    m_customPlotLog->replot();

    m_customPlotPooledNormal->graph(0)->setData(x2, y2);
    m_customPlotPooledNormal->graph(0)->rescaleAxes();
    m_customPlotPooledNormal->replot();

    m_customPlotPooledLog->graph(0)->setData(STMath::logVectorValues(x2), STMath::logVectorValues(y2));
    m_customPlotPooledLog->graph(0)->rescaleAxes();
    m_customPlotPooledLog->replot();
}

void AnalysisFRD::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (m_customPlotNormal->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeDrag(m_customPlotNormal->xAxis->orientation());
    } else if (m_customPlotNormal->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeDrag(m_customPlotNormal->yAxis->orientation());
    } else {
        m_customPlotNormal->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    }
}

void AnalysisFRD::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (m_customPlotNormal->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeZoom(m_customPlotNormal->xAxis->orientation());
    } else if (m_customPlotNormal->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeZoom(m_customPlotNormal->yAxis->orientation());
    } else {
        m_customPlotNormal->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    }
}

void AnalysisFRD::setUpperLimit(int limit)
{
    m_upperThresholdBarNormal->start->setCoords(limit, m_minY);
    m_upperThresholdBarNormal->end->setCoords(limit, m_maxY);
    m_upperThresholdBarLog->start->setCoords(std::log(limit + 1), std::log(m_minY + 1));
    m_upperThresholdBarLog->end->setCoords(std::log(limit + 1), std::log(m_maxY + 1));
    m_customPlotNormal->replot();
    m_customPlotLog->replot();
}

void AnalysisFRD::setLowerLimit(int limit)
{
    m_lowerThresholdBarNormal->start->setCoords(limit, m_minY);
    m_lowerThresholdBarNormal->end->setCoords(limit, m_maxY);
    m_lowerThresholdBarLog->start->setCoords(std::log(limit + 1), std::log(m_minY + 1));
    m_lowerThresholdBarLog->end->setCoords(std::log(limit + 1), std::log(m_maxY + 1));
    m_customPlotNormal->replot();
    m_customPlotLog->replot();
}

void AnalysisFRD::setPooledUpperLimit(int limit)
{
    m_upperThresholdBarPooledNormal->start->setCoords(limit, m_minYPooled);
    m_upperThresholdBarPooledNormal->end->setCoords(limit, m_maxYPooled);
    m_upperThresholdBarPooledLog->start->setCoords(std::log(limit + 1),
                                                   std::log(m_minYPooled + 1));
    m_upperThresholdBarPooledLog->end->setCoords(std::log(limit + 1),
                                                 std::log(m_maxYPooled + 1));
    m_customPlotPooledNormal->replot();
    m_customPlotPooledLog->replot();
}

void AnalysisFRD::setPooledLowerLimit(int limit)
{
    m_lowerThresholdBarPooledNormal->start->setCoords(limit, m_minYPooled);
    m_lowerThresholdBarPooledNormal->end->setCoords(limit, m_maxYPooled);
    m_lowerThresholdBarPooledLog->start->setCoords(std::log(limit + 1),
                                                   std::log(m_minYPooled + 1));
    m_lowerThresholdBarPooledLog->end->setCoords(std::log(limit + 1),
                                                 std::log(m_maxYPooled + 1));
    m_customPlotPooledNormal->replot();
    m_customPlotPooledLog->replot();
}

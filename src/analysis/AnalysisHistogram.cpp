/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AnalysisHistogram.h"

#include "ui_readsHistogramWidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "dataModel/Feature.h"

#include <cmath>
#include "math/Common.h"

AnalysisHistogram::AnalysisHistogram(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui(new Ui::readsHistogramWidget),
    m_customPlotNormal(nullptr),
    m_minX(0),
    m_minY(0),
    m_maxX(0),
    m_maxY(0)
{
    setModal(false);

    m_ui->setupUi(this);

    // creating plotting objects
    m_customPlotNormal = new QCustomPlot(m_ui->plotNormalWidget);
    Q_ASSERT(m_customPlotNormal != nullptr);

    //make connections
    connect(m_ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_ui->saveButton, SIGNAL(clicked()), this, SLOT(saveToPDF()));
}

AnalysisHistogram::~AnalysisHistogram()
{
    m_customPlotNormal->deleteLater();
    m_customPlotNormal = nullptr;
}

//TODO split and optimize this function
void AnalysisHistogram::compute(const DataProxy::FeatureList& features,
                                const int min, const int max)
{

    QHash<double, double> featureCounter;
    foreach(DataProxy::FeaturePtr feature, features) {
        featureCounter[feature->hits()]++;
    };

    QVector<double> x = QVector<double>::fromList(featureCounter.keys());
    QVector<double> y = QVector<double>::fromList(featureCounter.values());

    //TODO probably no need to keep maxY
    m_maxX = max;
    m_maxY = *std::max_element(y.begin(), y.end());
    m_minX = min;
    m_minY = *std::min_element(y.begin(), y.end());

    m_customPlotNormal->clearGraphs();

    // add bars
    m_lowerThresholdBar = new QCPItemLine(m_customPlotNormal);
    m_customPlotNormal->addItem(m_lowerThresholdBar);
    m_lowerThresholdBar->start->setCoords(m_minX, m_minY);
    m_lowerThresholdBar->end->setCoords(m_minX, m_maxY);
    m_lowerThresholdBar->setHead(QCPLineEnding::esNone);
    m_lowerThresholdBar->setPen(QPen(Qt::red));

    m_upperThresholdBar = new QCPItemLine(m_customPlotNormal);
    m_customPlotNormal->addItem(m_upperThresholdBar);
    m_upperThresholdBar->start->setCoords(m_maxX, m_minY);
    m_upperThresholdBar->end->setCoords(m_maxX, m_maxY);
    m_upperThresholdBar->setHead(QCPLineEnding::esNone);
    m_lowerThresholdBar->setPen(QPen(Qt::red));

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

    m_customPlotNormal->graph(0)->setData(x, y);
    m_customPlotNormal->graph(0)->rescaleAxes();

    m_customPlotNormal->setFixedSize(1200, 800);
    m_customPlotNormal->replot();
    m_customPlotNormal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_customPlotNormal->show();
}

void AnalysisHistogram::setUpperLimit(int limit)
{
    //limit will be in a range 0...100, we adjust the threshold
    //to the scale min - max of the data
    const int adjusted_limit = STMath::linearConversion<int,int>(limit,
                                                        Globals::GENE_THRESHOLD_MIN,
                                                        Globals::GENE_THRESHOLD_MAX,
                                                        m_minX,
                                                        m_maxX);
    //adjust the threshold bar
    m_upperThresholdBar->start->setCoords(adjusted_limit, m_minY);
    m_upperThresholdBar->end->setCoords(adjusted_limit, m_maxY);
    m_customPlotNormal->replot();
    m_customPlotNormal->show();
}

void AnalysisHistogram::setLowerLimit(int limit)
{
    //limit will be in a range 0...100, we adjust the threshold
    //to the scale min - max of the data
    const int adjusted_limit = STMath::linearConversion<int,int>(limit,
                                                        Globals::GENE_THRESHOLD_MIN,
                                                        Globals::GENE_THRESHOLD_MAX,
                                                        m_minX,
                                                        m_maxX);

    //adjust the threshold bar
    m_lowerThresholdBar->start->setCoords(adjusted_limit, m_minY);
    m_lowerThresholdBar->end->setCoords(adjusted_limit, m_maxY);
    m_customPlotNormal->replot();
    m_customPlotNormal->show();
}

void AnalysisHistogram::saveToPDF()
{
    QString filename =
            QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(),
                                         QString("%1").arg(tr("PNG Files (*.png)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    //TODO add most FRD genes and stats (use QPrinter)
    //TODO use PDF as output
    const bool saveOk = m_customPlotNormal->savePng(filename, 800, 800, 1.0, 100);

    if (!saveOk) {
        QMessageBox::critical(this, tr("Save FRD"), tr("Error saving FRD to a file"));
    } else {
        QMessageBox::information(this, tr("Save FRD"), tr("FRD was saved successfully"));
    }
}

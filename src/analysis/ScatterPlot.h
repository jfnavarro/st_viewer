/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include <QWidget>

#include "qcustomplot/qcustomplot.h"

namespace Ui {
class plotCanvas;
}

class QTableWidget;

class ScatterPlot : public QWidget
{
    Q_OBJECT

public:

    explicit ScatterPlot(int size, QWidget *parent = 0);
    ~ScatterPlot();

    void setHeaderText(const QString& text);

    QTableWidget* getTable() const;

    template<typename T>
    void plot(const QVector<T> &listX, const QVector<T> &listY,
              const QString& labelX = "X axes",
              const QString& labelY = "Y axes")
    {
        Q_ASSERT(m_customPlot != nullptr);

        m_customPlot->clearGraphs();
        m_customPlot->addGraph();
        m_customPlot->graph(0)->setScatterStyle(
                    QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::blue), Qt::white, 5));
        m_customPlot->graph(0)->setData(listX, listY);
        m_customPlot->graph(0)->setAntialiasedScatters(true);
        m_customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
        m_customPlot->graph(0)->rescaleAxes(true);
        // sets the legend
        m_customPlot->legend->setVisible(true);
        // give the axes some labels:
        m_customPlot->xAxis->setLabel(labelX);
        m_customPlot->yAxis->setLabel(labelY);
        // set axes ranges, so we see all data:
        m_customPlot->xAxis->setRange(0, 10e4);
        m_customPlot->yAxis->setRange(0, 10e4);
        m_customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
        m_customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        m_customPlot->xAxis->setTicks(true);
        m_customPlot->yAxis->setTicks(true);
        // plot and add mouse interaction
        m_customPlot->setFixedSize(800, 400);
        m_customPlot->replot();
        m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        m_customPlot->show();
        show();
    }

signals:

protected slots:

private:

    Ui::plotCanvas *m_ui;
    QCustomPlot *m_customPlot;

};

#endif // SCATTERPLOT_H

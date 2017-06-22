#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChartView>
#include <QChart>
#include <QRubberBand>

QT_CHARTS_USE_NAMESPACE

// A simple wrapper around QChartView to allow zooming and mouse events
class ChartView : public QChartView
{
    Q_OBJECT

public:

    explicit ChartView(QWidget *parent = 0);
    virtual ~ChartView();

protected:

    bool viewportEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:

    bool m_panning;
    QPointF m_originPanning;
};

#endif // CHARTVIEW_H

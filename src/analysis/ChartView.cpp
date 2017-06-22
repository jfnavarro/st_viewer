#include "ChartView.h"

#include <QMouseEvent>

ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
    , m_panning(false)
{
    setChart(new QChart);
}

ChartView::~ChartView()
{

}

bool ChartView::viewportEvent(QEvent *event)
{
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    m_panning = true;
    m_originPanning = event->pos();
    setCursor(Qt::ClosedHandCursor);
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        const QPoint point = event->pos();
        const QPointF pan_adjustment = QPointF(point - m_originPanning);
        chart()->scroll(-pan_adjustment.x(), pan_adjustment.y());
        m_originPanning = point;
    }
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_panning) {
        unsetCursor();
        m_panning = false;
    }
    QChartView::mouseReleaseEvent(event);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    const int angle = event->angleDelta().y();
    const qreal factor = (angle > 0) ? 1.1 : 0.9;
    chart()->zoom(factor);
    QChartView::wheelEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

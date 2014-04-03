#include "ScrollArea.h"
#include <qglobal.h>
#include <QScrollBar>
#include <QPointF>
#include <QResizeEvent>
#include "viewOpenGL/CellGLView.h"

ScrollArea::ScrollArea( QWidget * parent_ )
    : QAbstractScrollArea( parent_)
{
    m_view = new CellGLView();
    m_container = QWidget::createWindowContainer(m_view);
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setViewport(m_container);
    verticalScrollBar()->setTracking(true);
    horizontalScrollBar()->setTracking(true);  
    connect(m_view, SIGNAL(signalViewPortUpdated(const QRectF)), this, SLOT(setCellGLViewViewPort(const QRectF)));
    connect(m_view, SIGNAL(signalSceneUpdated(const QRectF)), this, SLOT(setCellGLViewScene(const QRectF)));
    connect(m_view, SIGNAL(signalSceneTransformationsUpdated(const QTransform)), this, SLOT(setCellGLViewSceneTransformations(const QTransform)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(someScrollChangedValue(int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(someScrollChangedValue(int)));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    viewport()->update();
}

ScrollArea::~ScrollArea() {}

CellGLView *ScrollArea::cellGlView() const {
    return m_view;
}

void ScrollArea::someScrollChangedValue(int) {
    QPoint focusPoint = m_view->sceneFocusCenterPoint().toPoint();
    QPoint point(horizontalScrollBar()->value(), verticalScrollBar()->value()); 
    if (focusPoint == point) {
        return;
    }
    m_view->setSceneFocusCenterPointWithClamping(point);
}

void ScrollArea::setupViewport(QWidget *viewport) {
    viewport->resize(size());
}

void ScrollArea::adjustScrollBars() {
    if (m_cellglview_scene.isEmpty() || m_cellglview_viewPort.isEmpty()) {
        return;
    }
    if (m_cellglview_scene.isValid() &&
        m_cellglview_viewPort.isValid() &&
        ! m_cellglview_sceneTransformations.isIdentity()) {
        const QRectF viewPortInSceneCoordinates = m_cellglview_sceneTransformations.inverted().mapRect(m_cellglview_viewPort);
        QRect allowedRect = m_view->allowedCenterPoints().toRect();
        QPoint focusPoint = m_view->sceneFocusCenterPoint().toPoint();
        verticalScrollBar()->setMaximum(allowedRect.top() + allowedRect.height() + 1);
        verticalScrollBar()->setMinimum(allowedRect.top());
        verticalScrollBar()->setValue(focusPoint.y());
        horizontalScrollBar()->setMaximum(allowedRect.left() + allowedRect.width() +1);
        horizontalScrollBar()->setMinimum(allowedRect.left());
        horizontalScrollBar()->setValue(focusPoint.x());
        verticalScrollBar()->setPageStep(viewPortInSceneCoordinates.height());
        horizontalScrollBar()->setPageStep(viewPortInSceneCoordinates.width());
        horizontalScrollBar()->setSingleStep(300);
        verticalScrollBar()->setSingleStep(300);
    }
}

void ScrollArea::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);
    m_container->update();
}

void ScrollArea::resizeEvent(QResizeEvent *e)
{
    viewport()->resize(e->size());
    viewport()->update();
}

void ScrollArea::setCellGLViewScene(const QRectF scene)
{
    m_cellglview_scene = scene;
}

void ScrollArea::setCellGLViewViewPort(const QRectF view)
{
    m_cellglview_viewPort = view;
}

void ScrollArea::setCellGLViewSceneTransformations(const QTransform transform)
{ 
    m_cellglview_sceneTransformations = transform;
    adjustScrollBars();
}

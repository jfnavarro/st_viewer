/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ScrollArea.h"

#include <qglobal.h>
#include <QScrollBar>
#include <QPointF>
#include <QResizeEvent>

#include "viewOpenGL/CellGLView.h"

ScrollArea::ScrollArea(QWidget *parent)
    : QAbstractScrollArea(parent),
      m_view(nullptr)
{

}

ScrollArea::~ScrollArea()
{

}

void ScrollArea::initializeView(QPointer<CellGLView> view)
{
    Q_ASSERT(!view.isNull());

    // set the viewport and keep a reference to it
    m_view = view;
    setViewport(m_view);

    verticalScrollBar()->setTracking(true);
    horizontalScrollBar()->setTracking(true);

    connect(m_view.data(), SIGNAL(signalViewPortUpdated(const QRectF)),
            this, SLOT(setCellGLViewViewPort(const QRectF)));
    connect(m_view.data(), SIGNAL(signalSceneUpdated(const QRectF)),
            this, SLOT(setCellGLViewScene(const QRectF)));
    connect(m_view.data(), SIGNAL(signalSceneTransformationsUpdated(const QTransform)),
            this, SLOT(setCellGLViewSceneTransformations(const QTransform)));

    connect(verticalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(someScrollBarChangedValue(int)));
    connect(horizontalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(someScrollBarChangedValue(int)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    viewport()->update();
}

void ScrollArea::someScrollBarChangedValue(int)
{
    const QRectF rectF = m_view->allowedCenterPoints();
    const qreal h_value = static_cast<qreal>(horizontalScrollBar()->sliderPosition());
    const qreal v_value = static_cast<qreal>(verticalScrollBar()->sliderPosition());
    const qreal x = rectF.width() * ( 1 - (h_value / m_scrollBarSteps) ) + rectF.x();
    const qreal y = rectF.height() * ( 1 - (v_value / m_scrollBarSteps) ) + rectF.y();
    const QPointF point(x,y);
    m_view->setSceneFocusCenterPointWithClamping(point);
}

void ScrollArea::setupViewport(QWidget *viewport)
{
    viewport->resize(size());
}

void ScrollArea::adjustScrollBar(const int scrollBarSteps,
                                 const qreal value,
                                 const qreal value_minimum,
                                 const qreal value_range,
                                 const qreal viewPortInSceneCoordinatesRange,
                                 QScrollBar *scrollBar)
{
    scrollBar->setMinimum(0);
    scrollBar->setMaximum(scrollBarSteps);
    scrollBar->setValue(scrollBarSteps * ( 1 - ( (value-value_minimum) / value_range) ) );
    // When we are maximally zoomed out the value_range will be zero for at least one of the
    // scrollbars. For the case that we need to divide by zero, we set the PageStep value to
    // be as big as possible. Unfortunately the value std::numeric_limits<int>::max() is not
    // big enough because sometimes the scroll gets to be a pixel to short.
    const int val = qFuzzyCompare(value_range,0.0) ? std::numeric_limits<int>::max() :
                                                     static_cast<int>(scrollBarSteps
                                                                      * viewPortInSceneCoordinatesRange
                                                                      / value_range);
    scrollBar->setPageStep(val);
    //TODO 300 magic number?
    scrollBar->setSingleStep(300);
}

void ScrollArea::adjustScrollBars()
{
    if (m_cellglview_scene.isEmpty() || m_cellglview_viewPort.isEmpty()
            || horizontalScrollBar()->isSliderDown() || verticalScrollBar()->isSliderDown()) {
        return;
    }

    if (m_cellglview_scene.isValid() &&
            m_cellglview_viewPort.isValid() &&
            ! m_cellglview_sceneTransformations.isIdentity()) {

        const QRectF viewPortInSceneCoordinates =
                m_cellglview_sceneTransformations.mapRect(m_cellglview_viewPort);

        const QRectF allowedRectF = m_view->allowedCenterPoints();
        const QPointF focusPointF = m_view->sceneFocusCenterPoint();

        adjustScrollBar(m_scrollBarSteps, focusPointF.x(), allowedRectF.x(), allowedRectF.width(),
                        viewPortInSceneCoordinates.width(), horizontalScrollBar());

        adjustScrollBar(m_scrollBarSteps, focusPointF.y(), allowedRectF.y(), allowedRectF.height(),
                        viewPortInSceneCoordinates.height(), verticalScrollBar());
    }
}

void ScrollArea::resizeEvent(QResizeEvent *e)
{
    m_view->resizeEvent(e);
}

void ScrollArea::paintEvent(QPaintEvent *e)
{
    m_view->paintEvent(e);
}

void ScrollArea::mousePressEvent(QMouseEvent *e)
{
    m_view->mousePressEvent(e);
}

void ScrollArea::mouseReleaseEvent(QMouseEvent *e)
{
    m_view->mouseReleaseEvent(e);
}

void ScrollArea::mouseMoveEvent(QMouseEvent *e)
{
    m_view->mouseMoveEvent(e);
}

void ScrollArea::wheelEvent(QWheelEvent *e)
{
    m_view->wheelEvent(e);
}

void ScrollArea::keyPressEvent(QKeyEvent *e)
{
    m_view->keyPressEvent(e);
}

void ScrollArea::setCellGLViewScene(const QRectF &scene)
{
    m_cellglview_scene = scene;
}

void ScrollArea::setCellGLViewViewPort(const QRectF &view)
{
    m_cellglview_viewPort = view;
}

void ScrollArea::setCellGLViewSceneTransformations(const QTransform &transform)
{
    if (m_cellglview_sceneTransformations != transform.inverted()) {
        m_cellglview_sceneTransformations = transform.inverted();
        adjustScrollBars();
    }
}

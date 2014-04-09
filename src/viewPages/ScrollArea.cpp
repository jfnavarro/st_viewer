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

    if (m_sliderMoveImplementation) {
        verticalScrollBar()->setTracking(false);
        horizontalScrollBar()->setTracking(false);
     } else {
         verticalScrollBar()->setTracking(true);
         horizontalScrollBar()->setTracking(true);
    }
    //verticalScrollBar()->setTracking(true);
    //horizontalScrollBar()->setTracking(true);

    connect(m_view, SIGNAL(signalViewPortUpdated(const QRectF)), 
            this, SLOT(setCellGLViewViewPort(const QRectF)));
    connect(m_view, SIGNAL(signalSceneUpdated(const QRectF)),
            this, SLOT(setCellGLViewScene(const QRectF)));
    connect(m_view, SIGNAL(signalSceneTransformationsUpdated(const QTransform)),
            this, SLOT(setCellGLViewSceneTransformations(const QTransform)));

   if (m_sliderMoveImplementation) {
         connect(verticalScrollBar(), SIGNAL(sliderMoved(int)),
                 this, SLOT(someScrollBarChangedValue(int)));
         connect(horizontalScrollBar(), SIGNAL(sliderMoved(int)),
                 this, SLOT(someScrollBarChangedValue(int)));
    } else {
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(someScrollBarChangedValue(int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(someScrollBarChangedValue(int)));
   }
    /*
    connect(verticalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(someScrollBarChangedValue(int)));
    connect(horizontalScrollBar(), SIGNAL(sliderMoved(int)),
            this, SLOT(someScrollBarChangedValue(int)));
    */

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    viewport()->update();
}

ScrollArea::~ScrollArea() {}

CellGLView *ScrollArea::cellGlView() const {
    return m_view;
}

void ScrollArea::someScrollBarChangedValue(int) {
    const QRectF rectF = m_view->allowedCenterPoints();
    qreal h_value = 0.0;
    qreal v_value = 0.0;
    if (m_sliderMoveImplementation) {
      h_value = static_cast<qreal>(horizontalScrollBar()->sliderPosition());
      v_value = static_cast<qreal>(verticalScrollBar()->sliderPosition());
    } else {
       h_value = static_cast<qreal>(horizontalScrollBar()->value());
       v_value = static_cast<qreal>(verticalScrollBar()->value());
    }
    const qreal x = rectF.width() * ( 1 - (h_value / m_scrollBarSteps) ) + rectF.x();
    const qreal y = rectF.height() * ( 1 - (v_value / m_scrollBarSteps) ) + rectF.y();
    const QPointF point(x,y);
    m_view->setSceneFocusCenterPointWithClamping(point);
}

void ScrollArea::setupViewport(QWidget *viewport) {
    viewport->resize(size());
}

void ScrollArea::adjustScrollBar(const int scrollBarSteps,
                                             const qreal value,
                                             const qreal value_minimum,
                                             const qreal value_range,
                                             const qreal viewPortInSceneCoordinatesRange,
                                             QScrollBar *scrollBar) {
    scrollBar->setMinimum(0);
    scrollBar->setMaximum(scrollBarSteps);
    scrollBar->setValue(scrollBarSteps * ( 1 - ( (value-value_minimum) / value_range) ) );
    // When we are maximally zoomed out the value_range will be zero for at least one of the
    // scrollbars. For the case that we need to divide by zero, we set the PageStep value to 
    // be as big as possible. Unfortunately the value std::numeric_limits<int>::max() is not 
    // big enough because sometimes the scroll gets to be a pixel to short.
    int val = (qFuzzyCompare(value_range,0.0)) ? std::numeric_limits<int>::max() :
                                     static_cast<int>(scrollBarSteps * viewPortInSceneCoordinatesRange / value_range);
    scrollBar->setPageStep(val);
    scrollBar->setSingleStep(300);
}

void ScrollArea::adjustScrollBars() {
    if (m_cellglview_scene.isEmpty() || m_cellglview_viewPort.isEmpty()) {
        return;
    }
    if (horizontalScrollBar()->isSliderDown() || verticalScrollBar()->isSliderDown()) {
        return;
    }
    if (m_cellglview_scene.isValid() &&
        m_cellglview_viewPort.isValid() &&
        ! m_cellglview_sceneTransformations.isIdentity()) {

        const QRectF viewPortInSceneCoordinates =
          m_cellglview_sceneTransformations.inverted().mapRect(m_cellglview_viewPort);

        const QRectF allowedRectF = m_view->allowedCenterPoints();
        const QPointF focusPointF = m_view->sceneFocusCenterPoint();

        adjustScrollBar(m_scrollBarSteps, focusPointF.x(), allowedRectF.x(), allowedRectF.width(),
                              viewPortInSceneCoordinates.width(), horizontalScrollBar());

        adjustScrollBar(m_scrollBarSteps, focusPointF.y(), allowedRectF.y(), allowedRectF.height(),
                               viewPortInSceneCoordinates.height(), verticalScrollBar());
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

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GraphicsViewGL.h"

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QApplication>
#include <QGLWidget>
#include <GLQt.h>
#include <math/GLMatrix.h>

#include "view/openGL/WidgetGL.h"
#include "view/openGL/MiniMapGL.h"
#include "view/openGL/HeatMapLegendGL.h"

#include <qmath.h>
#include "utils/MathExtended.h"

#include "SelectionEvent.h"
#include "GraphicsSceneGL.h"



GraphicsViewGL::GraphicsViewGL(QWidget* parent)
    : QGraphicsView(parent), m_minimap(0), m_opengl_surface(0),
      m_zoom(Globals::DEFAULT_ZOOM), m_zoom_min(Globals::DEFAULT_ZOOM_MIN),
      m_zoom_max(Globals::DEFAULT_ZOOM_MAX),
      m_mousePressViewPoint(), m_rubberBanding(false), m_rubberBandRect(),
      m_viewItems()
{
    setBackground(Qt::black);
    //NOTE see UI file for configuration variables
}

GraphicsViewGL::~GraphicsViewGL()
{
    finalizeGL();
}

void GraphicsViewGL::initGL(QGraphicsScene *scene)
{
    DEBUG_FUNC_NAME
    
    // reset state
    m_zoom = Globals::DEFAULT_ZOOM;
    m_zoom_min = Globals::DEFAULT_ZOOM_MIN;
    m_zoom_max = Globals::DEFAULT_ZOOM_MAX;
    
    QGLFormat format;
    format.setDirectRendering(true);
    format.setDoubleBuffer(true);
    format.setAlpha(true);
    format.setDepth(false);
    format.setRgba(true);
    format.setProfile(QGLFormat::CompatibilityProfile);
    
    // setup opengl viewport
    if (m_opengl_surface == 0) {
        m_opengl_surface = new WidgetGL(format);
        m_opengl_surface->makeCurrent();
        setViewport(m_opengl_surface); //NOTE relinquish ownership
        setScene(scene);
    }
    //setup minimap
    if (m_minimap == 0) {
        m_minimap = new MiniMapGL(this);
        m_minimap->setTransform(QTransform::fromTranslate(-10.0, -10.0));
        m_minimap->setAnchor(ViewItemGL::SouthEast);
        m_viewItems.push_back(m_minimap);
        connect(m_minimap, SIGNAL(signalCenterOn(QPointF)), this, SLOT(slotCenterOn(QPointF)));
    }
}

void GraphicsViewGL::finalizeGL()
{
    DEBUG_FUNC_NAME
    
    foreach(ViewItemGL * item, m_viewItems) {
        if (item != 0) {
            delete item;
        }
        item = 0;
    }
    
    m_viewItems.clear();
    m_minimap = 0;
    m_opengl_surface = 0;
}

const QImage GraphicsViewGL::grabPixmapGL()
{
    QImage image;
    QWidget *widget = viewport();
    if (QGLWidget *viewport = qobject_cast<QGLWidget*>(widget)) {
        //NOTE make sure we are grabbing from the front buffer
        //     OBS: assumes a nonstereo setting
        glReadBuffer(GL_FRONT_LEFT);
        image = viewport->grabFrameBuffer();
    }
    return image;
}

void GraphicsViewGL::addViewItem(ViewItemGL *viewItem)
{
    viewItem->setParent(this);
    m_viewItems.append(viewItem);
}

void GraphicsViewGL::mousePressEvent(QMouseEvent* event)
{
    QPointF point = event->localPos();
    foreach(ViewItemGL * item, m_viewItems) {
        const QPointF localPoint = (item->transform() * anchorTransform(item->anchor())).inverted().map(point);
        QMouseEvent newEvent(
            event->type(),
            localPoint,//event->localPos(),
            event->windowPos(),
            event->screenPos(),
            event->button(),
            event->buttons(),
            event->modifiers()
        );
        if (item->contains(localPoint) && item->mousePressEvent(&newEvent)) {
            // view item has handled the mouse event
            event->ignore();
            return;
        }
    }
    // rubber band selection
    pressRubberBand(event);
    QGraphicsView::mousePressEvent(event);
}

void GraphicsViewGL::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = event->localPos();
    foreach(ViewItemGL * item, m_viewItems) {
        const QPointF localPoint = (item->transform() * anchorTransform(item->anchor())).inverted().map(point);
        QMouseEvent newEvent(
            event->type(),
            localPoint,//event->localPos(),
            event->windowPos(),
            event->screenPos(),
            event->button(),
            event->buttons(),
            event->modifiers()
        );
        if (item->contains(localPoint) && item->mouseMoveEvent(&newEvent)) {
            // view item has handled the mouse event
            event->ignore();
            return;
        }
    }
    // rubber band selection
    moveRubberBand(event);
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsViewGL::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF point = event->localPos();
    foreach(ViewItemGL * item, m_viewItems) {
        const QPointF localPoint = (item->transform() * anchorTransform(item->anchor())).inverted().map(point);
        QMouseEvent newEvent(
            event->type(),
            localPoint,//event->localPos(),
            event->windowPos(),
            event->screenPos(),
            event->button(),
            event->buttons(),
            event->modifiers()
        );
        if (item->contains(localPoint) && item->mouseReleaseEvent(&newEvent)) {
            // view item has handled the mouse event
            event->ignore();
            return;
        }
    }
    // rubber band selection
    releaseRubberBand(event);
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsViewGL::wheelEvent(QWheelEvent* event)
{
    qreal zoomFactor = qPow(4.0 / 3.0, (-event->delta() / 240.0));
    setZoom(zoomFactor * zoom());
    event->ignore();
}

void GraphicsViewGL::showEvent(QShowEvent* event)
{
    // update zoom
    setTransformZoom(m_zoom);
    QGraphicsView::showEvent(event);
}

void GraphicsViewGL::resizeEvent(QResizeEvent* event)
{
    //NOTE do not change zoom transform here! will cause event loop!
    QGraphicsView::resizeEvent(event);
}

void GraphicsViewGL::drawBackground(QPainter *painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    if (painter->paintEngine()->type() != QPaintEngine::OpenGL &&
        painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qDebug() << "GraphicsViewGL: drawBackground needs a QGLWidget to be"
                 "set as viewport on the graphics view";
        return;
    }

    const QColor color = backgroundBrush().color();
    GL::GLcolor bkgColor = GL::toGLcolor(color);
    painter->beginNativePainting();
    {
        glClearColor(bkgColor.red, bkgColor.green, bkgColor.blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    painter->endNativePainting();
}

void GraphicsViewGL::drawForeground(QPainter* painter, const QRectF& rect)
{

    if (painter->paintEngine()->type() != QPaintEngine::OpenGL &&
        painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qDebug() << "GraphicsViewGL: drawForeground needs a QGLWidget to be"
                 "set as viewport on the graphics view";
        return;
    }
    // update minimap
    const QRectF& scene = sceneRect();
    m_minimap->setScene(scene);
    m_minimap->setView(rect);
    // render all view items
    foreach(ViewItemGL * item, m_viewItems) {
        if (item->visible()) {
            QTransform transform = item->transform() * anchorTransform(item->anchor());
            painter->setTransform(transform);
            item->render(painter);
        }
    }
    painter->beginNativePainting();
    {
        glFlush();
    }
    painter->endNativePainting();
}

void GraphicsViewGL::enableMinimap(bool enable)
{
    m_minimap->setVisible(enable);
    update();
}

const QSizeF GraphicsViewGL::zoom() const
{
    return m_zoom;
}

void GraphicsViewGL::setZoom(const QSizeF& zoom)
{
    const QSizeF boundedZoom = QtExt::clamp(zoom, m_zoom_min, m_zoom_max, Qt::KeepAspectRatio);
    if (m_zoom != boundedZoom) {
        m_zoom = boundedZoom;
        setTransformZoom(m_zoom);
    }
}

void GraphicsViewGL::zoomIn(qreal zoomFactor)
{
    if (zoomFactor < 1.0) {
        qDebug() << QString("[GraphicsViewGL] Warning: Applying decreasing zoom (%1) with zoom in function!").arg(zoomFactor);
    }
    setZoom(zoomFactor * zoom());
}
void GraphicsViewGL::zoomOut(qreal zoomFactor)
{
    if (zoomFactor > 1.0) {
        qDebug() << QString("[GraphicsViewGL] Warning: Applying increasing zoom (%1) with zoom out function!").arg(zoomFactor);
    }

    setZoom(zoomFactor * zoom());
}

void GraphicsViewGL::setBackground(const QColor& color)
{
    setBackgroundBrush(QBrush(color, Qt::SolidPattern));
    update();
}

void GraphicsViewGL::updateSceneRect(const QRectF& rect)
{
    setTransformZoom(m_zoom);
    QGraphicsView::updateSceneRect(rect);
}

void GraphicsViewGL::slotCenterOn(const QPointF& point)
{
    centerOn(point);
}

void GraphicsViewGL::pressRubberBand(QMouseEvent* event)
{
    if (isInteractive()) {
        // Store some event data
        m_mousePressViewPoint = event->pos();
    }
    if (dragMode() == QGraphicsView::RubberBandDrag && !m_rubberBanding) {
        if (isInteractive()) {
            // Rubberbanding is only allowed in interactive mode.
            m_rubberBanding = true;
            m_rubberBandRect = QRect();
        }
    }
}
void GraphicsViewGL::moveRubberBand(QMouseEvent* event)
{
    Q_UNUSED(event);

}

void GraphicsViewGL::releaseRubberBand(QMouseEvent* event)
{
    DEBUG_FUNC_NAME
    if (dragMode() == QGraphicsView::RubberBandDrag && isInteractive() && !event->buttons()) {
        if (m_rubberBanding) {
            
            // get the scene
            GraphicsSceneGL* s = dynamic_cast<GraphicsSceneGL*>(scene());
            
            // Check for enough drag distance
            if ((m_mousePressViewPoint - event->pos()).manhattanLength() < QApplication::startDragDistance()) {
                 
                // Clear selection if not for enough drag distance
                if (s)
                {
                    SelectionEvent::SelectionMode mode = SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
                    SelectionEvent selectionEvent(QPainterPath(), mode);
                    s->setSelectionArea(&selectionEvent);
                }
                QGraphicsView::mouseMoveEvent(event);
                return;
            }
            
            // Update old rubberband
            if (!m_rubberBandRect.isEmpty()) {
                viewport()->update();
            }
            
            // Update rubberband position
            const QPoint &mp = m_mousePressViewPoint;
            QPoint ep = event->pos();
            m_rubberBandRect = QRect(qMin(mp.x(), ep.x()), qMin(mp.y(), ep.y()),
                                     qAbs(mp.x() - ep.x()) + 1, qAbs(mp.y() - ep.y()) + 1);

            // Update new rubberband
            if (!m_rubberBandRect.isEmpty()) {
                viewport()->update();
            }
            
            // Set the new selection area
            QPainterPath selectionArea;
            selectionArea.addPolygon(mapToScene(m_rubberBandRect));
            selectionArea.closeSubpath();
            
            if (s) {
                SelectionEvent::SelectionMode mode = SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
                SelectionEvent selectionEvent(selectionArea, mode);
                s->setSelectionArea(&selectionEvent);
            }

            // update viewport
            if (viewportUpdateMode() != QGraphicsView::NoViewportUpdate) {
                viewport()->update();
            }
            
            // reset variables
            m_rubberBanding = false;
            m_rubberBandRect = QRect();
        }
    }
}

const QTransform GraphicsViewGL::anchorTransform(ViewItemGL::Anchor anchor) const
{
    //TODO cache values and update on resize
    const QSizeF viewSize = viewport()->geometry().size();

    QTransform transform(Qt::Uninitialized);
    switch (anchor) {
    case ViewItemGL::Center:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5f, viewSize.height() * 0.5f);
        break;
    case ViewItemGL::North:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5f, 0.0f);
        break;
    case ViewItemGL::NorthEast:
        transform = QTransform::fromTranslate(viewSize.width(), 0.0f);
        break;
    case ViewItemGL::East:
        transform = QTransform::fromTranslate(viewSize.width(), viewSize.height() * 0.5f);
        break;
    case ViewItemGL::SouthEast:
        transform = QTransform::fromTranslate(viewSize.width(), viewSize.height());
        break;
    case ViewItemGL::South:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5f, viewSize.height());
        break;
    case ViewItemGL::SouthWest:
        transform = QTransform::fromTranslate(0.0f, viewSize.height());
        break;
    case ViewItemGL::West:
        transform = QTransform::fromTranslate(0.0f, viewSize.height() * 0.5f);
        break;
    case ViewItemGL::NorthWest:
    // fall-through
    default:
        transform = QTransform::fromTranslate(0.0f, 0.0f);
        break;
    }
    return transform;
}

void GraphicsViewGL::setTransformZoom(const QSizeF& zoom)
{
    // derive current aspect ratio
    const QSizeF view  = viewport()->size();
    const QSizeF scene = sceneRect().size();
    qreal z = qMax(view.width() / scene.width(), view.height() / scene.height());
    const QSizeF current = QSizeF(transform().m11(), transform().m22());
    scale(z * zoom.width() / current.width(), z * zoom.height() / current.height());
}

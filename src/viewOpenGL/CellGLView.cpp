/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CellGLView.h"

#include "math/Common.h"
#include "utils/Utils.h"

#include <QGLPainter>
#include <QArray>
#include <QWheelEvent>
#include <QtOpenGL>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QGuiApplication>
#include <QVector2DArray>
#include <QRubberBand>

static const qreal DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT = 10.0;
static const qreal MAX_ZOOM_DIVIDE_FACTOR = 100.0;
//static const qreal DELTA_MOUSE_PANNING = 1.0f;
static const int KEY_PRESSES_TO_MOVE_A_POINT_OVER_THE_SCREEN = 10;

CellGLView::CellGLView(QScreen *parent) :
    QWindow(parent)
{
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setDepthBufferSize(0);
    format.setAlphaBufferSize(24);
    format.setBlueBufferSize(24);
    format.setGreenBufferSize(24);
    format.setRedBufferSize(24);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setStereo(false);
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    create();
}

void CellGLView::reset()
{

}

QRectF CellGLView::scene() const
{
    return m_scene;
}

QRectF CellGLView::viewPort() const
{
    return m_viewport;
}


CellGLView::~CellGLView()
{
    //NOTE View does not own rendering nodes nor texture
    // draw rendering nodes
    foreach(GraphicItemGL *node, m_nodes) {
        delete node;
        node = 0;
    }
    if ( m_context ) {
        delete m_context;
    }
    m_context = 0;
}

void CellGLView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

void CellGLView::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
}

void CellGLView::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);
    ensureContext();
    if ( !m_initialized ) {
        initializeGL();
    }
    //paint
    paintGL();
    m_context->swapBuffers(this); // this is important
}

void CellGLView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    const QRect rect = geometry();
    ensureContext();
    if ( !m_initialized ) {
        initializeGL();
    }
    resizeGL(rect.width(), rect.height());
}

void CellGLView::ensureContext()
{
    if ( !m_context ) {
        m_context = new QOpenGLContext();
        m_context->setFormat(format);
        const bool success = m_context->create();
        qDebug() << "CellGLView, OpenGL context create = " << success;
    }
    m_context->makeCurrent(this);
}

void CellGLView::initializeGL()
{
    QGLPainter painter;
    painter.begin();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_initialized = true;
}

void CellGLView::paintGL()
{
    QGLTexture2D::processPendingResourceDeallocations();

    QGLPainter painter;
    painter.begin();

    painter.setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QRectF viewport(m_viewport);
    QMatrix4x4 projm;
    projm.ortho(viewport);
    painter.projectionMatrix() = projm;

    // send signal to minimap with new scene
    //emit signalSceneUpdated(sceneTransformations().mapRect(m_scene));

    // draw rendering nodes
    foreach(GraphicItemGL *node, m_nodes) {
        if ( node->visible() ) {
            QTransform local_transform = nodeTransformations(node);
            if ( node->transformable() ) {
                local_transform *= sceneTransformations();
            }
            painter.modelViewMatrix().push();
            painter.modelViewMatrix() *= local_transform;
            node->draw(&painter);
            painter.modelViewMatrix().pop();
        }
    }
    glFlush(); // forces to send the data to the GPU saving time (no need for this when only 1 context)
}

void CellGLView::resizeGL(int width, int height)
{
    //devicePixelRatio() fixes the problem with MAC retina
    const qreal pixelRatio = devicePixelRatio();
    glViewport(0.0, 0.0, width * pixelRatio, height * pixelRatio);
    setViewPort(QRectF(0.0, 0.0, width * pixelRatio, height * pixelRatio));
    if (m_scene.isValid()) {
        m_zoom_factor = clampZoomFactorToAllowedRange(m_zoom_factor);
        setSceneFocusCenterPointWithClamping(m_scene_focus_center_point);
    }
}

void CellGLView::wheelEvent(QWheelEvent* event)
{
    qreal zoomFactor = qPow(4.0 / 3.0, (event->delta() / 240.0));
    setZoomFactorAndUpdate(zoomFactor *m_zoom_factor);
    event->ignore();
}

void CellGLView::addRenderingNode(GraphicItemGL *node)
{
    Q_ASSERT(node != nullptr);
    m_nodes.append(node);
    connect(node, SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::removeRenderingNode(GraphicItemGL *node)
{
    Q_ASSERT(node != nullptr);
    m_nodes.removeOne(node);
    disconnect(node, SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::update()
{
    QGuiApplication::postEvent(this, new QExposeEvent(geometry()));
}

qreal CellGLView::clampZoomFactorToAllowedRange(qreal zoom) const
{
    Q_ASSERT(minZoom() < maxZoom());
    return qMin(qMax(minZoom(), zoom), maxZoom());
}

void CellGLView::setZoomFactorAndUpdate(qreal zoom)
{
    const qreal new_zoom_factor = clampZoomFactorToAllowedRange(zoom);
    if (m_zoom_factor != new_zoom_factor) {
        m_zoom_factor = new_zoom_factor;
        setSceneFocusCenterPointWithClamping(m_scene_focus_center_point);
        update();
    }
}

void CellGLView::centerOn(const QPointF& point)
{
    setSceneFocusCenterPointWithClamping(point);
}

void CellGLView::rotate(qreal angle)
{
    if (angle >= -180.0 && angle <= 180.0 && m_rotate != angle ) {
        m_rotate += angle;
        STMath::clamp(m_rotate, -360.0, 360.0);
        update();
    }
}
  
void CellGLView::setViewPort(QRectF viewport)
{
    if ( m_viewport != viewport && viewport.isValid() ) {
        m_viewport = viewport;
        //emit signalViewPortUpdated(m_viewport);
    }
}

void CellGLView::setScene(QRectF scene)
{
    if ( m_scene != scene && scene.isValid() ) {
        m_scene = scene;
        m_scene_focus_center_point = m_scene.center();
        m_zoom_factor = minZoom();
        Q_ASSERT(m_scene.contains(m_scene_focus_center_point));
	//	emit signalSceneUpdated(m_scene);
    }
}

qreal CellGLView::minZoom() const
{
    Q_ASSERT(m_scene.isValid());
    Q_ASSERT(m_viewport.isValid());
    Q_ASSERT(!m_scene.isNull());
    Q_ASSERT(!m_viewport.isNull());
    const qreal min_zoom_height = m_viewport.height( ) / m_scene.height();
    const qreal min_zoom_width = m_viewport.width() / m_scene.width();
    return qMax(min_zoom_height, min_zoom_width);
}

qreal CellGLView::maxZoom() const
{
    Q_ASSERT(m_scene.isValid());
    Q_ASSERT(m_viewport.isValid());
    Q_ASSERT(!m_scene.isNull());
    Q_ASSERT(!m_viewport.isNull());

    // Maybe we could come up with a way to calculate the
    // MAX_ZOOM_DIVIDE_FACTOR
    // Right now it is just an arbitrarily chosen number constant
    // that seems to be suited to zoom up til the resolution
    // of a few gitter boxes.

    const qreal max_zoom_height = m_viewport.height( ) / MAX_ZOOM_DIVIDE_FACTOR;
    const qreal max_zoom_width = m_viewport.width( ) / MAX_ZOOM_DIVIDE_FACTOR;
    return qMin(max_zoom_height, max_zoom_width);
}

const QImage CellGLView::grabPixmapGL() const
{
    const int w = width();
    const int h = height();
    QImage res(w, h, QImage::Format_RGB32);
    glReadBuffer(GL_FRONT_LEFT);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, res.bits());
    res = res.rgbSwapped();
    const QVector<QColor> pal = QColormap::instance().colormap();
    if (pal.size()) {
        res.setColorCount(pal.size());
        for (int i = 0; i < pal.size(); i++) {
            res.setColor(i, pal.at(i).rgb());
        }
    }
    return res.mirrored();
}

void CellGLView::zoomIn()
{
    setZoomFactorAndUpdate(m_zoom_factor * (100.0 + DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT) / 100.0);
}

void CellGLView::zoomOut()
{
    setZoomFactorAndUpdate(m_zoom_factor * (100.0 - DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT) / 100.0);
}

void CellGLView::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        m_panning = true;
        m_originPanning = event->globalPos(); //panning needs globalPos
        QPoint point = event->pos();
        // notify nodes of the mouse event
        foreach(GraphicItemGL *node, m_nodes) {
            if ( node->selectable() ) {
                //TODO should also add scene transformation is node is transformable
                const QPointF localPoint = nodeTransformations(node).inverted().map(point);
                //const QPointF localPoint = point;
                QMouseEvent newEvent(
                            event->type(),
                            localPoint,
                            event->windowPos(),
                            event->screenPos(),
                            event->button(),
                            event->buttons(),
                            event->modifiers()
                            );
                if (  node->contains(localPoint) ) {
                    node->mousePressEvent(&newEvent);
                }
            }
        }
    }
    else if ( event->button() == Qt::RightButton && !m_rubberBanding ) {
        // rubberbanding changes cursor to pointing hand
        setCursor(Qt::PointingHandCursor);
        m_rubberBanding = true;
        m_originRubberBand = event->pos();
        m_rubberBandRect = QRect();
    }
    event->ignore();
}

void CellGLView::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        unsetCursor();
        m_panning = false;
        QPoint point = event->pos();
        // notify nodes of the mouse event
        foreach(GraphicItemGL *node, m_nodes) {
            if ( node->selectable() ) {
                //TODO should also add scene transformation is node is transformable
                const QPointF localPoint = nodeTransformations(node).inverted().map(point);
                QMouseEvent newEvent(
                            event->type(),
                            localPoint,
                            event->windowPos(),
                            event->screenPos(),
                            event->button(),
                            event->buttons(),
                            event->modifiers()
                            );
                if (  node->contains(localPoint) ) {
                    node->mouseReleaseEvent(&newEvent);
                }
            }
        }
    }  else if ( event->button() == Qt::RightButton && m_rubberBanding ) {
        unsetCursor();
        const QPoint origin = m_originRubberBand;
        QPoint destiny = event->pos();
        m_rubberBandRect = QRect(qMin(origin.x(), destiny.x()), qMin(origin.y(), destiny.y()),
                                 qAbs(origin.x() - destiny.x()) + 1, qAbs(origin.y() - destiny.y()) + 1);

        //TODO paint rubberband

        // notify nodes for rubberband
        foreach(GraphicItemGL *node, m_nodes) {
            if (node->rubberBandable() ) {
                QTransform node_trans = nodeTransformations(node);
                if ( node->transformable() ) {
                    node_trans *= sceneTransformations();
                }
                // map selected area to node cordinate system
                QRectF transformed = node_trans.inverted().mapRect(m_rubberBandRect);
                // if selection area is not inside the bounding rect select empty rect
                if ( !node->boundingRect().contains(transformed) ) {
                    transformed = QRectF();
                }
                // Set the new selection area
                SelectionEvent::SelectionMode mode =
                        SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
                SelectionEvent selectionEvent(transformed, mode);
                // send selection event to node
                node->setSelectionArea(&selectionEvent);
            }
        }
        // reset variables
        m_rubberBanding = false;
        m_rubberBandRect = QRect();

        //TODO paint rubberband
    }
    event->ignore();
}

void CellGLView::setSceneFocusCenterPointWithClamping(const QPointF &center_point)
{
    QRectF allowed_center_points_rect(0,
                                      0,
                                      m_scene.width() - m_viewport.width() / m_zoom_factor,
                                      m_scene.height() - m_viewport.height() / m_zoom_factor);
    // This is another approach that is commented out where the whole image of the cell tissue will be displayed at startup
    // although the height / width proportions of the application window might very big or very small:
    //   qreal factor = 1 - (minZoom() / m_zoom_factor);
    //   QRectF allowed_center_points_rect(0, 0, m_scene.width() * factor, m_scene.height() * factor);
    allowed_center_points_rect.moveCenter(m_scene.center());
    QPointF clamped_point = center_point;
    clamped_point.setY(qMax(clamped_point.y(), allowed_center_points_rect.top()));
    clamped_point.setY(qMin(clamped_point.y(), allowed_center_points_rect.bottom()));
    clamped_point.setX(qMax(clamped_point.x(), allowed_center_points_rect.left()));
    clamped_point.setX(qMin(clamped_point.x(), allowed_center_points_rect.right()));

    if ( clamped_point != m_scene_focus_center_point) {
        m_scene_focus_center_point = clamped_point;
        update();
    }
}

void CellGLView::mouseMoveEvent(QMouseEvent *event)
{
    if ( m_panning ) {
        // panning changes cursor to closed hand
        setCursor(Qt::ClosedHandCursor);
        QPoint point = event->globalPos(); //panning needs global pos
        QPoint pan_adjustment = (point - m_originPanning) / m_zoom_factor;
        setSceneFocusCenterPointWithClamping(pan_adjustment + m_scene_focus_center_point);

        m_originPanning = point;
    }
    if ( event->button() == Qt::RightButton && m_rubberBanding  ) {
        // get rubberband
        const QPoint origin = m_originRubberBand;
        QPoint destiny = event->pos();
        m_rubberBandRect = QRect(qMin(origin.x(), destiny.x()), qMin(origin.y(), destiny.y()),
                                 qAbs(origin.x() - destiny.x()) + 1, qAbs(origin.y() - destiny.y()) + 1);

        //TODO paint rubberband
    }
    else if ( event->button() == Qt::LeftButton ) {
        QPoint point = event->pos();
        // notify nodes of the mouse event
        foreach(GraphicItemGL *node, m_nodes) {
            if ( node->selectable() ) {
                //TODO should also add scene transformation is node is transformable
                const QPointF localPoint = nodeTransformations(node).inverted().map(point);
                QMouseEvent newEvent(
                            event->type(),
                            localPoint,
                            event->windowPos(),
                            event->screenPos(),
                            event->button(),
                            event->buttons(),
                            event->modifiers()
                            );
                if (  node->contains(localPoint) ) {
                    node->mouseMoveEvent(&newEvent);
                }
            }
        }
    }
    event->ignore();
}

void CellGLView::keyPressEvent(QKeyEvent *event)
{
    const qreal shortest_side_length = qMin(m_viewport.width(), m_viewport.height());
    const qreal delta_panning_key =  shortest_side_length /
            (KEY_PRESSES_TO_MOVE_A_POINT_OVER_THE_SCREEN * m_zoom_factor);

    QPointF pan_adjustment(0,0);  
    switch(event->key()) {
    case Qt::Key_Right:
        pan_adjustment = QPoint(delta_panning_key, 0);
        break;
    case Qt::Key_Left:
        pan_adjustment = QPoint(-delta_panning_key, 0);
        break;
    case Qt::Key_Up:
        pan_adjustment = QPoint(0, -delta_panning_key);
        break;
    case Qt::Key_Down:
        pan_adjustment = QPoint(0, delta_panning_key);
        break;
    default:
        break;
    }
    setSceneFocusCenterPointWithClamping(pan_adjustment + m_scene_focus_center_point);
    event->ignore();
}

const QTransform CellGLView::sceneTransformations() const
{
    QTransform transform;
    const QPointF point = m_scene.center() + (m_scene.center() - m_scene_focus_center_point);
    transform.translate(point.x(), point.y());
    transform.scale(1 / m_zoom_factor, 1 / m_zoom_factor);
    transform.translate(-m_viewport.width() / 2.0,  -m_viewport.height() / 2.0);
    if ( m_rotate != 0.0 ) {
        //TOFIX should rotate around its center
        transform.rotate(m_rotate, Qt::ZAxis);
    }
    return transform.inverted();
}

const QTransform CellGLView::nodeTransformations(GraphicItemGL *node) const
{
    const QSizeF viewSize = m_viewport.size();
    QTransform transform(Qt::Uninitialized);
    const Globals::Anchor anchor = node->anchor();

    switch (anchor)
    {
    case Globals::Anchor::Center:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5, viewSize.height() * 0.5);
        break;
    case Globals::Anchor::North:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5, 0.0);
        break;
    case Globals::Anchor::NorthEast:
        transform = QTransform::fromTranslate(viewSize.width(), 0.0);
        break;
    case Globals::Anchor::East:
        transform = QTransform::fromTranslate(viewSize.width(), viewSize.height() * 0.5);
        break;
    case Globals::Anchor::SouthEast:
        transform = QTransform::fromTranslate(viewSize.width(), viewSize.height());
        break;
    case Globals::Anchor::South:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5, viewSize.height());
        break;
    case Globals::Anchor::SouthWest:
        transform = QTransform::fromTranslate(0.0, viewSize.height());
        break;
    case Globals::Anchor::West:
        transform = QTransform::fromTranslate(0.0, viewSize.height() * 0.5);
        break;
    case Globals::Anchor::NorthWest:
    case Globals::Anchor::None:
        // fall trough
    default:
        transform = QTransform::fromTranslate(0.0, 0.0);
        break;
    }
    if ( node->invertedX() || node->invertedY() ) {
        transform.scale(node->invertedX() ? -1.0 : 1.0, node->invertedY() ? -1.0 : 1.0);
    }
    return node->transform() * transform;
}

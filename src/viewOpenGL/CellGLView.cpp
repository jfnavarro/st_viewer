/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CellGLView.h"
#include "RubberbandGL.h"
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
#include <QOpenGLFramebufferObject>
#include <QTransform>

//TODO make this names smaller!!!
static const qreal DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT = 10.0;
static const int KEY_PRESSES_TO_MOVE_A_POINT_OVER_THE_SCREEN = 10;
static const int MIN_NUM_IMAGE_PIXELS_PER_SCREEN_IN_MAX_ZOOM = 100;
static const int DEFAULT_MIN_ZOOM = 1;
static const int DEFAULT_MAX_ZOOM = 100;

namespace   {

bool nodeIsSelectableButNotTransformable(const GraphicItemGL &node)
{
    return !node.transformable() && node.selectable();
}

bool nodeIsSelectable(const GraphicItemGL &node)
{
    return node.selectable();
}

}

CellGLView::CellGLView(UpdateBehavior updateBehavior, QWindow *parent) :
    QOpenGLWindow(updateBehavior, parent),
    m_originPanning(-1, -1),
    m_originRubberBand(-1, -1),
    m_panning(false),
    m_rubberBanding(false),
    m_selecting(false),
    m_rubberband(nullptr),
    m_rotate(0.0),
    m_zoom_factor(1.0)
{
    //init projection matrix to id
    m_projm.setToIdentity();

    //creates and sets the OpenGL format and surface type
    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setStereo(false);
    format.setStencilBufferSize(0);
    format.setDepthBufferSize(0);
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);

    // creates OpenGL context and make it current
    create();
    makeCurrent();

    //TODO consider decoupling rubberband object and view
    m_rubberband = new RubberbandGL(this);
    m_rubberband->setAnchor(Globals::Anchor::None);
}

CellGLView::~CellGLView()
{

}

void CellGLView::setDefaultPanningAndZooming()
{
    m_scene_focus_center_point = m_scene.center();
    m_zoom_factor = minZoom();
    emit signalSceneTransformationsUpdated(sceneTransformations());
}

void CellGLView::clearData()
{
    m_originPanning = QPoint(-1, -1);
    m_originRubberBand = QPoint(-1, -1);
    m_panning = false;
    m_rubberBanding = false;
    m_selecting = false;
    m_rotate = 0.0;
    setDefaultPanningAndZooming();
}

void CellGLView::initializeGL()
{
    //initializeOpenGLFunctions();

    // initialize painter
    m_painter.begin();

    // configure OpenGL variables
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);

    // Set the default blend options.
    if (m_painter.hasOpenGLFeature(QOpenGLFunctions::BlendColor)) {
        m_painter.glBlendColor(0, 0, 0, 0);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquation)) {
        m_painter.glBlendEquation(GL_FUNC_ADD);
    }

    if (m_painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate)) {
        m_painter.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    }
}

void CellGLView::paintGL()
{
    // sets the projection matrix of the OpenGL painter
    m_painter.projectionMatrix() = m_projm;

    // clear color buffer
    m_painter.setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT);

    //render nodes
    foreach(GraphicItemGL *node, m_nodes) {
        if (node->visible()) {
            QTransform local_transform = nodeTransformations(node);
            if (node->transformable()) {
                local_transform *= sceneTransformations();
            }
            m_painter.modelViewMatrix().push();
            m_painter.modelViewMatrix() *= local_transform;
            node->draw(&m_painter);
            m_painter.modelViewMatrix().pop();
        }
    }

    // paint rubberband if selecting
    if (m_rubberBanding && m_selecting) {
        m_rubberband->draw(&m_painter);
    }
}

void CellGLView::resizeGL(int width, int height)
{
    const QRectF newViewport = QRectF(0.0, 0.0, width, height);

    //update projection matrix
    m_projm.setToIdentity();
    m_projm.ortho(newViewport);

    //create viewport
    glViewport(0.0, 0.0, width, height);
    setViewPort(newViewport);

    //update local variables
    if (m_scene.isValid()) {
        m_zoom_factor = clampZoomFactorToAllowedRange(m_zoom_factor);
        setSceneFocusCenterPointWithClamping(m_scene_focus_center_point);
        emit signalSceneTransformationsUpdated(sceneTransformations());
    }
}

void CellGLView::wheelEvent(QWheelEvent* event)
{
    const qreal zoomFactor = qPow(4.0 / 3.0, (event->delta() / 240.0));
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

qreal CellGLView::clampZoomFactorToAllowedRange(const qreal zoom) const
{
    Q_ASSERT(minZoom() < maxZoom());
    return qMin(qMax(minZoom(), zoom), maxZoom());
}

void CellGLView::setZoomFactorAndUpdate(const qreal zoom)
{
    const qreal new_zoom_factor = clampZoomFactorToAllowedRange(zoom);
    if (m_zoom_factor != new_zoom_factor) {
        m_zoom_factor = new_zoom_factor;
        setSceneFocusCenterPointWithClamping(m_scene_focus_center_point);
        emit signalSceneTransformationsUpdated(sceneTransformations());
        update();
    }
}

void CellGLView::centerOn(const QPointF& point)
{
    setSceneFocusCenterPointWithClamping(point);
}

void CellGLView::rotate(qreal angle)
{
    if (angle >= -180.0 && angle <= 180.0 && m_rotate != angle) {
        m_rotate += angle;
        STMath::clamp(m_rotate, -360.0, 360.0);
        emit signalSceneTransformationsUpdated(sceneTransformations());
        update();
    }
}

void CellGLView::setViewPort(const QRectF viewport)
{
    if (!viewport.isValid() || viewport.isEmpty() || viewport.isNull()) {
        return;
    }

    if (m_viewport != viewport) {
        m_viewport = viewport;
        emit signalViewPortUpdated(m_viewport);
        emit signalSceneTransformationsUpdated(sceneTransformations());
    }
}

void CellGLView::setScene(const QRectF scene)
{
    if (!scene.isValid() || scene.isEmpty() || scene.isNull()) {
        return;
    }

    if (m_scene != scene) {
        m_scene = scene;
        setDefaultPanningAndZooming();
        emit signalSceneUpdated(m_scene);
    }
}

qreal CellGLView::minZoom() const
{
    if (!m_viewport.isValid() || !m_scene.isValid()) {
        return DEFAULT_MIN_ZOOM;
    }

    const qreal min_zoom_height = m_viewport.height( ) / m_scene.height();
    const qreal min_zoom_width = m_viewport.width() / m_scene.width();
    return qMax(min_zoom_height, min_zoom_width);
}

qreal CellGLView::maxZoom() const
{
    if (!m_viewport.isValid() || !m_scene.isValid()) {
        return DEFAULT_MAX_ZOOM;
    }

    const qreal max_zoom_x = m_viewport.width() /
            MIN_NUM_IMAGE_PIXELS_PER_SCREEN_IN_MAX_ZOOM;
    const qreal max_zoom_y = m_viewport.height() /
            MIN_NUM_IMAGE_PIXELS_PER_SCREEN_IN_MAX_ZOOM;
    return qMin(max_zoom_x, max_zoom_y);
}

const QImage CellGLView::grabPixmapGL()
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

void CellGLView::setSelectionMode(const bool selectionMode)
{
    m_selecting = selectionMode;
}

void CellGLView::zoomIn()
{
    setZoomFactorAndUpdate(m_zoom_factor *
                           (100.0 + DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT) / 100.0);
}

void CellGLView::zoomOut()
{
    setZoomFactorAndUpdate(m_zoom_factor *
                           (100.0 - DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT) / 100.0);
}

bool CellGLView::sendMouseEventToNodes(const QPoint point, const QMouseEvent *event,
                                       const MouseEventType type, const FilterFunc filterFunc)

{
    bool mouseEventWasSentToAtleastOneNode = false;
    foreach(GraphicItemGL *node, m_nodes) {
        const QPointF localPoint = nodeTransformations(node).inverted().map(point);
        if (filterFunc(*node) && node->contains(localPoint)) {
            mouseEventWasSentToAtleastOneNode = true;
            QMouseEvent newEvent(
                        event->type(),
                        localPoint,
                        event->windowPos(),
                        event->screenPos(),
                        event->button(),
                        event->buttons(),
                        event->modifiers()
                        );
            if (type == pressType) {
                node->mousePressEvent(&newEvent);
            } else if (type == moveType) {
                node->mouseMoveEvent(&newEvent);
            } else if (type == releaseType) {
                node->mouseReleaseEvent(&newEvent);
            } else {
                qDebug() << "Mouse event type not recognized";
            }
        }
    }

    return mouseEventWasSentToAtleastOneNode;
}

void CellGLView::sendRubberBandEventToNodes(const QRectF rubberBand,
                                            const QMouseEvent *event) {
    // notify nodes for rubberband
    foreach(GraphicItemGL *node, m_nodes) {
        if (node->rubberBandable()) {
            //apply scene transformations to node
            QTransform node_trans = nodeTransformations(node);
            if (node->transformable()) {
                node_trans *= sceneTransformations();
            }

            // map selected area to node cordinate system
            QRectF transformed = node_trans.inverted().mapRect(rubberBand);
            // if selection area is not inside the bounding rect select empty rect
            if (!node->boundingRect().contains(transformed)) {
                qDebug() << "Discarding selections";
                transformed = QRectF();
            }

            // Set the new selection area
            const SelectionEvent::SelectionMode mode =
                    SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
            const SelectionEvent selectionEvent(transformed, mode);
            // send selection event to node
            node->setSelectionArea(&selectionEvent);
        }
    }
}

void CellGLView::mousePressEvent(QMouseEvent *event)
{
    const QPoint point = event->pos();

    if (event->button() == Qt::LeftButton && m_selecting && !m_rubberBanding) {
        // rubberbanding changes cursor to pointing hand
        setCursor(Qt::PointingHandCursor);
        m_rubberBanding = true;
        m_originRubberBand = event->pos();
        m_rubberband->setRubberbandRect(QRect());
        update();
    } else {
        // first send the event to any non-transformable nodes under the mouse click
        const bool mouseEventCaptureByNode = sendMouseEventToNodes(point,
                                                                   event,
                                                                   pressType,
                                                                   nodeIsSelectableButNotTransformable);
        if (!mouseEventCaptureByNode) {
            // no non-transformable nodes under the mouse click were found.
            if (event->button() == Qt::LeftButton && !m_selecting) {
                m_panning = true;
                m_originPanning = event->globalPos(); //panning needs globalPos
                // panning changes cursor to closed hand
                setCursor(Qt::ClosedHandCursor);
                //TODO this sends the event twice to a node selectable but not transformable
                //find a better way to do this
                // notify nodes of the mouse event
                sendMouseEventToNodes(point, event, pressType, nodeIsSelectable);
            }
        }
    }

    event->ignore();
}

void CellGLView::mouseReleaseEvent(QMouseEvent *event)
{
    // first check if we are selecting
    if (event->button() == Qt::LeftButton && m_selecting && m_rubberBanding) {
        unsetCursor();
        const QPoint origin = m_originRubberBand;
        const QPoint destiny = event->pos();
        const QRectF rubberBandRect = QRect(qMin(origin.x(), destiny.x()),
                                            qMin(origin.y(), destiny.y()),
                                            qAbs(origin.x() - destiny.x()) + 1,
                                            qAbs(origin.y() - destiny.y()) + 1);
        sendRubberBandEventToNodes(rubberBandRect, event);
        // reset rubberband variables
        m_rubberBanding = false;
        m_rubberband->setRubberbandRect(QRect());
        //well, there is no need to trigger an update here since
        //sendRubberBandEventToNodes will make the GeneRenderer node trigger an update
        //update();
    } else if (event->button() == Qt::LeftButton && m_panning && !m_selecting) {
        unsetCursor();
        m_panning = false;
        const QPoint point = event->pos();
        // notify nodes of the mouse event
        sendMouseEventToNodes(point, event, releaseType, nodeIsSelectable);
    }

    event->ignore();
}

void CellGLView::mouseMoveEvent(QMouseEvent *event)
{
    // first check if we are in selection mode
    if (event->buttons() & Qt::LeftButton && m_selecting && m_rubberBanding ) {
        // get rubberband
        const QPoint origin = m_originRubberBand;
        const QPoint destiny = event->pos();
        const QRectF rubberBandRect = QRect(qMin(origin.x(), destiny.x()),
                                            qMin(origin.y(), destiny.y()),
                                            qAbs(origin.x() - destiny.x()) + 1,
                                            qAbs(origin.y() - destiny.y()) + 1);
        m_rubberband->setRubberbandRect(rubberBandRect);
        update();
    } else if (event->buttons() & Qt::LeftButton &&  m_panning && !m_selecting) {
        const QPoint point = event->globalPos(); //panning needs global pos
        const QPointF pan_adjustment = QPointF(point - m_originPanning) / m_zoom_factor;
        setSceneFocusCenterPointWithClamping(pan_adjustment + m_scene_focus_center_point);
        m_originPanning = point;
    } else {
        const QPoint point = event->pos();
        // notify nodes of the mouse event
        sendMouseEventToNodes(point, event, moveType, nodeIsSelectable);
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
        pan_adjustment = QPoint(-delta_panning_key, 0);
        break;
    case Qt::Key_Left:
        pan_adjustment = QPoint(delta_panning_key, 0);
        break;
    case Qt::Key_Up:
        pan_adjustment = QPoint(0, delta_panning_key);
        break;
    case Qt::Key_Down:
        pan_adjustment = QPoint(0, -delta_panning_key);
        break;
    default:
        break;
    }

    setSceneFocusCenterPointWithClamping(pan_adjustment + m_scene_focus_center_point);
    event->ignore();
}

QRectF CellGLView::allowedCenterPoints() const {
    QRectF allowed_center_points(0, 0,
                                 qMax(m_scene.width() - m_viewport.width() / m_zoom_factor, 0.0),
                                 qMax(m_scene.height() - m_viewport.height() / m_zoom_factor, 0.0));
    allowed_center_points.moveCenter(m_scene.center());
    return allowed_center_points;
}

QPointF CellGLView::sceneFocusCenterPoint() const
{
    return m_scene_focus_center_point;
}

void CellGLView::setSceneFocusCenterPointWithClamping(const QPointF center_point)
{
    const QRectF allowed_center_points_rect = allowedCenterPoints();
    QPointF clamped_point = center_point;
    clamped_point.setY(qMax(clamped_point.y(), allowed_center_points_rect.top()));
    clamped_point.setY(qMin(clamped_point.y(), allowed_center_points_rect.bottom()));
    clamped_point.setX(qMax(clamped_point.x(), allowed_center_points_rect.left()));
    clamped_point.setX(qMin(clamped_point.x(), allowed_center_points_rect.right()));
    if (clamped_point != m_scene_focus_center_point) {
        m_scene_focus_center_point = clamped_point;
        emit signalSceneTransformationsUpdated(sceneTransformations());
        update();
    }
}

const QTransform CellGLView::sceneTransformations() const
{
    QTransform transform;
    const QPointF point = m_scene.center() + (m_scene.center() - m_scene_focus_center_point);
    transform.translate(point.x(), point.y());
    transform.scale(1 / m_zoom_factor, 1 / m_zoom_factor);
    transform.translate(-m_viewport.width() / 2.0,  -m_viewport.height() / 2.0);
    if (m_rotate != 0.0) {
        //TODO should rotate around its center, complete rotation
        transform.rotate(m_rotate, Qt::ZAxis);
    }

    return transform.inverted();
}

const QTransform CellGLView::nodeTransformations(GraphicItemGL *node) const
{
    //this functions combines the node internal transformations with respect
    //to the view size and anchor positions

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

    if (node->invertedX() || node->invertedY()) {
        transform.scale(node->invertedX() ? -1.0 : 1.0, node->invertedY() ? -1.0 : 1.0);
    }

    return node->transform() * transform;
}

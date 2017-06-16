#include "CellGLView.h"

#include <QWheelEvent>
#include <QtOpenGL>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QGuiApplication>
#include <QRubberBand>
#include <QOpenGLFramebufferObject>
#include <QTransform>

#include "math/Common.h"

static const float DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT = 10.0;
static const int KEY_OFFSET = 10;
static const int MIN_PIXELS_MAX_ZOOM = 100;
static const int DEFAULT_MIN_ZOOM = 1;
static const int DEFAULT_MAX_ZOOM = 100;
static const int OPENGL_VERSION_MAJOR = 2;
static const int OPENGL_VERSION_MINOR = 0;
static const QColor lasso_color = QColor(0,0,255,90);

CellGLView::CellGLView(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_originPanning(-1, -1)
    , m_originRubberBand(-1, -1)
    , m_originLasso(-1, -1)
    , m_panning(false)
    , m_rubberBanding(false)
    , m_lassoSelection(false)
    , m_selecting(false)
    , m_rubberband(nullptr)
    , m_lasso()
    , m_scene_focus_center_point(-1, -1)
    , m_zoom_factor(1.0)
{
    // init projection matrix to identity
    m_projm.setToIdentity();

    // init rubber band object
    m_rubberband.reset(new QRubberBand(QRubberBand::Rectangle, this));
    QPalette palette;
    palette.setBrush(QPalette::Highlight, QBrush(lasso_color));
    m_rubberband->setPalette(palette);

    // Configure OpenGL format for this view
    QSurfaceFormat format;
    format.setVersion(OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
    format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setStereo(false);
    format.setStencilBufferSize(0);
    format.setDepthBufferSize(0);
    format.setSwapInterval(0);
    setFormat(format);
}

CellGLView::~CellGLView()
{
}

void CellGLView::clearData()
{
    m_originPanning = QPoint(-1, -1);
    m_originRubberBand = QPoint(-1, -1);
    m_originLasso = QPoint(-1, -1);
    m_panning = false;
    m_rubberBanding = false;
    m_selecting = false;
    m_lassoSelection = false;
    m_zoom_factor = 1;
    m_scene_focus_center_point = QPoint(-1, -1);
}

void CellGLView::initializeGL()
{
    if (!m_qopengl_functions.initializeOpenGLFunctions()) {
        QMessageBox::critical(this,
                              tr("STViewer"),
                              tr("Required OpenGL version not supported.\n"
                                 "Please update your system to at least OpenGL ")
                              + QString("%1.%2").arg(OPENGL_VERSION_MAJOR).arg(OPENGL_VERSION_MINOR)
                              + ".");
        QApplication::exit();
        return;
    }

    m_qopengl_functions.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // configure OpenGL variables
    m_qopengl_functions.glDisable(GL_TEXTURE_2D);
    m_qopengl_functions.glDisable(GL_DEPTH_TEST);
    m_qopengl_functions.glDisable(GL_COLOR_MATERIAL);
    m_qopengl_functions.glDisable(GL_CULL_FACE);
    m_qopengl_functions.glShadeModel(GL_SMOOTH);
    m_qopengl_functions.glEnable(GL_BLEND);

    // set the default blending options.
    m_qopengl_functions.glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
    m_qopengl_functions.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_qopengl_functions.glBlendEquation(GL_FUNC_ADD);
    m_qopengl_functions.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void CellGLView::paintGL()
{
    // clear color buffer
    m_qopengl_functions.glClear(GL_COLOR_BUFFER_BIT);

    QPainter painter(this);
    painter.setWorldMatrixEnabled(true);
    painter.setViewTransformEnabled(true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // render nodes
    for (auto node : m_nodes) {
        if (node->visible()) {
            QTransform local_transform = nodeTransformations(node);
            if (node->transformable()) {
                local_transform *= sceneTransformations();
            }
            painter.setWorldTransform(local_transform);
            m_qopengl_functions.glMatrixMode(GL_MODELVIEW);
            m_qopengl_functions.glLoadMatrixf(
                        reinterpret_cast<const GLfloat *>(QMatrix4x4(local_transform).constData()));
            node->draw(m_qopengl_functions, painter);
            painter.resetTransform();
        }
    }

    if (m_selecting && m_lassoSelection && !m_lasso.isEmpty()) {
        painter.setBrush(lasso_color);
        painter.setPen(lasso_color);
        painter.drawPath(m_lasso.simplified());
    }

    m_qopengl_functions.glLoadIdentity();
}

void CellGLView::resizeGL(int width, int height)
{
    const QRectF newViewport = QRectF(0.0, 0.0, width, height);

    // update projection matrix
    m_projm.setToIdentity();
    m_projm.ortho(newViewport);

    // sets the OpenGL viewport
    m_qopengl_functions.glViewport(0.0f, 0.0f, width, height);

    // reset the OpenGL coordinate system with the projection matrix
    m_qopengl_functions.glMatrixMode(GL_PROJECTION);
    m_qopengl_functions.glLoadMatrixf(reinterpret_cast<const GLfloat *>(m_projm.constData()));

    // reset the OpenGL model view matrix
    m_qopengl_functions.glMatrixMode(GL_MODELVIEW);
    m_qopengl_functions.glLoadIdentity();

    // create viewport
    setViewPort(newViewport);

    // update local variables for zooming and scene resolution
    if (m_scene.isValid()) {
        m_zoom_factor = clampZoomFactorToAllowedRange(m_zoom_factor);
        setSceneFocusCenterPointWithClamping(m_scene_focus_center_point);
    }
}

void CellGLView::wheelEvent(QWheelEvent *event)
{
    // computes zoom factor and update zoom
    const float zoomFactor = qPow(4.0 / 3.0, (event->delta() / 240.0));
    setZoomFactorAndUpdate(zoomFactor * m_zoom_factor);
    event->ignore();
}

void CellGLView::addRenderingNode(QSharedPointer<GraphicItemGL> node)
{
    Q_ASSERT(!node.isNull());
    m_nodes.append(node);
    connect(node.data(), SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::removeRenderingNode(QSharedPointer<GraphicItemGL> node)
{
    Q_ASSERT(!node.isNull());
    m_nodes.removeOne(node);
    disconnect(node.data(), SIGNAL(updated()), this, SLOT(update()));
}

float CellGLView::clampZoomFactorToAllowedRange(const float zoom) const
{
    Q_ASSERT(minZoom() < maxZoom());
    return qMin(qMax(minZoom(), zoom), maxZoom());
}

void CellGLView::setZoomFactorAndUpdate(const float zoom)
{
    const float new_zoom_factor = clampZoomFactorToAllowedRange(zoom);
    if (m_zoom_factor != new_zoom_factor) {
        m_zoom_factor = new_zoom_factor;
        setSceneFocusCenterPointWithClamping(m_scene_focus_center_point);
        update();
    }
}

void CellGLView::setViewPort(const QRectF &viewport)
{
    if (!viewport.isValid() || viewport.isEmpty() || viewport.isNull()) {
        return;
    }

    if (m_viewport != viewport) {
        m_viewport = viewport;
    }
}

void CellGLView::setScene(const QRectF &scene)
{
    if (!scene.isValid() || scene.isEmpty() || scene.isNull()) {
        return;
    }

    if (m_scene != scene) {
        m_scene = scene;
        qDebug() << "Setting graphic view scene to " << m_scene;
        m_scene_focus_center_point = m_scene.center();
        m_zoom_factor = minZoom();
    }
}

float CellGLView::minZoom() const
{
    // we want to the min zoom to at least covers the whole image
    if (!m_viewport.isValid() || !m_scene.isValid()) {
        return DEFAULT_MIN_ZOOM;
    }

    const float min_zoom_height = m_viewport.height() / m_scene.height();
    const float min_zoom_width = m_viewport.width() / m_scene.width();
    return qMin(min_zoom_height, min_zoom_width);
}

float CellGLView::maxZoom() const
{
    // we want the max zoom to have a min number of pixes visible
    if (!m_viewport.isValid() || !m_scene.isValid()) {
        return DEFAULT_MAX_ZOOM;
    }

    const float max_zoom_x = m_viewport.width() / MIN_PIXELS_MAX_ZOOM;
    const float max_zoom_y = m_viewport.height() / MIN_PIXELS_MAX_ZOOM;
    return qMin(max_zoom_x, max_zoom_y);
}

const QImage CellGLView::grabPixmapGL()
{
    QPixmap res = grab(QRect(0,0,width(),height()));
    return res.toImage();
}

void CellGLView::setSelectionMode(const bool selectionMode)
{
    m_selecting = selectionMode;
}

void CellGLView::setLassoSelectionMode(const bool lassoSelectionMode)
{
    m_lassoSelection = lassoSelectionMode;
}

void CellGLView::zoomIn()
{
    setZoomFactorAndUpdate(m_zoom_factor * (100.0 + DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT) / 100.0);
}

void CellGLView::zoomOut()
{
    setZoomFactorAndUpdate(m_zoom_factor * (100.0 - DEFAULT_ZOOM_ADJUSTMENT_IN_PERCENT) / 100.0);
}

void CellGLView::rotate(const int angle)
{
    Q_UNUSED(angle)
}

void CellGLView::sendSelectionToNodes(const QPainterPath &path, const QMouseEvent *event)
{
    // notify nodes for rubberband
    for (const auto &node : m_nodes) {
        if (node->rubberBandable()) {
            // apply scene transformations to node
            QTransform node_trans = nodeTransformations(node);
            if (node->transformable()) {
                node_trans *= sceneTransformations();
            }

            // map selected area to node cordinate system
            QPainterPath transformed = node_trans.inverted().map(path);

            // if selection area is not inside the bounding rect select empty rect
            if (!transformed.intersects(node->boundingRect())) {
                transformed = QPainterPath();
            }

            // Set the new selection area
            const SelectionEvent::SelectionMode mode
                    = SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
            const SelectionEvent selectionEvent(transformed, mode);

            // send selection event to node
            node->setSelectionArea(selectionEvent);
        }
    }
}

void CellGLView::mousePressEvent(QMouseEvent *event)
{
    const bool is_left = event->button() == Qt::LeftButton;
    if (is_left && m_selecting && !m_lassoSelection) {
        // rubberbanding changes cursor to pointing hand
        setCursor(Qt::PointingHandCursor);
        m_rubberBanding = true;
        m_originRubberBand = event->pos();
        m_rubberband->setGeometry(QRect(m_originRubberBand, QSize()));
        m_rubberband->show();
    } else if (is_left && m_selecting) {
        m_lasso = QPainterPath();
        m_originLasso = event->pos();
        m_lasso.moveTo(m_originLasso);
        update();
    } else if (is_left) {
        m_panning = true;
        m_originPanning = event->globalPos(); // panning needs globalPos
        // panning changes cursor to closed hand
        setCursor(Qt::ClosedHandCursor);
    }
    event->ignore();
}

void CellGLView::mouseMoveEvent(QMouseEvent *event)
{
    const bool is_left = event->buttons() & Qt::LeftButton;
    // first check if we are in selection mode
    if (is_left && m_selecting && m_rubberBanding) {
        // update the rubber band
        m_rubberband->setGeometry(QRect(m_originRubberBand, event->pos()).normalized());
    } else if (is_left && m_selecting) {
        const QPoint new_point = event->pos();
        if ((new_point - m_originLasso).manhattanLength() > 5) {
            m_lasso.lineTo(new_point);
            m_originLasso = new_point;
            update();
        }
    } else if (is_left && m_panning) {
        // user is moving the view (panning)
        const QPoint point = event->globalPos(); // panning needs global pos
        const QPointF pan_adjustment = QPointF(point - m_originPanning) / m_zoom_factor;
        setSceneFocusCenterPointWithClamping(pan_adjustment + m_scene_focus_center_point);
        m_originPanning = point;
    }
    event->ignore();
}

void CellGLView::mouseReleaseEvent(QMouseEvent *event)
{
    const bool is_left = event->button() == Qt::LeftButton;
    if (is_left && m_selecting && m_rubberBanding) {
        unsetCursor();
        const QRectF rubberBandRect = m_rubberband->geometry();
        QPainterPath path;
        path.addRect(rubberBandRect);
        sendSelectionToNodes(path, event);
        m_rubberband->hide();
        m_rubberBanding = false;
    } else if (is_left && m_selecting) {
        sendSelectionToNodes(m_lasso, event);
        m_lasso = QPainterPath();
        update();
    } else if (is_left && m_panning) {
        unsetCursor();
        m_panning = false;
    }
    event->ignore();
}

void CellGLView::keyPressEvent(QKeyEvent *event)
{
    const float shortest_side_length = qMin(m_viewport.width(), m_viewport.height());
    const float delta_panning_key = shortest_side_length / (KEY_OFFSET * m_zoom_factor);

    QPointF pan_adjustment(0, 0);
    switch (event->key()) {
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
    // move the view
    setSceneFocusCenterPointWithClamping(pan_adjustment + m_scene_focus_center_point);
    event->ignore();
}

QRectF CellGLView::allowedCenterPoints() const
{
    QRectF allowed_center_points(0,
                                 0,
                                 qMax(m_scene.width() - m_viewport.width() / m_zoom_factor, 0.0),
                                 qMax(m_scene.height() - m_viewport.height() / m_zoom_factor, 0.0));
    allowed_center_points.moveCenter(m_scene.center());
    return allowed_center_points;
}

void CellGLView::setSceneFocusCenterPointWithClamping(const QPointF &center_point)
{
    const QRectF allowed_center_points_rect = allowedCenterPoints();
    QPointF clamped_point = center_point;
    clamped_point.setY(qMax(clamped_point.y(), allowed_center_points_rect.top()));
    clamped_point.setY(qMin(clamped_point.y(), allowed_center_points_rect.bottom()));
    clamped_point.setX(qMax(clamped_point.x(), allowed_center_points_rect.left()));
    clamped_point.setX(qMin(clamped_point.x(), allowed_center_points_rect.right()));
    if (clamped_point != m_scene_focus_center_point) {
        m_scene_focus_center_point = clamped_point;
        update();
    }
}

const QTransform CellGLView::sceneTransformations() const
{
    // returns all the transformations applied to the scene from the user with respect to the viewport
    QTransform transform;
    const QPointF point = m_scene.center() + (m_scene.center() - m_scene_focus_center_point);
    transform.translate(point.x(), point.y());
    transform.scale(1 / m_zoom_factor, 1 / m_zoom_factor);
    transform.translate(-m_viewport.width() / 2.0, -m_viewport.height() / 2.0);
    return transform.inverted();
}

const QTransform CellGLView::nodeTransformations(QSharedPointer<GraphicItemGL> node) const
{
    // this function combines the node internal transformations with respect
    // to the view size and anchor positions to create a new transformation matrix

    const QSizeF viewSize = m_viewport.size();
    QTransform transform(Qt::Uninitialized);
    const GraphicItemGL::Anchor anchor = node->anchor();

    switch (anchor) {
    case GraphicItemGL::Anchor::Center:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5, viewSize.height() * 0.5);
        break;
    case GraphicItemGL::Anchor::North:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5, 0.0);
        break;
    case GraphicItemGL::Anchor::NorthEast:
        transform = QTransform::fromTranslate(viewSize.width(), 0.0);
        break;
    case GraphicItemGL::Anchor::East:
        transform = QTransform::fromTranslate(viewSize.width(), viewSize.height() * 0.5);
        break;
    case GraphicItemGL::Anchor::SouthEast:
        transform = QTransform::fromTranslate(viewSize.width(), viewSize.height());
        break;
    case GraphicItemGL::Anchor::South:
        transform = QTransform::fromTranslate(viewSize.width() * 0.5, viewSize.height());
        break;
    case GraphicItemGL::Anchor::SouthWest:
        transform = QTransform::fromTranslate(0.0, viewSize.height());
        break;
    case GraphicItemGL::Anchor::West:
        transform = QTransform::fromTranslate(0.0, viewSize.height() * 0.5);
        break;
    case GraphicItemGL::Anchor::NorthWest:
    case GraphicItemGL::Anchor::None:
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

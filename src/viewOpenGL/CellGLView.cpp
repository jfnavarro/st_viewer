 #include "CellGLView.h"

#include <QGLPainter>
#include <QArray>
#include <QWheelEvent>
#include <QtOpenGL>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QGuiApplication>

static const qreal DEFAULT_ZOOM_MIN = 1.0f;
static const qreal DEFAULT_ZOOM_MAX = 20.0f;
static const qreal DEFAULT_ZOOM_IN  = qreal(1.1 / 1.0);
static const qreal DEFAULT_ZOOM_OUT = qreal(1.0 / 1.1);
static const qreal DELTA_PANNING = 2.5f;
static const qreal DELTA_MOUSE_PANNING = 0.5f;

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

    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
}

CellGLView::~CellGLView()
{
    //NOTE View does not own rendering nodes nor texture
    //TODO double check for memmory leaks
}

void CellGLView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    ensureContext();
    if ( !m_initialized ) {
        initializeGL();
    }
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
    paintGL();
    m_context->swapBuffers(this);
}

void CellGLView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QRect rect = geometry();
    Q_ASSERT(event->size() == rect.size());

    if ( rect.size() != m_viewport.size() )
    {
        ensureContext();
        if ( !m_initialized ) {
            initializeGL();
        }
        resizeGL(rect.width(), rect.height());
        m_viewport = rect;
        emit signalSceneUpdated(m_viewport);
    }
}

void CellGLView::ensureContext()
{
    if ( !m_context ) {
        m_context = new QOpenGLContext();
        m_context->setFormat(format);
        const bool success = m_context->create();
        qDebug() << "Creating OpenGL context = " << success;
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

    glShadeModel(GL_FLAT); // or GL_SMOOTH
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QRect rect = geometry();
    resizeGL(rect.width(), rect.height());
    m_initialized = true;
}

void CellGLView::paintGL()
{
    QGLTexture2D::processPendingResourceDeallocations(); //check this

    QGLPainter painter;
    painter.begin();

    painter.setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projm;
    projm.ortho(QRectF(0.0f, 0.0f, width(), height()));
    painter.projectionMatrix() = projm;

    // draw rendering nodes
    foreach(GraphicItemGL *node, m_nodes) {

        if ( node->visible() ) {

            painter.modelViewMatrix().push();
            painter.projectionMatrix().push();

            if ( node->invertedX() || node->invertedY() ) {
                painter.projectionMatrix().scale(node->invertedX() ? -1.0f : 1.0f,
                                                             node->invertedY() ? -1.0f : 1.0f, 0.0f);
            }

            if ( node->transformable() ) {
                painter.projectionMatrix().scale(m_zoom, m_zoom, 0.0f);
                painter.projectionMatrix().translate(m_panx, m_pany, 0.0f);
            }

            painter.modelViewMatrix() *= node->transform() * anchorTransform(node->anchor());

            node->draw(&painter);

            painter.projectionMatrix().pop();
            painter.modelViewMatrix().pop();
        }
    }

    glFlush(); // forces to send the data to the GPU saving time
}

void CellGLView::resizeGL(int width, int height)
{
    //devicePixelRatio() fixes the problem with MAC retina
    qreal pixelRatio = devicePixelRatio();
    glViewport(0.0f, 0.0f, width * pixelRatio, height * pixelRatio);
}

void CellGLView::wheelEvent(QWheelEvent* event)
{
    qreal zoomFactor = qPow(4.0 / 3.0, (event->delta() / 240.0));
    setZoom(zoomFactor * m_zoom);
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

void CellGLView::setZoom(qreal delta)
{
    if (m_zoom != delta) {
        m_zoom = delta;
        update();
    }
}

void CellGLView::centerOn(const QPointF& point)
{
    QVector3D center_point(point.x(), point.y(), 0.0f);
    qDebug() << "Center on = " << center_point;
}

void CellGLView::rotate(int angle)
{
    Q_UNUSED(angle);
}

const QImage CellGLView::grabPixmapGL() const
{
    //TODO
    return QImage();
}

void CellGLView::zoomIn()
{
    setZoom(m_zoom * DEFAULT_ZOOM_IN);
}

void CellGLView::zoomOut()
{
    setZoom(m_zoom * DEFAULT_ZOOM_OUT);
}

void CellGLView::rotate(int deltax, int deltay, int rotation)
{
    if (rotation == 90 || rotation == 270) {
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    float anglex = deltax * 90.0f / width();
    float angley = deltay * 90.0f / height();

    Q_UNUSED(anglex);
    Q_UNUSED(angley);
    //TODO
}

void CellGLView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_panning = true;
        m_lastpos = event->globalPos();
        setCursor(Qt::ClosedHandCursor);

        QPointF point = event->localPos();
        foreach(GraphicItemGL *node, m_nodes) {
            const QPointF localPoint = (node->transform()
                                        * anchorTransform(node->anchor())).inverted().map(point);
            QMouseEvent newEvent(
                event->type(),
                localPoint,//event->localPos(),
                event->windowPos(),
                event->screenPos(),
                event->button(),
                event->buttons(),
                event->modifiers()
            );
            if ( node->selectable() && node->contains(localPoint) ){
                node->mousePressEvent(&newEvent);
            }
        }
    }
    event->ignore();
}

void CellGLView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        m_panning = false;
        unsetCursor();

        QPointF point = event->localPos();
        foreach(GraphicItemGL *node, m_nodes) {
            const QPointF localPoint = (node->transform() *
                                        anchorTransform(node->anchor())).inverted().map(point);
            QMouseEvent newEvent(
                event->type(),
                localPoint,//event->localPos(),
                event->windowPos(),
                event->screenPos(),
                event->button(),
                event->buttons(),
                event->modifiers()
            );
            if (node->selectable() && node->contains(localPoint) ) {
                node->mouseReleaseEvent(&newEvent);
            }
        }
    }
    event->ignore();
}

void CellGLView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {

        m_panx += (event->globalPos().x() - m_lastpos.x()) * DELTA_MOUSE_PANNING;
        m_pany -= (event->globalPos().y() - m_lastpos.y()) * -DELTA_MOUSE_PANNING;
        m_lastpos = event->globalPos();
        update();
    }

    QPointF point = event->localPos();
    foreach(GraphicItemGL *node, m_nodes) {
        const QPointF localPoint = (node->transform()
                                    * anchorTransform(node->anchor())).inverted().map(point);
        QMouseEvent newEvent(
            event->type(),
            localPoint,//event->localPos(),
            event->windowPos(),
            event->screenPos(),
            event->button(),
            event->buttons(),
            event->modifiers()
        );

        if ( node->selectable() && node->contains(localPoint) ) {
            node->mouseMoveEvent(&newEvent);
        }
    }

    event->ignore();
}

void CellGLView::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Right:
        m_panx += DELTA_PANNING;
        break;
    case Qt::Key_Left:
        m_panx -= DELTA_PANNING;
        break;
    case Qt::Key_Up:
        m_pany -= DELTA_PANNING;
        break;
    case Qt::Key_Down:
        m_pany += DELTA_PANNING;
        break;
    default:
        break;
    }
    update();
    event->ignore();
}

const QTransform CellGLView::anchorTransform(GraphicItemGL::Anchor anchor) const
{
    //const QSizeF viewSize = m_viewport.size();
    const QSizeF viewSize = size();
    QTransform transform(Qt::Uninitialized);

    switch (anchor)
    {
        case GraphicItemGL::Center:
            transform = QTransform::fromTranslate(viewSize.width() * 0.5f, viewSize.height() * 0.5f);
            break;
        case GraphicItemGL::North:
            transform = QTransform::fromTranslate(viewSize.width() * 0.5f, 0.0f);
            break;
        case GraphicItemGL::NorthEast:
            transform = QTransform::fromTranslate(viewSize.width(), 0.0f);
            break;
        case GraphicItemGL::East:
            transform = QTransform::fromTranslate(viewSize.width(), viewSize.height() * 0.5f);
            break;
        case GraphicItemGL::SouthEast:
            transform = QTransform::fromTranslate(viewSize.width(), viewSize.height());
            break;
        case GraphicItemGL::South:
            transform = QTransform::fromTranslate(viewSize.width() * 0.5f, viewSize.height());
            break;
        case GraphicItemGL::SouthWest:
            transform = QTransform::fromTranslate(0.0f, viewSize.height());
            break;
        case GraphicItemGL::West:
            transform = QTransform::fromTranslate(0.0f, viewSize.height() * 0.5f);
            break;
        case GraphicItemGL::NorthWest:
            // fall-through
        default:
            transform = QTransform::fromTranslate(0.0f, 0.0f);
            break;
    }
    return transform;
}

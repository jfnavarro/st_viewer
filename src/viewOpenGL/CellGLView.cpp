/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CellGLView.h"

#include "math/Common.h"

#include <QGLPainter>
#include <QArray>
#include <QWheelEvent>
#include <QtOpenGL>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QGuiApplication>
#include <QVector2DArray>

static const qreal DEFAULT_ZOOM_MIN = 1.0f;
static const qreal DEFAULT_ZOOM_MAX = 20.0f;
static const qreal DEFAULT_ZOOM_IN  = qreal(1.1 / 1.0);
static const qreal DEFAULT_ZOOM_OUT = qreal(1.0 / 1.1);
static const qreal DELTA_PANNING = 3.0f;
static const qreal DELTA_MOUSE_PANNING = 1.0f;

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

    const QRect rect = geometry();
    Q_ASSERT(event->size() == rect.size());

    if ( rect.size() != m_viewport.size() )
    {
        ensureContext();
        if ( !m_initialized ) {
            initializeGL();
        }
        resizeGL(rect.width(), rect.height());
        // update scene variable
        setScene(rect);
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

    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const QRect rect = geometry();
    resizeGL(rect.width(), rect.height());
    m_initialized = true;
}

void CellGLView::paintGL()
{
    //QGLTexture2D::processPendingResourceDeallocations(); //check this

    QGLPainter painter;
    painter.begin();

    painter.setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projm;
    projm.ortho(QRectF(0.0, 0.0, width(), height()));
    painter.projectionMatrix() = projm;

    // draw rendering nodes
    foreach(GraphicItemGL *node, m_nodes) {

        if ( node->visible() ) {

            painter.modelViewMatrix().push();
            painter.projectionMatrix().push();

            if ( node->invertedX() || node->invertedY() ) {
                painter.projectionMatrix().scale(node->invertedX() ? -1.0 : 1.0,
                                                             node->invertedY() ? -1.0 : 1.0, 0.0);
            }

            if ( node->transformable() ) {
                //TODO update scene variable size and send signal

                if ( m_zoom != 1.0 ) {
                    painter.projectionMatrix().scale(m_zoom, m_zoom, 0.0);
                }
                if ( m_panx != 0.0 && m_pany != 0.0 ) {
                    painter.projectionMatrix().translate(m_panx, m_pany, 0.0);
                }
                if ( m_rotate != 0.0 ) {
                    const QPointF center = node->boundingRect().center();
                    painter.projectionMatrix().rotate(m_rotate, center.x(), center.y(), 0.0);
                }
            }

            painter.modelViewMatrix() *= node->transform() * anchorTransform(node->anchor());

            node->draw(&painter);

            painter.projectionMatrix().pop();
            painter.modelViewMatrix().pop();
        }
    }

    //glFlush(); // forces to send the data to the GPU saving time
}

void CellGLView::resizeGL(int width, int height)
{
    //devicePixelRatio() fixes the problem with MAC retina
    qreal pixelRatio = devicePixelRatio();
    glViewport(0.0, 0.0, width * pixelRatio, height * pixelRatio);
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
    //TODO check for min and max
    if ( m_zoom != delta ) {
        m_zoom = delta;
        update();
    }
}

void CellGLView::centerOn(const QPointF& point)
{
    //TODO check and validate this
    m_panx += point.x();
    m_pany += point.y();
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
        emit signalViewPortUpdated(m_viewport);
    }
}

void CellGLView::setScene(QRectF scene)
{
    if ( m_scene != scene && scene.isValid() ) {
        m_scene = scene;
        emit signalViewPortUpdated(m_scene);
    }
}

const QImage CellGLView::grabPixmapGL() const
{
    //TOFIX image is black
    // possible reasons :
    // 1.- buffer needs to be read from the surface or the context
    // 2.- w and h needs to be expanded to the biggest node size
    const int w = width();
    const int h = height();
    QImage res(w, h, QImage::Format_Indexed8);
    glReadBuffer(GL_FRONT_LEFT);
    glReadPixels(0, 0, w, h, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, res.bits());
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
    setZoom(m_zoom * DEFAULT_ZOOM_IN);
}

void CellGLView::zoomOut()
{
    setZoom(m_zoom * DEFAULT_ZOOM_OUT);
}

void CellGLView::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        setCursor(Qt::ClosedHandCursor);
        m_panning = true;
        m_originPanning = event->globalPos();
        const QPointF point = event->localPos();

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
    else if ( event->button() == Qt::RightButton && !m_rubberBanding ) {
        // Rubberbanding changes cursor to pointing hand
        setCursor(Qt::PointingHandCursor);
        m_originRubberBand = event->globalPos();
        m_rubberBanding = true;
    }
    event->ignore();
}

void CellGLView::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        unsetCursor();
        m_panning = false;
        const QPointF point = event->localPos();

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
            if ( node->selectable() && node->contains(localPoint) ) {
                node->mouseReleaseEvent(&newEvent);
            }
        }
    }  else if ( event->button() == Qt::RightButton && m_rubberBanding ) {
        unsetCursor();
        const QPoint origin = m_originRubberBand;
        const QPointF destiny = event->localPos();

        foreach(GraphicItemGL *node, m_nodes) {
            const QPointF localOrigin = (node->transform() *
                                        anchorTransform(node->anchor())).inverted().map(origin);
            const QPointF localDestiny= (node->transform() *
                                        anchorTransform(node->anchor())).inverted().map(destiny);

            QRect rect(qMin(localOrigin.x(), localDestiny.x()), qMin(localOrigin.y(), localDestiny.y()),
                       qAbs(localOrigin.x() - localDestiny.x()) + 1, qAbs(localOrigin.y() - localDestiny.y()) + 1);

            qDebug() << "RubberBanding Area Out " << rect << "RubberBandable = " << node->rubberBandable() << " Box = " << node->boundingRect();
            if ( node->rubberBandable() && node->contains(rect) ) {
                qDebug() << "RubberBanding Area In " << rect;
            }

        }
        // reset variables
        m_rubberBanding = false;
    }

    event->ignore();
}

void CellGLView::mouseMoveEvent(QMouseEvent *event)
{
    if ( m_panning ) {
        m_panx += (event->globalPos().x() - m_originPanning.x()) * DELTA_MOUSE_PANNING;
        m_pany -= (event->globalPos().y() - m_originPanning.y()) * -DELTA_MOUSE_PANNING;
        m_originPanning = event->globalPos();
        update();
    }
    if ( event->button() == Qt::RightButton && m_rubberBanding  ) {
        // NOTE I can update rubber band rect here and draw it
        // if I want to give the effect of the selection shape expanding
    }
    else if ( event->button() == Qt::LeftButton ) {
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

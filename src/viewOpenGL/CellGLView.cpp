 #include "CellGLView.h"

#include <QGLPainter>
#include <QGLWidget>
#include <QGLSceneNode>
#include <QGLBuilder>
#include <QGeometryData>
#include <QArray>
#include <QGLCube>
#include <QGLMaterial>
#include <QGLCamera>
#include <QWheelEvent>
#include <QRay3D>
#include <QGLTexture2D>
#include <QtOpenGL>
#include <QGLPickNode>
#include <QMouseEvent>
#include <QCoreApplication>

static const QSizeF DEFAULT_ZOOM_MIN = QSizeF(1.00, 1.00);
static const QSizeF DEFAULT_ZOOM_MAX = QSizeF(20.0, 20.0);
static const QSizeF DEFAULT_ZOOM = DEFAULT_ZOOM_MIN;
static const qreal DEFAULT_ZOOM_IN  = qreal(1.1 / 1.0);
static const qreal DEFAULT_ZOOM_OUT = qreal(1.0 / 1.1);
static const QSize DEFAULT_BOUND_SIZE = QSize(100, 100);

CellGLView::CellGLView(QWindow *parent) :
    QGLView(parent)
{

}

CellGLView::CellGLView(const QSurfaceFormat &format, QWindow *parent) :
    QGLView(format,parent)
{

}

CellGLView::~CellGLView()
{
    //NOTE View does not own rendering nodes nor texture
}

void CellGLView::initializeGL(QGLPainter *painter)
{
    setOption(QGLView::FOVZoom, true); //enable orthographical zoom
    setOption(QGLView::CameraNavigation, true); // panning
    setOption(QGLView::ObjectPicking, true); //enables object selection

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);

    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    painter->setClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // SETUP 2D camera
    camera()->setProjectionType(QGLCamera::Orthographic);
    //camera()->setMinViewSize(QSizeF(600.0,600.0)); //TODO
    camera()->setViewSize(QSizeF(width(),height())); // the size of the window
    camera()->setAdjustForAspectRatio(false);
    camera()->setFieldOfView(0.0);
    camera()->setNearPlane(0.0);
    camera()->setFarPlane(1.0);
    camera()->setEyeSeparation(0.0); //2D viewport
    camera()->setUpVector(QVector3D(0.0, 1.0, 0.0));
    camera()->setCenter(QVector3D(0.0, 0.0, 0.0));
    camera()->setEye(QVector3D(0.0, 0.0, 1.0));
}

void CellGLView::paintGL(QGLPainter *painter)
{

    painter->projectionMatrix().push();
    painter->projectionMatrix() *=
            QTransform::fromTranslate(-width() / 2, -height() / 2) *=
            QTransform::fromScale(1.0f, -1.0f);

    // draw rendering nodes
    foreach(QGLSceneNode *node, m_nodes) {
        node->draw(painter);
    }

    glFlush(); // forces to send the data to the GPU saving time

    painter->projectionMatrix().pop();
}

void CellGLView::rotate()
{
    update();
}

void CellGLView::resizeGL(int width, int height)
{
    //devicePixelRatio() fix the problem with MAC retina
    qreal pixelRatio = devicePixelRatio();
    QGLView::resizeGL(width * pixelRatio, height * pixelRatio);
    //camera()->setViewSize(size()); // the size of the window
}

void CellGLView::wheelEvent(QWheelEvent* event)
{
    setZoom(qreal(event->delta()));
    event->ignore();
}

void CellGLView::addRenderingNode(QGLSceneNode *node)
{
    Q_ASSERT(node != nullptr);
    m_nodes.append(node);
    connect(node, SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::removeRenderingNode(QGLSceneNode *node)
{
    Q_ASSERT(node != nullptr);
    m_nodes.removeOne(node);
    disconnect(node, SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::setZoom(qreal delta)
{
    camera()->setViewSize( camera()->viewSize() * ( 1 - 0.001 * delta ) );
}

/*
void CellGLView::slotCenterOn(const QPointF& point)
{
    centerOn(point);
}*/

const QImage CellGLView::grabPixmapGL() const
{
    return QImage();
}

void CellGLView::nodeClicked()
{
    qDebug() << "Node Clicked";
}

void CellGLView::zoomIn()
{
    setZoom(DEFAULT_ZOOM_IN);
}

void CellGLView::zoomOut()
{
    setZoom(DEFAULT_ZOOM_OUT);
}

void CellGLView::pan(int deltax, int deltay)
{
    QVector3D t = camera()->translation(deltax, deltay, 0.0f);
    camera()->setEye(camera()->eye() - t);
    camera()->setCenter(camera()->center() - t);
}

void CellGLView::rotate(int deltax, int deltay)
{
    int rotation = camera()->screenRotation();
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
    QQuaternion q = camera()->pan(-anglex);
    q *= camera()->tilt(-angley);
    camera()->rotateCenter(q);
}

void CellGLView::mousePressEvent(QMouseEvent *e)
{
    QObject *object;

    if (!panning && (options() & QGLView::ObjectPicking) != 0) {
        object = objectForPoint(e->pos());
    }
    else {
        object = nullptr;
    }

    if (pressedObject) {
        // Send the press event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonPress,
             (pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->globalPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(pressedObject, &event);
    } else if (object) {
        // Record the object that was pressed and forward the event.
        pressedObject = object;
        enteredObject = nullptr;
        pressedButton = e->button();
        // Send a mouse press event for (0, 0).
        QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0),
                          e->globalPos(), e->button(), e->buttons(),
                          e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if ((options() & QGLView::CameraNavigation) != 0 &&
                    e->button() == Qt::LeftButton) {
        panning = true;
        lastPan = e->pos();
        startPan = e->pos();
        startEye = camera()->eye();
        startCenter = camera()->center();
        panModifiers = e->modifiers();
        setCursor(Qt::ClosedHandCursor);
    }
    //QWindow::mousePressEvent(e);
}

void CellGLView::mouseReleaseEvent(QMouseEvent *e)
{
    if (panning && e->button() == Qt::LeftButton) {
        panning = false;
        unsetCursor();
    }
    if (pressedObject) {
        // Notify the previously pressed object about the release.
        QObject *object = objectForPoint(e->pos());
        QObject *pressed = pressedObject;
        if (e->button() == pressedButton) {
            pressedObject = nullptr;
            pressedButton = Qt::NoButton;
            enteredObject = object;
            // Send the release event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseButtonRelease,
                 (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);
            // Send leave and enter events if necessary.
            if (object != pressed) {
                QEvent event(QEvent::Leave);
                QCoreApplication::sendEvent(pressed, &event);
                if (object) {
                    QEvent event(QEvent::Enter);
                    QCoreApplication::sendEvent(object, &event);
                }
            }
        } else {
            // Some other button than the original was released.
            // Forward the event to the pressed object.
            QMouseEvent event
                (QEvent::MouseButtonRelease,
                 (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);
        }
    }
    //QWindow::mouseReleaseEvent(e);
}

void CellGLView::mouseMoveEvent(QMouseEvent *e)
{
    if (panning) {

        QPoint delta = e->pos() - startPan;
        if (e->modifiers() == panModifiers) {
            camera()->setEye(startEye);
            camera()->setCenter(startCenter);
        } else {
            startPan = lastPan;
            delta = e->pos() - startPan;
            startEye = camera()->eye();
            startCenter = camera()->center();
            panModifiers = e->modifiers();
        }

        lastPan = e->pos();
        pan(delta.x(), delta.y());

    } else if ((options() & QGLView::ObjectPicking) != 0) {

        QObject *object = objectForPoint(e->pos());
        if (pressedObject) {

            // Send the move event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseMove,
                 (pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressedObject, &event);

        } else if (object) {

            if (object != enteredObject) {
                if (enteredObject) {
                    QEvent event(QEvent::Leave);
                    QCoreApplication::sendEvent(enteredObject, &event);
                }
                QEvent event(QEvent::Enter);
                QCoreApplication::sendEvent(enteredObject, &event);
            }
            QMouseEvent event
                (QEvent::MouseMove, QPoint(0, 0),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        } else if (enteredObject) {
            QEvent event(QEvent::Leave);
            QCoreApplication::sendEvent(enteredObject, &event);
            enteredObject = 0;
        }
    }
    //QWindow::mouseMoveEvent(e);
}

void CellGLView::keyPressEvent(QKeyEvent *e)
{
    e->ignore();
}

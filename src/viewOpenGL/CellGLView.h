#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QGLView>

class QGLPainter;
class QGLSceneNode;
class QSurfaceFormat;
class QGLCamera;
class QVector3D;

class CellGLView : public QGLView
{
    Q_OBJECT

public:

    explicit CellGLView(QWindow *parent = 0);
    explicit CellGLView(const QSurfaceFormat &format, QWindow *parent=0);
    virtual ~CellGLView();

    void addRenderingNode(QGLSceneNode *node);
    void removeRenderingNode(QGLSceneNode *node);

    const QImage grabPixmapGL() const;

public slots:

    void zoomOut();
    void zoomIn();

protected:

    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent* event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    // Pan left/right/up/down without rotating about the object.
    void pan(int deltax, int deltay);

    // Rotate about the object being viewed.
    void rotate(int deltax, int deltay);

signals:

private slots:

    void rotate();
    void setZoom(qreal delta);
    void nodeClicked();

private:

    QList<QGLSceneNode *> m_nodes;

    //QGLSceneNode * m_master_node = nullptr;

    QObject *enteredObject = nullptr;
    QObject *pressedObject = nullptr;
    Qt::MouseButton pressedButton = Qt::NoButton;
    bool panning = false;
    QPoint startPan = QPoint(-1, -1);
    QPoint lastPan = QPoint(-1, -1);
    QVector3D startEye;
    QVector3D startCenter;
    QVector3D startUpVector;
    Qt::KeyboardModifiers panModifiers = Qt::NoModifier;
};

#endif // CELLGLVIEW_H

#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QGLView>

class QGLPainter;
class QGLSceneNode;
class QSurfaceFormat;
class QGLCamera;

class CellGLView : public QGLView
{
    Q_OBJECT

public:

    explicit CellGLView(QWindow *parent = 0);
    explicit CellGLView(const QSurfaceFormat &format, QWindow *parent=0);
    virtual ~CellGLView();

    void addRenderingNode(QGLSceneNode *node);
    void removeRenderingNode(QGLSceneNode *node);

    void setBackGroundTexture(QGLSceneNode *node);

    const QImage grabPixmapGL() const;

public slots:

    void zoomOut();
    void zoomIn();

protected:

    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);
    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent* event);

signals:

private slots:

    void rotate();
    void setZoom(qreal delta);
    void nodeClicked();

private:

    QList<QGLSceneNode *> m_nodes;

    QGLSceneNode *m_texture = nullptr;

    //QGLSceneNode * m_master_node = nullptr;

    QSizeF m_zoom;
};

#endif // CELLGLVIEW_H

#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QWindow>
#include "GraphicItemGL.h"

class QGLPainter;
class GraphicItemGL;
class QSurfaceFormat;
class QVector3D;
class QOpenGLContext;

class CellGLView : public QWindow
{
    Q_OBJECT

public:

    explicit CellGLView(QScreen *parent = 0);
    virtual ~CellGLView();

    void addRenderingNode(GraphicItemGL *node);
    void removeRenderingNode(GraphicItemGL *node);

    const QImage grabPixmapGL() const;

public slots:

    void zoomOut();
    void zoomIn();

    void centerOn(const QPointF& point);

    void update();

protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void exposeEvent(QExposeEvent *eevent);
    void resizeEvent(QResizeEvent * event);

    // Rotate about the object being viewed.
    void rotate(int deltax, int deltay, int rotation);

    void setZoom(qreal delta);

    const QTransform anchorTransform(GraphicItemGL::Anchor anchor) const;

    void ensureContext();

signals:


private:

    // openGL context variables
    QOpenGLContext *m_context = nullptr;
    QSurfaceFormat format;
    bool m_initialized;
    QRectF m_viewport;

    // list of nodes to be renderered in the context
    QList<GraphicItemGL *> m_nodes;

    // auxiliary variables for panning, zoom and selection
    QPoint m_lastpos = QPoint(-1, -1);
    bool m_panning = false;
    qreal m_zoom = 1.0f;
    qreal m_panx = 1.0f;
    qreal m_pany = 1.0f;
};

#endif // CELLGLVIEW_H

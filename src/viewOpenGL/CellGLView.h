/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QWindow>
#include "GraphicItemGL.h"
#include "SelectionEvent.h"

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

    void rotate(qreal angle);

    void update();

    void setViewPort(QRectF viewport);
    void setScene(QRectF scene);

protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void ensureContext();

    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void exposeEvent(QExposeEvent *eevent);
    void resizeEvent(QResizeEvent * event);

    void setZoom(qreal delta);

    const QTransform nodeTransformations(GraphicItemGL *node) const;
    const QTransform sceneTransformations() const;

signals:

    void signalViewPortUpdated(QRectF);
    void signalSceneUpdated(QRectF);

private:

    // helper debug function
    void drawRect(const QRectF &rect, QGLPainter *painter, QColor color = Qt::red);

    // openGL context variables
    QOpenGLContext *m_context = nullptr;
    QSurfaceFormat format;
    bool m_initialized = false;

    // scene and viewport aux variables
    QRectF m_viewport;
    QRectF m_scene;

    // list of nodes to be renderered in the view
    QList<GraphicItemGL *> m_nodes;

    // auxiliary variables for panning, zoom and selection
    QPoint m_originPanning = QPoint(-1, -1);
    QPoint m_originRubberBand = QPoint(-1, -1);
    bool m_panning = false;
    bool m_rubberBanding = false;
    QRectF m_rubberBandRect;
    qreal m_rotate = 0.0;
    qreal m_zoom = 1.0;
    qreal m_panx = 0.0;
    qreal m_pany = 0.0;
};

#endif // CELLGLVIEW_H

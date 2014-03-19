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
class QRubberBand;

/* CellGLView is a container
 * to render OpenGL GraphicItemGL type objects
 * QGLWidget is deprecated so Qt3D is the way to go
 * however, QGLView adds camera and things that we do not
 * need so since QGLView is a wraper around QWindow
 * we built our own customized wrapper
 */
class CellGLView : public QWindow
{
    Q_OBJECT

public:

    explicit CellGLView(QScreen *parent = 0);
    virtual ~CellGLView();

    void addRenderingNode(GraphicItemGL *node);
    void removeRenderingNode(GraphicItemGL *node);

    const QImage grabPixmapGL() const;

    void reset();

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

    void setZoomFactorAndUpdate(qreal zoom);

    const QTransform nodeTransformations(GraphicItemGL *node) const;
    const QTransform sceneTransformations() const;

signals:

    void signalViewPortUpdated(QRectF);
    void signalSceneUpdated(QRectF);

private:
    void setSceneFocusCenterPointWithClamping(const QPointF &center_point);
    qreal clampZoomFactorToAllowedRange(qreal zoom) const;
    qreal minZoom() const;
    qreal maxZoom() const;

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
    QRect m_rubberBandRect;
    qreal m_rotate = 0.0;
    QPointF m_scene_focus_center_point;
    // Just to avoid undefined behaviour if we would miss setting m_zoom_factor later
    // we set the value here. This value should never be used though.
    qreal m_zoom_factor = 1.0; 
};

#endif // CELLGLVIEW_H

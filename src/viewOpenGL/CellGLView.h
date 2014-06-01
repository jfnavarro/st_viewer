/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QWindow>
#include <QGLFramebufferObjectSurface>

#include "GraphicItemGL.h"
#include "SelectionEvent.h"

#include <functional>

class QGLPainter;
class GraphicItemGL;
class QSurfaceFormat;
class QVector3D;
class QOpenGLContext;
class QRubberBand;
class RubberbandGL;
class QGLFramebufferObject;

// CellGLView is a container
// to render OpenGL GraphicItemGL type objects
// QGLWidget is deprecated so Qt3D is the way to go
// however, QGLView adds camera and things that we do not
// need so since QGLView is a wraper around QWindow
// we built our own customized wrapper
class CellGLView : public QWindow
{
    Q_OBJECT

public:

    //used to filter nodes for mouse events
    typedef std::function<bool (const GraphicItemGL &)> FilterFunc;

    enum MouseEventType {
        moveType,
        pressType,
        releaseType
    };

    explicit CellGLView(QScreen *parent = 0);
    virtual ~CellGLView();

    //add/remove nodes from the rendering queue
    void addRenderingNode(GraphicItemGL *node);
    void removeRenderingNode(GraphicItemGL *node);

    //return a QImage representation of the canvas
    const QImage grabPixmapGL() const;

    //used for the Scroll Area container to adjust the scroll bars
    QRectF allowedCenterPoints() const;
    QPointF sceneFocusCenterPoint() const;

public slots:

    //some public slots to configure properties of the view
    void setSelectionMode(const bool selectionMode);
    void zoomOut();
    void zoomIn();
    void centerOn(const QPointF& point);
    void rotate(qreal angle);
    void update();
    void setViewPort(const QRectF viewport);
    void setScene(const QRectF scene);
    void setSceneFocusCenterPointWithClamping(const QPointF center_point);

protected:

    //OpenGL rendering and initialization functions
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void ensureContext();

    //overloaded key and mouse events for zooming/panning/selection/rendering
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void exposeEvent(QExposeEvent *eevent);
    void resizeEvent(QResizeEvent * event);

    void setZoomFactorAndUpdate(const qreal zoom);

    //returns the node local transformations in the view CS
    const QTransform nodeTransformations(GraphicItemGL *node) const;

signals:

    void signalViewPortUpdated(const QRectF);
    void signalSceneUpdated(const QRectF);
    void signalSceneTransformationsUpdated(const QTransform transform);

private:

    //helper functions used to compute center position/zoom/padding
    const QTransform sceneTransformations() const;
    void resizeFromGeometry();
    qreal clampZoomFactorToAllowedRange(qreal zoom) const;
    qreal minZoom() const;
    qreal maxZoom() const;

    // notify rubberbandable nodes with a rubberband event
    void sendRubberBandEventToNodes(const QRectF rubberBand, const QMouseEvent *event);

    // returns true if the event was sent to at least one of the nodes
    bool sendMouseEventToNodes(const QPoint point, const QMouseEvent *event,
				       const MouseEventType type, const FilterFunc filterFunc);

    // openGL context variables
    QScopedPointer<QOpenGLContext> m_context;
    QSurfaceFormat format;
    bool m_initialized;

    // scene and viewport aux variables
    QRectF m_viewport;
    QRectF m_scene;

    // list of nodes to be renderered in the view
    QList<GraphicItemGL *> m_nodes;

    // auxiliary variables for panning, zoom and selection
    QPoint m_originPanning;
    QPoint m_originRubberBand;
    bool m_panning;
    bool m_rubberBanding;
    bool m_selecting;
    QScopedPointer<RubberbandGL> m_rubberband;
    qreal m_rotate;
    QPointF m_scene_focus_center_point;
    // Just to avoid undefined behaviour if we would miss setting m_zoom_factor later
    // we set the value here. This value should never be used though.
    qreal m_zoom_factor;

    // scene viewport projection
    QMatrix4x4 m_projm;

    Q_DISABLE_COPY(CellGLView)
};

#endif // CELLGLVIEW_H

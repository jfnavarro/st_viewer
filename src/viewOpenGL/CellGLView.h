/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QOpenGLWindow>
#include <QPointer>
#include <QOpenGLFunctions>

#include "GraphicItemGL.h"
#include "SelectionEvent.h"

#include <functional>

class GraphicItemGL;
class QRubberBand;
class RubberbandGL;

// CellGLView is a container
// to render OpenGL GraphicItemGL type objects
// in combination with Qt3D QGLPainter.
// QOpenGLWindow is a wrapper around QWindow to
// allow OpenGL rendering.

class CellGLView : public QOpenGLWindow
{
    Q_OBJECT

public:

    enum MouseEventType {
        moveType,
        pressType,
        releaseType
    };

    explicit CellGLView(UpdateBehavior updateBehavior = NoPartialUpdate,
                        QWindow *parent = 0);
    virtual ~CellGLView();

    //add/remove nodes from the rendering queue
    void addRenderingNode(GraphicItemGL *node);
    void removeRenderingNode(GraphicItemGL *node);

    //return a QImage representation of the canvas
    const QImage grabPixmapGL();

    //used for the Scroll Area container to adjust the scroll bars
    QRectF allowedCenterPoints() const;
    QPointF sceneFocusCenterPoint() const;

    //clear all local variables and data
    void clearData();

public slots:

    //TODO slots should have the prefix "slot"

    //some public slots to configure properties of the view
    void zoomOut();
    void zoomIn();
    void centerOn(const QPointF& point);
    void rotate(qreal angle);

    //slot to enable the rubberband selection mode
    void setSelectionMode(const bool selectionMode);

    //slots to set the viewport and scene size and the set the focus in a point
    //very handy to make possible the interaction with the minimap
    void setViewPort(const QRectF viewport);
    void setScene(const QRectF scene);
    void setSceneFocusCenterPointWithClamping(const QPointF center_point);

protected:

    //OpenGL rendering and initialization functions
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    //overloaded key and mouse events for zooming/panning/selection
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    //returns the node local transformations in the view CS adjusted for anchor
    const QTransform nodeTransformations(GraphicItemGL *node) const;

signals:

    //signals to notify when the scene/view are changed/transformed
    //very handy for the minimap and the scrollarea
    void signalViewPortUpdated(const QRectF);
    void signalSceneUpdated(const QRectF);
    void signalSceneTransformationsUpdated(const QTransform transform);

private:

    //used to filter nodes for mouse events
    typedef std::function<bool (const GraphicItemGL &)> FilterFunc;

    //helper function to adjust the zoom level
    void setZoomFactorAndUpdate(const qreal zoom);

    //helper functions used to compute center position/zoom/padding
    const QTransform sceneTransformations() const;
    qreal clampZoomFactorToAllowedRange(const qreal zoom) const;
    qreal minZoom() const;
    qreal maxZoom() const;
    void setDefaultPanningAndZooming();

    // notify rubberbandable nodes with a rubberband event
    void sendRubberBandEventToNodes(const QRectF rubberBand,
                                    const QMouseEvent *event);

    // returns true if the event was sent to at least one of the nodes
    bool sendMouseEventToNodes(const QPoint point, const QMouseEvent *event,
				       const MouseEventType type, const FilterFunc filterFunc);

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
    QPointer<RubberbandGL> m_rubberband;
    qreal m_rotate;
    QPointF m_scene_focus_center_point;
    qreal m_zoom_factor;

    // scene viewport projection
    QMatrix4x4 m_projm;

    Q_DISABLE_COPY(CellGLView)
};

#endif // CELLGLVIEW_H

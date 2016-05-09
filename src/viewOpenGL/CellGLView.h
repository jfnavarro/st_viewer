#ifndef CELLGLVIEW_H
#define CELLGLVIEW_H

#include <QOpenGLWidget>
#include <QPointer>

#include "GraphicItemGL.h"
#include "SelectionEvent.h"

#include <functional>

class GraphicItemGL;
class QRubberBand;
class RubberbandGL;

// CellGLView is a container
// to render OpenGL GraphicItemGL type objects
// It allows for zooming, panning and selection.
// It is based on a QOpenGLWidget which is a OpenGL
// renderable qt widget.
// It also makes use of QOpenGLfunctions to
// assure cross-platform compatibility

// The main concept in this class is that the rendering
// canvas is based on the size of the cell tissue image
// and that is what is called "scene" here, the "viewport"
// would be the size of the widget so these two concepts
// are used to compute zooming, scroll-bars and so
// What it is shown in the canvas is the cell tissue image
// in its original resolution and size. Then the spots(genes)
// are shown on top of the image and therefore their coordinates
// (array coordinats) must be transformed to the image space.

class CellGLView : public QOpenGLWidget
{
    Q_OBJECT

public:
    enum MouseEventType { moveType, pressType, releaseType };

    explicit CellGLView(QWidget *parent = 0);
    virtual ~CellGLView();

    // add/remove nodes from the rendering queue
    void addRenderingNode(QSharedPointer<GraphicItemGL> node);
    void removeRenderingNode(QSharedPointer<GraphicItemGL> node);

    // return a QImage representation of the canvas
    const QImage grabPixmapGL();

    // used for the ScrollArea container to adjust the scroll bars
    QRectF allowedCenterPoints() const;
    QPointF sceneFocusCenterPoint() const;

    // clear all local variables and data
    void clearData();

    // we must keep these overrided functions public so they can
    // be accessed from the ScrollArea class which wraps around
    // this object to implement scroll bars
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    bool event(QEvent *e) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

public slots:

    // TODO slots should have the prefix "slot"

    // some public slots to configure properties of the view
    void zoomOut();
    void zoomIn();
    void centerOn(const QPointF &point);
    void rotate(float angle);

    // slot to enable the rubberband selection mode
    void setSelectionMode(const bool selectionMode);

    // slots to set the viewport and scene size and the set the focus in a point
    // very handy to make possible the interaction with the minimap
    void setViewPort(const QRectF &viewport);
    void setScene(const QRectF &scene);
    void setSceneFocusCenterPointWithClamping(const QPointF &center_point);

protected:
    // OpenGL rendering and initialization functions
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    // returns the node local transformations in the view CS adjusted for anchor
    const QTransform nodeTransformations(QSharedPointer<GraphicItemGL> node) const;

signals:

    // signals to notify when the scene/view are changed/transformed
    // very handy for the minimap and the scrollarea
    void signalViewPortUpdated(const QRectF);
    void signalSceneUpdated(const QRectF);
    void signalSceneTransformationsUpdated(const QTransform transform);

private:
    // used to filter nodes for mouse events
    typedef std::function<bool(const GraphicItemGL &)> FilterFunc;

    // helper function to adjust the zoom level
    void setZoomFactorAndUpdate(const float zoom);

    // helper functions used to compute center position/zoom/padding
    const QTransform sceneTransformations() const;
    float clampZoomFactorToAllowedRange(const float zoom) const;
    float minZoom() const;
    float maxZoom() const;
    // this function ensures that the whole image fits to the canvas
    void setDefaultPanningAndZooming();

    // notify rubberbandable nodes with a rubberband event
    void sendRubberBandEventToNodes(const QRectF &rubberBand, const QMouseEvent *event);

    // returns true if the event was sent to at least one of the nodes
    bool sendMouseEventToNodes(const QPoint &point,
                               const QMouseEvent *event,
                               const MouseEventType type,
                               const FilterFunc &filterFunc);

    // scene and viewport aux variables
    QRectF m_viewport;
    QRectF m_scene;

    // list of nodes to be renderered in the view
    QList<QSharedPointer<GraphicItemGL>> m_nodes;

    // auxiliary variables for panning, zoom and selection
    QPoint m_originPanning;
    QPoint m_originRubberBand;
    bool m_panning;
    bool m_rubberBanding;
    bool m_selecting;
    QPointer<RubberbandGL> m_rubberband;
    float m_rotate;
    QPointF m_scene_focus_center_point;
    float m_zoom_factor;

    // scene viewport projection
    QMatrix4x4 m_projm;

    // a cross platform wrapper around OpenGL functions
    GraphicItemGL::QOpenGLFunctionsVersion m_qopengl_functions;

    Q_DISABLE_COPY(CellGLView)
};

#endif // CELLGLVIEW_H

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GRAPHICSVIEWGL_H
#define GRAPHICSVIEWGL_H

#include <QDebug>
#include "utils/DebugHelper.h"
#include <QtWidgets/QGraphicsView>
#include <QWheelEvent>
#include <QtGui/QDropEvent>
#include <QMimeData>
#include "ViewItemGL.h"
#include "utils/Utils.h"

class ViewItemGL;
class QGLWidget;
class MiniMapGL;
class HeatMapLegendGL;
class QGLContext;

// Extension of the QT graphical view class. Provides additional functionality
// to initialize and finalize opengl related data.
//NOTE this needs to be abstracted (decoupled) so as to allow hardware
//     accelerated as well as software based rendering.
//
// Extends the standard graphical view with functionality to control zooming,
// as well as providing an easy way of drawing HUD elements.
class GraphicsViewGL : public QGraphicsView
{
    Q_OBJECT

public:

    explicit GraphicsViewGL(QWidget* parent = 0);
    virtual ~GraphicsViewGL();

    void initGL(QGraphicsScene*);
    void finalizeGL();

    const QImage grabPixmapGL();

    void addViewItem(ViewItemGL *viewItem);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void showEvent(QShowEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void drawBackground(QPainter* painter, const QRectF& rect);
    virtual void drawForeground(QPainter* painter, const QRectF& rect);

    void enableMinimap(bool enable);

    const QSizeF zoom() const;
    void setZoom(const QSizeF& zoom = Globals::DEFAULT_ZOOM);

public slots:
    //TODO: should only be one "generic" zoom function
    void zoomIn(qreal zoomFactor = Globals::DEFAULT_ZOOM_IN);
    void zoomOut(qreal zoomFactor = Globals::DEFAULT_ZOOM_OUT);
    void setBackground(const QColor& color);
    void updateSceneRect(const QRectF& rect);

private slots:
    void slotCenterOn(const QPointF& point);

private:
    void pressRubberBand(QMouseEvent* event);
    void moveRubberBand(QMouseEvent* event);
    void releaseRubberBand(QMouseEvent* event);

    const QTransform anchorTransform(ViewItemGL::Anchor anchor) const;
    void setTransformZoom(const QSizeF& zoom);

    MiniMapGL *m_minimap;
    QGLWidget *m_opengl_surface;

    // internal zoom
    QSizeF m_zoom;
    QSizeF m_zoom_min;
    QSizeF m_zoom_max;

    // internal rubber band
    QPoint m_mousePressViewPoint;
    bool m_rubberBanding;
    QRect m_rubberBandRect;

    // gui items
    typedef QList<ViewItemGL*> ViewItems;
    ViewItems m_viewItems;
};

#endif // GRAPHICSVIEWGL_H

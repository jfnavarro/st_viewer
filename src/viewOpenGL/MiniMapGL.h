/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef MINIMAPGL_H
#define MINIMAPGL_H

#include "GraphicItemGL.h"

class QGLPainter;
class QImage;
class QVector2DArray;
class QRectF;
class QColor;
class QEvent;
class QMouseEvent;

// MiniMap is an view port GUI item that visualizes the view ports current
// "image" in relation to the scene. Ie. it shows where in the scene the
// view port currently is.
class MiniMapGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit MiniMapGL(QObject* parent = 0);
    virtual ~MiniMapGL();

    const QColor& sceneColor() const;    
    const QColor& viewColor() const;

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    const QRectF boundingRect() const;

public slots:

    void setViewColor(const QColor& viewColor);
    void setSceneColor(const QColor& sceneColor);

    void setScene(const QRectF& scene);
    void setViewPort(const QRectF& view);

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    void setSelectionArea(const SelectionEvent *){};
    void clearSelection(){};

signals:

    void signalCenterOn(const QPointF& point);

private:

    // internal functions
    void drawBorderRect(const QRectF &rect, QColor color, QGLPainter *painter);
    const QPointF mapToScene(const QPointF& point) const;
    void updateTransform(const QRectF& scene);

    // mini versions
    QRectF m_scene;
    QRectF m_view;
    QColor m_sceneColor;
    QColor m_viewColor;

    // mouse events
    bool m_selecting = false;
};

#endif // MINIMAPGL_H //

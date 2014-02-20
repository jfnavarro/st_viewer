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

    void setScene(const QRectF& scene);
    void setViewPort(const QRectF& view);

    void setSceneColor(const QColor& sceneColor);
    const QColor& sceneColor() const;

    void setViewColor(const QColor& viewColor);
    const QColor& viewColor() const;

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    virtual bool mouseMoveEvent(QMouseEvent* event);
    virtual bool mousePressEvent(QMouseEvent* event);
    virtual bool mouseReleaseEvent(QMouseEvent* event);

    const QRectF boundingRect() const;

signals:

    void signalCenterOn(const QPointF& point);

private:

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

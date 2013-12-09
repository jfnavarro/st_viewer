/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef MINIMAPGL_H
#define MINIMAPGL_H

#include "data/GLElementData.h"
#include "data/GLElementRender.h"
#include "ViewItemGL.h"

// MiniMap is an view port GUI item that visualizes the view ports current
// "image" in relation to the scene. Ie. it shows where in the scene the
// view port currently is.
class MiniMapGL : public ViewItemGL
{
    Q_OBJECT
    Q_PROPERTY(QColor sceneColor READ getSceneColor WRITE setSceneColor)
    Q_PROPERTY(QColor viewColor READ getViewColor WRITE setViewColor)

public:

    explicit MiniMapGL(QObject* parent = 0);
    virtual ~MiniMapGL();

    void setBounds(const QRectF& bounds);
    void setScene(const QRectF& scene);
    void setView(const QRectF& view);

    virtual void render(QPainter* painter);

    virtual const QRectF boundingRect() const;
    virtual bool contains(const QPointF& point) const;

    virtual bool mouseMoveEvent(QMouseEvent* event);
    virtual bool mousePressEvent(QMouseEvent* event);
    virtual bool mouseReleaseEvent(QMouseEvent* event);

    inline void setSceneColor(const QColor& sceneColor) { m_sceneColor = sceneColor; }
    inline const QColor& getSceneColor() const { return m_sceneColor; }
    inline void setViewColor(const QColor& viewColor) { m_viewColor = viewColor; }
    inline const QColor& getViewColor() const { return m_viewColor; }

signals:

    void signalCenterOn(const QPointF& point);

private:

    static const QRectF DEFAULT_BOUNDS;

    void updateTransform(const QRectF& scene);
    const QPointF mapToScene(const QPointF& point) const;

    // minimap select
    bool m_selecting;

    QRectF m_bounds;
    QTransform m_transform;
    // mini versions
    QRectF m_scene;
    QRectF m_view;
    QColor m_sceneColor;
    QColor m_viewColor;

    // render data
    GL::GLElementData m_data;
    GL::GLElementRenderQueue m_queue;

    void rebuildMinimapData();
    void generateMinimapData();
};

#endif // MINIMAPGL_H //

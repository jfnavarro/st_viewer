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

    const QColor sceneColor() const;
    const QColor viewColor() const;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

public slots:

    //TODO slots should have the prefix "slot"

    void setViewColor(const QColor viewColor);
    void setSceneColor(const QColor sceneColor);

    void setScene(const QRectF scene);
    void setViewPort(const QRectF view);

    void setParentSceneTransformations(const QTransform transform);

signals:

    //to notify the view
    void signalCenterOn(const QPointF& point);

protected:

    void draw(QGLPainter *painter) override;
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;

private:

    // internal functions
    QTransform localTransform() const;
    void centerOnLocalPos(const QPointF &localPoint);

    // mini versions
    QRectF m_scene;
    QRectF m_viewPort;
    QColor m_sceneColor;
    QColor m_viewColor;
    QTransform m_parentSceneTransformations;

    // doing mouse events
    bool m_selecting;

    Q_DISABLE_COPY(MiniMapGL)
};

#endif // MINIMAPGL_H //

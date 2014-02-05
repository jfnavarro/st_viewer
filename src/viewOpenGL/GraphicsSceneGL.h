/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef GRAPHICSSCENEGL_H
#define GRAPHICSSCENEGL_H

#include <QGraphicsScene>

class SelectionEvent;

// Extension of the QT graphical scene object.
// Extends the basic functionality by providing a means of propagating
// selection events to the view items.
class GraphicsSceneGL : public QGraphicsScene
{

public:
    explicit GraphicsSceneGL(QObject* parent = 0);
    virtual ~GraphicsSceneGL();

    void setSelectionArea(const SelectionEvent *event);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    virtual void drawBackground(QPainter* painter, const QRectF& rect);
    virtual void drawForeground(QPainter* painter, const QRectF& rect);
};

#endif // GRAPHICSSCENEGL_H

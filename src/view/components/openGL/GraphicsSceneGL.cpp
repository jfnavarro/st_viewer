/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QGLWidget>
#include <QGraphicsObject>
#include <QMetaObject>

#include <qdebug.h>

#include "SelectionEvent.h"
Q_DECLARE_METATYPE(SelectionEvent)

#include "GraphicsSceneGL.h"

GraphicsSceneGL::GraphicsSceneGL(QObject* parent): QGraphicsScene(parent)
{

}

GraphicsSceneGL::~GraphicsSceneGL()
{

}

void GraphicsSceneGL::setSelectionArea(const SelectionEvent *event)
{
    QList<QGraphicsItem*> itemList = items();
    foreach(QGraphicsItem* item, itemList)
    {
        QGraphicsObject* obj = dynamic_cast<QGraphicsObject *>(item);
        if ((obj != 0) && (obj->metaObject()->indexOfMethod("setSelectionArea(const SelectionEvent*)") != -1))
        {
            const QPainterPath localPath = item->transform().inverted().map(event->path());
            SelectionEvent selectionEvent(localPath, event->mode());
            QMetaObject::invokeMethod(obj, "setSelectionArea", Q_ARG(const SelectionEvent*, &selectionEvent));
        }
    }
}

void GraphicsSceneGL::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
}

void GraphicsSceneGL::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsSceneGL::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsSceneGL::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
}

void GraphicsSceneGL::wheelEvent(QGraphicsSceneWheelEvent* event)
{
     QGraphicsScene::wheelEvent(event);
}

void GraphicsSceneGL::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsScene::drawBackground(painter, rect);
}

void GraphicsSceneGL::drawForeground(QPainter* painter, const QRectF& rect)
{
    QGraphicsScene::drawForeground(painter, rect);
}

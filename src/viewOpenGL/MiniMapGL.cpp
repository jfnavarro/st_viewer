/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "MiniMapGL.h"

#include <QVector2DArray>
#include <QGLPainter>
#include <QEvent>
#include <QMouseEvent>

#include "CellGLView.h"

static const QColor minimap_view_color = Qt::blue;
static const QColor minimap_scene_color = Qt::red;
static const qreal minimap_height = 100.0;
static const qreal minimap_width = 100.0;

MiniMapGL::MiniMapGL(QObject *parent)
    : GraphicItemGL(parent), 
      m_sceneColor(minimap_scene_color),
      m_viewColor(minimap_view_color)
{
    setVisualOption(GraphicItemGL::Transformable, false);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, true);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
setVisualOption(GraphicItemGL::RubberBandable, false);
}

MiniMapGL::~MiniMapGL()
{

}

void MiniMapGL::setScene(const QRectF scene)
{
    // early out
    if ( !scene.isValid() ) {
        return;
    }
    m_scene = scene;
}

void MiniMapGL::setViewPort(const QRectF view)
{
    // early out
    if ( !view.isValid() ) {
        return;
    }
    m_viewPort = view;
}

void MiniMapGL::setParentSceneTransformations(const QTransform transform)
{ 
    m_parentSceneTransformations = transform;
    emit updated();
}

void MiniMapGL::drawBorderRect(const QRectF &rect, QColor color, QGLPainter *painter)
{
    const QPointF stl = rect.topLeft();
    const QPointF str = rect.topRight();
    const QPointF sbr = rect.bottomRight();
    const QPointF sbl = rect.bottomLeft();
    QVector2DArray vertices;
    vertices.append(stl.x(), stl.y());
    vertices.append(str.x(), str.y());
    vertices.append(sbr.x(), sbr.y());
    vertices.append(sbl.x(), sbl.y());

    color.setAlphaF(0.2);
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(color);
    painter->setVertexAttribute(QGL::Position, vertices );
    painter->draw(QGL::TriangleFan, vertices.size());

    color.setAlphaF(0.8);
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(color);
    painter->setVertexAttribute(QGL::Position, vertices );
    painter->draw(QGL::LineLoop, vertices.size());
}

QTransform MiniMapGL::localTransform() const
{
    const QSizeF maxBoundingSize(minimap_height, minimap_width);
    const QSizeF sceneScaledSize = m_scene.size().scaled(maxBoundingSize, Qt::KeepAspectRatio );
    const qreal scaleFactor = sceneScaledSize.height() / m_scene.height();
    QTransform transform;
    transform.scale(scaleFactor, scaleFactor);
    const QPointF top_left = transform.mapRect(m_scene).topLeft();
    transform.translate(top_left.x(), top_left.y());
    return transform;
}

void MiniMapGL::draw(QGLPainter *painter)
{
    if (!m_viewPort.isValid() || !m_scene.isValid()) { 
        return;
    }
    const QRectF viewPortInSceneCoordinates = m_parentSceneTransformations.inverted().mapRect(m_viewPort);
    drawBorderRect(localTransform().mapRect(m_scene), m_sceneColor, painter);
    drawBorderRect(localTransform().mapRect(viewPortInSceneCoordinates), m_viewColor, painter);
}

void MiniMapGL::drawGeometry(QGLPainter *painter)
{
    Q_UNUSED(painter);
}

void MiniMapGL::setSceneColor(const QColor sceneColor)
{
    m_sceneColor = sceneColor;
    emit updated();
}

const QColor MiniMapGL::sceneColor() const
{
    return m_sceneColor;
}

void MiniMapGL::setViewColor(const QColor viewColor)
{
    m_viewColor = viewColor;
    emit updated();
}

const QColor MiniMapGL::viewColor() const
{
    return m_viewColor;
}

const QRectF MiniMapGL::boundingRect() const
{
    return localTransform().mapRect(m_scene);
}

void MiniMapGL::mouseMoveEvent(QMouseEvent* event)
{
    // set selecting to false if release event missed
    if ( !event->buttons().testFlag(Qt::LeftButton) ) {
        m_selecting = false;
    }
    // move
    if ( m_selecting ) {
        centerOnLocalPos(event->localPos());
    }
}

void MiniMapGL::mousePressEvent(QMouseEvent* event)
{
    // center if left button is pressed down
    if ( event->buttons().testFlag(Qt::LeftButton) ) {
        m_selecting = true;
        centerOnLocalPos(event->localPos());
    }
}

void MiniMapGL::mouseReleaseEvent(QMouseEvent* event)
{
    // set selecting to false if released
    if ( !event->buttons().testFlag(Qt::LeftButton) ) {
        m_selecting = false;
    }
}

void MiniMapGL::centerOnLocalPos(const QPointF &localPoint)
{
  //    localPoint
    const QPointF scenePoint = localTransform().inverted().map(boundingRect().bottomRight()-localPoint);
    emit signalCenterOn(scenePoint);
}

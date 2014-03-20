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

MiniMapGL::MiniMapGL(CellGLView *cell_gl_view)
    : GraphicItemGL(cell_gl_view), 
      m_cell_gl_view(cell_gl_view),
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
  const QRectF scene = m_cell_gl_view->scene();
  const QSizeF bounding_size(minimap_height, minimap_width);
  const QSizeF scene_scaled_size = scene.size().scaled(bounding_size, Qt::KeepAspectRatio );
  const qreal scale_factor = scene_scaled_size.height() / scene.height();
  QTransform transform;
  transform.scale(scale_factor, scale_factor);
  const QPointF top_left = transform.mapRect(scene).topLeft();
  transform.translate(top_left.x(), top_left.y());
  return transform;
}

void MiniMapGL::draw(QGLPainter *painter)
{
    const QRectF scene = m_cell_gl_view->scene();
    const QRectF viewPort = m_cell_gl_view->viewPort();
    const QRectF viewPort_in_scene_coordinates = m_cell_gl_view->sceneTransformations().inverted().mapRect(viewPort);
 
    if (scene.isValid() && viewPort.isValid()) {
        drawBorderRect(localTransform().mapRect(scene), m_sceneColor, painter);
        drawBorderRect(localTransform().mapRect(viewPort_in_scene_coordinates), m_viewColor, painter);
    }
}

void MiniMapGL::drawGeometry(QGLPainter *painter)
{
    Q_UNUSED(painter);
}

void MiniMapGL::setSceneColor(const QColor& sceneColor)
{
    m_sceneColor = sceneColor;
    emit updated();
}

const QColor& MiniMapGL::sceneColor() const
{
    return m_sceneColor;
}

void MiniMapGL::setViewColor(const QColor& viewColor)
{
    m_viewColor = viewColor;
    emit updated();
}

const QColor& MiniMapGL::viewColor() const
{
    return m_viewColor;
}

const QRectF MiniMapGL::boundingRect() const
{
    return localTransform().mapRect(m_cell_gl_view->scene());
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
    const QPointF scenePoint = localTransform().inverted().map(localPoint);
    emit signalCenterOn(scenePoint);
}

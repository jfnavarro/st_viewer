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

static const QColor minimap_view_color = Qt::blue;
static const QColor minimap_scene_color = Qt::red;
static const qreal minimap_height = 100.0;
static const qreal minimap_width = 100.0;

MiniMapGL::MiniMapGL(QObject* parent)
    : GraphicItemGL(parent),
      m_scene(0.0, 0.0, minimap_height, minimap_width),
      m_view(0.0, 0.0, minimap_height, minimap_width),
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

    QRectF m_bounds(0.0, 0.0, minimap_height, minimap_width);
    QRectF scaled = QRectF(m_bounds.topLeft(),
                           scene.size().scaled(m_bounds.size(), Qt::KeepAspectRatio));

    if ( m_scene != scaled ) {
        m_scene = scaled;
        updateTransform(scene);
        //emit updated();
    }
}

void MiniMapGL::setViewPort(const QRectF view)
{
    // early out
    if ( !view.isValid() ) {
        return;
    }

    const QRectF transformed = m_transform.mapRect(view);
    if ( m_view != transformed ) {
        m_view = transformed;
        //emit updated();
    }
}

void MiniMapGL::updateTransform(const QRectF scene)
{
    // early out
    if ( !m_scene.isValid() || !scene.isValid() ) {
        // set to identity matrix
        m_transform = QTransform();
        return;
    }

    const QPointF s1 = QPointF(scene.width(), scene.height());
    const QPointF s2 = QPointF(m_scene.width(), m_scene.height());

    const qreal s11 = (s2.x() / s1.x());
    const qreal s22 = (s2.y() / s1.y());

    m_transform =
        QTransform::fromTranslate(-scene.x(), -scene.y()) // align
        * QTransform(s11, 0.0, 0.0, s22, 0.0, 0.0);   // scale
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

void MiniMapGL::draw(QGLPainter *painter)
{
    // draw scene rectangle
    if (m_scene.isValid()) {
        drawBorderRect(m_scene, m_sceneColor, painter);
    }
    // draw view rectangle
    if (m_view.isValid()) {
        drawBorderRect(m_view, m_viewColor, painter);
    }
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
    return m_scene;
}

void MiniMapGL::mouseMoveEvent(QMouseEvent* event)
{
    // set selecting to false if release event missed
    if ( !event->buttons().testFlag(Qt::LeftButton) ) {
        m_selecting = false;
    }
    // move
    if ( m_selecting ) {
        const QPointF localPoint = event->localPos();
        const QPointF scenePoint = mapToScene(localPoint);
        emit signalCenterOn(scenePoint);
    }
}

void MiniMapGL::mousePressEvent(QMouseEvent* event)
{
    // center if left button is pressed down
    if ( event->buttons().testFlag(Qt::LeftButton) ) {
        m_selecting = true;
        const QPointF localPoint = event->localPos();
        const QPointF scenePoint = mapToScene(localPoint);
        emit signalCenterOn(scenePoint);
    }
}

void MiniMapGL::mouseReleaseEvent(QMouseEvent* event)
{
    // set selecting to false if released
    if ( !event->buttons().testFlag(Qt::LeftButton) ) {
        m_selecting = false;
    }
}

const QPointF MiniMapGL::mapToScene(const QPointF& point) const
{
    return m_transform.inverted().map(point);
}

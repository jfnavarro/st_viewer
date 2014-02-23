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
static const qreal minimap_height = 100.0f;
static const qreal minimap_width = 100.0f;

MiniMapGL::MiniMapGL(QObject* parent)
    : GraphicItemGL(parent),
      m_scene(0.0f, 0.0f, minimap_height, minimap_width),
      m_view(0.0f, 0.0f, minimap_height, minimap_width),
      m_sceneColor(minimap_scene_color),
      m_viewColor(minimap_view_color)
{
    setVisualOption(GraphicItemGL::Transformable, false);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, true);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
}

MiniMapGL::~MiniMapGL()
{

}

void MiniMapGL::setScene(const QRectF& scene)
{
    // early out
    if ( !scene.isValid() ) {
        return;
    }

    QRectF m_bounds(0.0f, 0.0f, minimap_height, minimap_width);
    QRectF scaled = QRectF(m_bounds.topLeft(),
                           scene.size().scaled(m_bounds.size(), Qt::KeepAspectRatio));

    if (m_scene != scaled ) {
        m_scene = scaled;
        emit updated();
    }
}

void MiniMapGL::setViewPort(const QRectF& view)
{
    // early out
    if ( !view.isValid() ) {
        return;
    }

    const QRectF transformedView = m_transform.mapRect(view);
    if ( m_view != transformedView ) {
        m_view = transformedView;
        emit updated();
    }
}

void MiniMapGL::updateTransform(const QRectF& scene)
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
        QTransform::fromTranslate(-scene.x(), -scene.y())
        * QTransform(s11, 0.0, 0.0, s22, 0.0, 0.0);

}

void MiniMapGL::draw(QGLPainter *painter)
{
    // draw scene rectangle
    if (m_scene.isValid()) {

        const QPointF stl = m_scene.topLeft();
        const QPointF str = m_scene.topRight();
        const QPointF sbr = m_scene.bottomRight();
        const QPointF sbl = m_scene.bottomLeft();

        m_sceneColor.setAlphaF(0.2);

        QVector2DArray scene_vertices;
        scene_vertices.append(stl.x(), stl.y());
        scene_vertices.append(str.x(), str.y());
        scene_vertices.append(sbr.x(), sbr.y());
        scene_vertices.append(sbl.x(), sbl.y());

        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(m_sceneColor);
        painter->setVertexAttribute(QGL::Position, scene_vertices );
        painter->draw(QGL::TriangleFan, scene_vertices.size());

        m_sceneColor.setAlphaF(0.8);
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(m_sceneColor);
        painter->setVertexAttribute(QGL::Position, scene_vertices );
        painter->draw(QGL::LineLoop, scene_vertices.size());
    }

    // draw view rectangle
    if (m_view.isValid()) {

        const QPointF vtl = m_view.topLeft();
        const QPointF vtr = m_view.topRight();
        const QPointF vbr = m_view.bottomRight();
        const QPointF vbl = m_view.bottomLeft();

        QVector2DArray view_vertices;
        view_vertices.append(vtl.x(), vtl.y());
        view_vertices.append(vtr.x(), vtr.y());
        view_vertices.append(vbr.x(), vbr.y());
        view_vertices.append(vbl.x(), vbl.y());

        m_viewColor.setAlphaF(0.2);
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(m_viewColor);
        painter->setVertexAttribute(QGL::Position, view_vertices );
        painter->draw(QGL::TriangleFan, view_vertices.size());

        m_viewColor.setAlphaF(0.8);
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(m_sceneColor);
        painter->setVertexAttribute(QGL::Position, view_vertices );
        painter->draw(QGL::LineLoop, view_vertices.size());
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

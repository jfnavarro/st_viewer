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
    : QGLSceneNode(parent),
      m_scene(0.0f,0.0f, minimap_height / 2, minimap_width / 2),
      m_view(0.0f,0.0f, minimap_height, minimap_width),
      m_sceneColor(minimap_scene_color),
      m_viewColor(minimap_view_color),
      m_visible(false)
{

}

MiniMapGL::~MiniMapGL()
{

}

void MiniMapGL::setScene(const QRectF& scene)
{
    QRectF m_bounds(0.0f, 0.0f, minimap_height, minimap_width);
    QRectF scaled = QRectF(m_bounds.topLeft(),
                           scene.size().scaled(m_bounds.size(), Qt::KeepAspectRatio));

    if (m_scene != scaled) {
        m_scene = scaled;
        updateTransform(scene);
        emit updated();
    }
}

void MiniMapGL::setView(const QRectF& view)
{
    //const QRectF transformedView = m_transform.mapRect(view);
    const QRectF transformedView = view;
    if (m_view != transformedView) {
        m_view = transformedView;
        emit updated();
    }
}

void MiniMapGL::updateTransform(const QRectF& scene)
{
    const QPointF s1 = QPointF(scene.width(), scene.height());
    const QPointF s2 = QPointF(m_scene.width(), m_scene.height());
    const qreal s11 = (s2.x() / s1.x());
    const qreal s22 = (s2.y() / s1.y());
    setLocalTransform(
        QTransform::fromTranslate(-scene.x(), -scene.y())
        * QTransform(s11, 0.0, 0.0, s22, 0.0, 0.0) );

}

void MiniMapGL::draw(QGLPainter *painter)
{
    if (!m_visible) {
        return;
    }

    glEnable(GL_BLEND);
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
            painter->setColor(m_sceneColor);
            painter->setVertexAttribute(QGL::Position, scene_vertices );
            painter->draw(QGL::TriangleFan, scene_vertices.size());

            m_sceneColor.setAlphaF(0.8);
            painter->clearAttributes();
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

            m_viewColor.setAlphaF(0.2);

            QVector2DArray view_vertices;
            view_vertices.append(vtl.x(), vtl.y());
            view_vertices.append(vtr.x(), vtr.y());
            view_vertices.append(vbr.x(), vbr.y());
            view_vertices.append(vbl.x(), vbl.y());

            painter->clearAttributes();
            painter->setColor(m_viewColor);
            painter->setVertexAttribute(QGL::Position, view_vertices );
            painter->draw(QGL::TriangleFan, view_vertices.size());

            m_viewColor.setAlphaF(0.8);
            painter->clearAttributes();
            painter->setColor(m_sceneColor);
            painter->setVertexAttribute(QGL::Position, view_vertices );
            painter->draw(QGL::LineLoop, view_vertices.size());
        }
    }
    glDisable(GL_BLEND);
}

bool MiniMapGL::event(QEvent *e)
{
    qDebug() << "Clicked";
    // Convert the raw event into a signal representing the user's action.
    QMouseEvent *me = dynamic_cast<QMouseEvent*>(e);
    if (e->type() == QEvent::MouseButtonPress) {
        if (me->button() == Qt::LeftButton) {
            emit pressed();
        }
    } else if (e->type() == QEvent::MouseButtonRelease) {
        if (me->button() == Qt::LeftButton) {
            emit released();
            if (me->x() >= 0)  {  // Positive: inside object, Negative: outside.
                emit clicked();
            }
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    }
    return QGLSceneNode::event(e);
}

void MiniMapGL::drawGeometry(QGLPainter *painter)
{
    QGLSceneNode::drawGeometry(painter);
}


void MiniMapGL::setSceneColor(const QColor& sceneColor)
{
    m_sceneColor = sceneColor;
}

const QColor& MiniMapGL::getSceneColor() const
{
    return m_sceneColor;
}

void MiniMapGL::setViewColor(const QColor& viewColor)
{
    m_viewColor = viewColor;
}

const QColor& MiniMapGL::getViewColor() const
{
    return m_viewColor;
}

void MiniMapGL::setVisible(bool visible)
{
    m_visible = visible;
    emit updated();
}

bool MiniMapGL::visible() const
{
    return m_visible;
}
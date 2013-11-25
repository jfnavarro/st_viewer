/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "utils/Utils.h"

#include <QPainter>

#include <GLQt.h>
#include <GLScope.h>
#include <data/GLElementRender.h>
#include <data/GLElementShapeFactory.h>
#include <math/GLMatrix.h>

#include "view/pages/CellViewPage.h"

#include "MiniMapGL.h"

const QRectF MiniMapGL::DEFAULT_BOUNDS = QRectF(0.0f, 0.0f,
        Globals::minimap_height,
        Globals::minimap_width);

MiniMapGL::MiniMapGL(QObject* parent)
    : ViewItemGL(parent), m_selecting(false), m_bounds(DEFAULT_BOUNDS),
      m_transform(), m_scene(), m_view()
{
    m_sceneColor = Globals::minimap_scene_color;
    m_viewColor = Globals::minimap_view_color;
}

MiniMapGL::~MiniMapGL()
{

}

void MiniMapGL::setBounds(const QRectF& bounds)
{
    if (m_bounds != bounds) {
        m_bounds = bounds;
    }
}

void MiniMapGL::setScene(const QRectF& scene)
{
    // early out
    if (!scene.isValid()) {
        return;
    }
    QRectF scaled = QRectF(m_bounds.topLeft(), scene.size().scaled(m_bounds.size(), Qt::KeepAspectRatio));
    if (m_scene != scaled) {
        m_scene = scaled;
        updateTransform(scene);
        rebuildMinimapData();
    }
}

void MiniMapGL::setView(const QRectF& view)
{
    // early out
    if (!view.isValid()) {
        return;
    }
    const QRectF transformedView = m_transform.mapRect(view);
    if (m_view != transformedView) {
        m_view = transformedView;
        rebuildMinimapData();
    }
}

void MiniMapGL::updateTransform(const QRectF& scene)
{
    // early out
    if (!m_bounds.isValid() || !m_scene.isValid() || !scene.isValid()) {
        // set to identity matrix
        m_transform = QTransform();
        return;
    }
    const QPointF s1 = QPointF(scene.width(),   scene.height());
    const QPointF s2 = QPointF(m_scene.width(), m_scene.height());
    const qreal s11 = (s2.x() / s1.x());
    const qreal s22 = (s2.y() / s1.y());
    m_transform =
        QTransform::fromTranslate(-scene.x(), -scene.y()) // align
        * QTransform(s11, 0.0f, 0.0f, s22, 0.0f, 0.0f);   // scale
}

void MiniMapGL::render(QPainter* painter)
{
    GL::GLElementRender renderer;

    painter->beginNativePainting();
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        GL::GLscope glBlendScope(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderer.render(m_data);
        glPopMatrix();
    }
    painter->endNativePainting();
}

const QRectF MiniMapGL::boundingRect() const
{
    return m_scene;
}

const bool MiniMapGL::contains(const QPointF& point) const
{
    return m_scene.contains(point);
}

const bool MiniMapGL::mouseMoveEvent(QMouseEvent* event)
{
    // set selecting to false if release event missed
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_selecting = false;
    }

    // move
    if (m_selecting) {
        const QPointF localPoint = event->localPos();
        const QPointF scenePoint = mapToScene(localPoint);
        emit signalCenterOn(scenePoint);
        return true;
    }

    return false;
}
const bool MiniMapGL::mousePressEvent(QMouseEvent* event)
{
    // center if left button is pressed down
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_selecting = true;
        const QPointF localPoint = event->localPos();
        const QPointF scenePoint = mapToScene(localPoint);
        emit signalCenterOn(scenePoint);
        return true;
    }
    return false;
}
const bool MiniMapGL::mouseReleaseEvent(QMouseEvent* event)
{
    // set selecting to false if released
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_selecting = false;
    }
    // always propagate release events
    return false;
}

const QPointF MiniMapGL::mapToScene(const QPointF& point) const
{
    return m_transform.inverted().map(point);
}

void MiniMapGL::rebuildMinimapData()
{
    // clear rendering data and generate anew
    m_data.clear();
    generateMinimapData();
}

void MiniMapGL::generateMinimapData()
{
    const GL::GLflag flags = GL::GLElementShapeFactory::AutoAddColor | GL::GLElementShapeFactory::AutoAddConnection;
    GL::GLElementRectangleFactory factory(m_data, flags);

    // draw scene rectangle
    if (m_scene.isValid()) {
        const GL::GLpoint stl = GL::toGLpoint(m_scene.topLeft());
        const GL::GLpoint str = GL::toGLpoint(m_scene.topRight());
        const GL::GLpoint sbr = GL::toGLpoint(m_scene.bottomRight());
        const GL::GLpoint sbl = GL::toGLpoint(m_scene.bottomLeft());
        const GL::GLcolor sceneColor = GL::toGLcolor(m_sceneColor);
        factory.setColor(0.2f * sceneColor);
        factory.addShape(GL::GLrectangle::fromCorners(stl, sbr));
        factory.setColor(0.8f * sceneColor);
        factory.addShape(GL::GLrectangle::fromLine(stl, str, 1.0f));
        factory.addShape(GL::GLrectangle::fromLine(str, sbr, 1.0f));
        factory.addShape(GL::GLrectangle::fromLine(sbr, sbl, 1.0f));
        factory.addShape(GL::GLrectangle::fromLine(sbl, stl, 1.0f));
    }

    // draw view rectangle
    if (m_view.isValid()) {
        const GL::GLpoint vtl = GL::toGLpoint(m_view.topLeft());
        const GL::GLpoint vtr = GL::toGLpoint(m_view.topRight());
        const GL::GLpoint vbr = GL::toGLpoint(m_view.bottomRight());
        const GL::GLpoint vbl = GL::toGLpoint(m_view.bottomLeft());
        const GL::GLcolor viewColor = GL::toGLcolor(m_viewColor);
        factory.setColor(0.2f * viewColor);
        factory.addShape(GL::GLrectangle::fromCorners(vtl, vbr));
        factory.setColor(0.8f * viewColor);
        factory.addShape(GL::GLrectangle::fromLine(vtl, vtr, 1.0f));
        factory.addShape(GL::GLrectangle::fromLine(vtr, vbr, 1.0f));
        factory.addShape(GL::GLrectangle::fromLine(vbr, vbl, 1.0f));
        factory.addShape(GL::GLrectangle::fromLine(vbl, vtl, 1.0f));
    }
}

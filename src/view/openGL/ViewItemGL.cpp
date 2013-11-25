/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ViewItemGL.h"

ViewItemGL::ViewItemGL(QObject* parent)
    : QObject(parent), m_transform(), m_z(0), m_visible(true), m_anchor(NorthWest)
{

}

ViewItemGL::~ViewItemGL()
{

}

const bool ViewItemGL::contains(const QPointF& point) const
{
    return this->boundingRect().contains(point);
}

const bool ViewItemGL::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    return false;
}
const bool ViewItemGL::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    return false;
}
const bool ViewItemGL::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    return false;
}

const ViewItemGL::Anchor ViewItemGL::anchor() const
{
    return m_anchor;
}

void ViewItemGL::setAnchor(ViewItemGL::Anchor anchor)
{
    m_anchor = anchor;
}

const QTransform ViewItemGL::transform() const
{
    return adjustForAnchor(m_transform);
}

void ViewItemGL::setTransform(const QTransform& transform)
{
    m_transform = transform;
}

int ViewItemGL::zValue() const
{
    return m_z;
}

void ViewItemGL::setZValue(int z)
{
    m_z = z;
}

bool ViewItemGL::visible() const
{
    return m_visible;
}

void ViewItemGL::setVisible(bool visible)
{
    m_visible = visible;
}

const QTransform ViewItemGL::adjustForAnchor(const QTransform& transform) const
{
    const QRectF rect = this->boundingRect();

    QTransform adjustedTransform = transform;
    switch (m_anchor) {
    case ViewItemGL::Center:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5f, (rect.y() + rect.height()) * -0.5f);
        break;
    case ViewItemGL::North:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5f, 0.0f);
        break;
    case ViewItemGL::NorthEast:
        adjustedTransform.translate((rect.x() + rect.width()) * -1.0f, 0.0f);
        break;
    case ViewItemGL::East:
        adjustedTransform.translate((rect.x() + rect.width()) * -1.0f, (rect.y() + rect.height()) * -0.5f);
        break;
    case ViewItemGL::SouthEast:
        adjustedTransform.translate((rect.x() + rect.width()) * -1.0f, (rect.y() + rect.height()) * -1.0f);
        break;
    case ViewItemGL::South:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5f, (rect.y() + rect.height()) * -1.0f);
        break;
    case ViewItemGL::SouthWest:
        adjustedTransform.translate(0.0f, (rect.y() + rect.height()) * -1.0f);
        break;
    case ViewItemGL::West:
        adjustedTransform.translate(0.0f, (rect.y() + rect.height()) * -0.5f);
        break;
    case ViewItemGL::NorthWest:
    // fall-through
    default:
        break;
    }
    return adjustedTransform;
}


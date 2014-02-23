/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneData.h"

#include <QVector3D>
#include <qglnamespace.h>
#include <QColor4ub>
#include <QGLAttributeValue>
#include <QCustomDataArray>

static const QGL::VertexAttribute selectionVertex = QGL::CustomVertex1;
static const QGL::VertexAttribute visibleVertex = QGL::CustomVertex0;

GeneData::GeneData()
{
}

GeneData::~GeneData()
{

}

void GeneData::clear()
{
    QGeometryData::clear();
    m_values.clear();
    m_refCount.clear();
}

int GeneData::addQuad(qreal x, qreal y, qreal size, QColor4ub color)
{
    static const QVector2D ta(0.0f, 0.0f);
    static const QVector2D tb(0.0f, 1.0f);
    static const QVector2D tc(1.0f, 1.0f);
    static const QVector2D td(1.0f, 0.0f);

    const int index_count = count();

    appendVertex( QVector3D(x - size / 2.0f , y - size / 2.0f, 0.0f) );
    appendVertex( QVector3D(x + size / 2.0f, y - size / 2.0f, 0.0f) );
    appendVertex( QVector3D(x + size / 2.0f, y + size / 2.0f, 0.0f) );
    appendVertex( QVector3D(x - size / 2.0f, y + size / 2.0f, 0.0f) );
    appendTexCoord(ta, tb, tc, td, QGL::TextureCoord0);
    appendColor(color, color, color, color);
    appendIndices(index_count, index_count + 1, index_count + 2);
    appendIndices(index_count, index_count + 2, index_count + 3);

    // update custom vertex arrays
    m_values.append(0.0f);
    m_values.append(0.0f);
    m_values.append(0.0f);
    m_values.append(0.0f);

    m_refCount.append(0.0f);
    m_refCount.append(0.0f);
    m_refCount.append(0.0f);
    m_refCount.append(0.0f);

    appendAttribute(0.0f, selectionVertex);
    appendAttribute(0.0f, selectionVertex);
    appendAttribute(0.0f, selectionVertex);
    appendAttribute(0.0f, selectionVertex);

    appendAttribute(0.0f, visibleVertex);
    appendAttribute(0.0f, visibleVertex);
    appendAttribute(0.0f, visibleVertex);
    appendAttribute(0.0f, visibleVertex);

    // return first index of the quad created
    return index_count;
}

void GeneData::updateQuadSize(const int index, qreal x, qreal y, qreal size)
{
    vertex(index) = QVector3D(x - size / 2.0f , y - size / 2.0f, 0.0f);
    vertex(index + 1) = QVector3D(x + size / 2.0f, y - size / 2.0f, 0.0f);
    vertex(index + 2) = QVector3D(x + size / 2.0f, y + size / 2.0f, 0.0f);
    vertex(index + 3) = QVector3D(x - size / 2.0f, y + size / 2.0f, 0.0f);
}

void GeneData::updateQuadColor(const int index, QColor4ub newcolor)
{
    color(index) = newcolor;
    color(index + 1) = newcolor;
    color(index + 2) = newcolor;
    color(index + 3) = newcolor;
}

void GeneData::updateQuadVisible(const int index, bool visible)
{
    floatAttribute(index, visibleVertex) = float(visible);
    floatAttribute(index + 1, visibleVertex) = float(visible);
    floatAttribute(index + 2, visibleVertex) = float(visible);
    floatAttribute(index + 3, visibleVertex) = float(visible);
}

void GeneData::updateQuadSelected(const int index, bool selected)
{
    floatAttribute(index, selectionVertex) = float(selected);
    floatAttribute(index + 1, selectionVertex) = float(selected);
    floatAttribute(index + 2, selectionVertex) = float(selected);
    floatAttribute(index + 3, selectionVertex) = float(selected);
}

void GeneData::updateQuadRefCount(const int index, float refcount)
{
    m_refCount.setAt(index, refcount);
    m_refCount.setAt(index + 1, refcount);
    m_refCount.setAt(index + 2, refcount);
    m_refCount.setAt(index + 3, refcount);
}

void GeneData::updateQuadValue(const int index, float value)
{
    m_values.setAt(index, value);
    m_values.setAt(index + 1, value);
    m_values.setAt(index + 2, value);
    m_values.setAt(index + 3, value);
}

QColor4ub GeneData::quadColor(const int index) const
{
    return colorAt(index);
}

bool GeneData::quadVisible(const int index) const
{
    // all vertices has same value
    return floatAttributeAt(index, visibleVertex) == 1.0f;
}

bool GeneData::quadSelected(const int index) const
{
    // all vertices has same value
    return floatAttributeAt(index, selectionVertex) == 1.0f;
}

float GeneData::quadRefCount(const int index) const
{
    // all vertices has same value
    return m_refCount.floatAt(index);
}

float GeneData::quadValue(const int index) const
{
    // all vertices has same value
    return m_values.floatAt(index);
}

void GeneData::resetRefCount()
{
    for(int i = 0; i < m_refCount.count(); ++i) {
        m_refCount.setAt(i, 0.0f);
    }
}

void GeneData::resetValues()
{
    for(int i = 0; i < m_values.count(); ++i) {
        m_values.setAt(i, 0.0f);
    }
}

const QGLAttributeValue GeneData::refCount() const
{
    //TOFIX possible expensive
    return QGLAttributeValue(m_refCount);
}

const QGLAttributeValue GeneData::values() const
{
    //TOFIX possible expensive
    return QGLAttributeValue(m_values);
}

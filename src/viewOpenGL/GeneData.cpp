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
#include <QGLIndexBuffer>

static const QGL::VertexAttribute selectionVertex = QGL::CustomVertex0;
static const QGL::VertexAttribute valuesVertex = QGL::CustomVertex1;

static const int QUAD_SIZE = 4;

GeneData::GeneData()
{
}

GeneData::~GeneData()
{

}

void GeneData::clearData()
{
    clear();
    m_refCount.clear();
}

int GeneData::addQuad(qreal x, qreal y, qreal size, QColor4ub color)
{
    static const QVector2D ta(0.0, 0.0);
    static const QVector2D tb(0.0, 1.0);
    static const QVector2D tc(1.0, 1.0);
    static const QVector2D td(1.0, 0.0);

    const int index_count = count();

    appendVertex( QVector3D(x - size / 2.0 , y - size / 2.0, 0.0) );
    appendVertex( QVector3D(x + size / 2.0, y - size / 2.0, 0.0) );
    appendVertex( QVector3D(x + size / 2.0, y + size / 2.0, 0.0) );
    appendVertex( QVector3D(x - size / 2.0, y + size / 2.0, 0.0) );

    appendTexCoord(ta, tb, tc, td, QGL::TextureCoord0);

    appendColor(color, color, color, color);

    appendIndices(index_count, index_count + 1, index_count + 2);
    appendIndices(index_count, index_count + 2, index_count + 3);

    // update custom vertex arrays
    for(int i = 0; i < QUAD_SIZE; i++) {
        appendAttribute(0.0, valuesVertex);
        appendAttribute(0.0, selectionVertex);
        m_refCount.append(0);
    }

    // return first index of the quad created
    return index_count;
}

void GeneData::updateQuadSize(const int index, qreal x, qreal y, qreal size)
{
    vertex(index) = QVector3D(x - size / 2.0 , y - size / 2.0, 0.0);
    vertex(index + 1) = QVector3D(x + size / 2.0, y - size / 2.0, 0.0);
    vertex(index + 2) = QVector3D(x + size / 2.0, y + size / 2.0, 0.0);
    vertex(index + 3) = QVector3D(x - size / 2.0, y + size / 2.0, 0.0);
}

void GeneData::updateQuadColor(const int index, QColor4ub newcolor)
{
    for(int i = 0; i < QUAD_SIZE; i++) {
        color(index + i) = newcolor;
    }
}

void GeneData::updateQuadSelected(const int index, bool selected)
{
    for(int i = 0; i < QUAD_SIZE; i++) {
        floatAttribute(index  + i, selectionVertex) = selected ? 1.0 : 0.0;
    }
}

void GeneData::updateQuadRefCount(const int index, int refcount)
{
    for(int i = 0; i < QUAD_SIZE; i++) {
        m_refCount[index + i] = refcount;
    }
}

void GeneData::updateQuadValue(const int index, int value)
{
    for(int i = 0; i < QUAD_SIZE; i++) {
        floatAttribute(index + i , valuesVertex) = static_cast<float>(value);
    }
}

QColor4ub GeneData::quadColor(const int index) const
{
    return colorAt(index);
}

bool GeneData::quadSelected(const int index) const
{
    // all vertices has same value
    return floatAttributeAt(index, selectionVertex) == 1.0;
}

int GeneData::quadRefCount(const int index) const
{
    // all vertices has same value
    return m_refCount.at(index);
}

int GeneData::quadValue(const int index) const
{
    // all vertices has same value
    return static_cast<int>(floatAttributeAt(index, valuesVertex));
}

void GeneData::resetRefCount()
{
    std::fill(m_refCount.begin(), m_refCount.end(), 0.0);
}

void GeneData::resetValues()
{
    for(int i = 0; i < attributes(valuesVertex).count(); ++i) {
        floatAttribute(i, valuesVertex) = 0.0;
    }
}

void GeneData::resetSelection(bool selected)
{
    const float reset_value =  selected ? 1.0 : 0.0;
    for(int i = 0; i < attributes(selectionVertex).count(); ++i) {
        floatAttribute(i, selectionVertex) = reset_value;
    }
}

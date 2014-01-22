/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementData.h"

namespace GL
{

GLElementData::GLElementData() : m_mode(GL_LINES)
{

}

GLElementData::~GLElementData()
{

}

void GLElementData::clear(GLflag flags)
{
    if (flags & GLElementData::PointArray) {
        m_points.resize(0);
    }
    if (flags & GLElementData::ColorArray) {
        m_colors.resize(0);
    }
    if (flags & GLElementData::IndexArray) {
        m_indices.resize(0);
    }
    if (flags & GLElementData::TextureArray) {
        m_textures.resize(0);
    }
    if (flags & GLElementData::RenderMode) {
        m_mode = GL_TRIANGLES;
    }
}

void GLElementData::addPoint(const qreal &x, const qreal &y)
{
    addPoint(QPointF(x, y));
}

void GLElementData::addPoint(const QPointF &point)
{
    m_points.push_back(QVector2D(point));
}

GLindex GLElementData::addShape(const QLineF &line)
{
    addPoint(line.p1());
    addPoint(line.p2());

    return m_points.size();
}

GLindex GLElementData::addShape(const QRectF &rectangle)
{
    QPointF p0 = rectangle.topLeft();
    QPointF p1 = rectangle.topRight();
    QPointF p2 = rectangle.bottomLeft();
    QPointF p3 = rectangle.bottomRight();

    addPoint(p0);
    addPoint(p1);
    addPoint(p2);
    addPoint(p3);

    return m_points.size();
}

void GLElementData::addColor(const QColor4ub &color)
{
    m_colors.push_back(color);
}

void GLElementData::addColorLine(const QColor4ub &color)
{
    addColor(color);
    addColor(color);
}

void GLElementData::addColorRect(const QColor4ub &color)
{
    addColor(color);
    addColor(color);
    addColor(color);
    addColor(color);
}

void GLElementData::addTexture(const QPointF &point)
{
    m_textures.push_back(point);
}

void GLElementData::addTexture(const QLineF &line)
{
    addTexture(line.p1());
    addTexture(line.p2());
}

void GLElementData::addTexture(const QRectF &rectangle)
{
    QPointF p0 = rectangle.topLeft();
    QPointF p1 = rectangle.topRight();
    QPointF p2 = rectangle.bottomLeft();
    QPointF p3 = rectangle.bottomRight();

    addTexture(p0);
    addTexture(p1);
    addTexture(p2);
    addTexture(p3);
}

void GLElementData::connectLine(GLindex *index)
{
    GLindex tail  = m_points.size() - 1;

    // return new index if pointer provided
    if (index != 0) {
        (*index) = m_indices.size();
    }

    for (IndexesType::size_type i = 0; i < line_size; ++i) {
        m_indices.push_back(tail--);
    }
}

void GLElementData::connectRect(GLindex *index)
{
    GLindex tail  = m_points.size() - 1;

    // return new index if pointer provided
    if (index != 0) {
        (*index) = m_indices.size();
    }

    for (IndexesType::size_type i = 0; i < rect_size; ++i) {
        m_indices.push_back(tail--);
    }
}

void GLElementData::connectLine(const GLindex &indexIn, GLindex *indexOut)
{
    GLindex tail  = m_points.size() - 1;

    // return new index if pointer provided
    if (index != 0) {
        (*indexOut) = m_indices.size();
    }

    for (IndexesType::size_type i = 0; i < line_size; ++i) {
        m_indices.push_back( (indexIn!= GL::INVALID_INDEX) ? indexIn : tail--);
    }
}

void GLElementData::connectRect(const GLindex &indexIn, GLindex *indexOut)
{
    GLindex tail  = m_points.size() - 1;

    // return new index if pointer provided
    if (index != 0) {
        (*indexOut) = m_indices.size();
    }

    for (IndexesType::size_type i = 0; i < rect_size; ++i) {
        m_indices.push_back( (indexIn != GL::INVALID_INDEX) ? indexIn : tail--);
    }
}

void GLElementData::deconnectLine(const GLindex &index)
{
    // get tail
    GLindex tail = m_indices.size() - line_size;

    if (index != tail) {
        for (IndexesType::size_type i = 0; i < line_size; ++i) {
            qSwap(m_indices[index + i], m_indices[tail + i]);
        }
    }

    m_indices.resize(tail);
}

void GLElementData::deconnectRect(const GLindex &index)
{
    // get tail
    GLindex tail = m_indices.size() - rect_size;

    if (index != tail) {
        for (IndexesType::size_type i = 0; i < rect_size; ++i) {
            qSwap(m_indices[index + i], m_indices[tail + i]);
        }
    }

    m_indices.resize(tail);
}

// data modifiers

void GLElementData::setShape(const GLindex &index, const QLineF &line)
{
    QPointF p0 = line.p1();
    QPointF p1 = line.p2();
    m_points[index] = p0;
    m_points[index + 1] = p1;
}

void GLElementData::setShape(const GLindex &index, const QRectF &rectangle)
{
    QPointF p0 = rectangle.topLeft();
    QPointF p1 = rectangle.topRight();
    QPointF p2 = rectangle.bottomLeft();
    QPointF p3 = rectangle.bottomRight();

    m_points[index] = p0;
    m_points[index + 1] = p1;
    m_points[index + 2] = p2;
    m_points[index + 3] = p3;
}

void GLElementData::setColorLine(const GLindex &index, const QColor4ub &color)
{
    m_colors[index] = color;
    m_colors[index + 1] = color;
}

void GLElementData::setColorRect(const GLindex &index, const QColor4ub &color)
{
    m_colors[index] = color;
    m_colors[index + 1] = color;
    m_colors[index + 2] = color;
    m_colors[index + 3] = color;
}

void GLElementData::setTexture(const GLindex &index, const QLineF &line)
{
    QPointF p0 = line.p1();
    QPointF p1 = line.p2();
    m_textures[index] = p0;
    m_textures[index + 1] = p1;
}

void GLElementData::setTexture(const GLindex &index, const QRectF &rectangle)
{
    QPointF p0 = rectangle.topLeft();
    QPointF p1 = rectangle.topRight();
    QPointF p2 = rectangle.bottomLeft();
    QPointF p3 = rectangle.bottomRight();

    m_textures[index] = p0;
    m_textures[index + 1] = p1;
    m_textures[index + 2] = p2;
    m_textures[index + 3] = p3;
}

// data access

const QColor4ub GLElementData::getColor(const GLindex &index)
{
    return m_colors[index]; // colors are the same for all the shape's vertexs
}

// state modifiers
void GLElementData::setMode(const GLenum &mode)
{
    m_mode = mode;
}

const GLElementData::VerticesType &GLElementData::vertices() const
{
    return m_points;
}

const GLElementData::ColorsType &GLElementData::colors() const
{
    return m_colors;
}

const GLElementData::IndexesType &GLElementData::indices() const
{
    return m_indices;
}

const GLElementData::TexturesType &GLElementData::textures() const
{
    return m_textures;
}

const GLenum &GLElementData::mode() const
{
    return m_mode;
}

} // namespace GL //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementRectangleFactory.h"

namespace GL
{

GLElementRectangleFactory::GLElementRectangleFactory(GLElementData &data, GLflag flags)
    : GLElementShapeFactory(data, flags)
{
    // initialize data
    m_data->setMode(GL_QUADS);
}

GLElementRectangleFactory::~GLElementRectangleFactory()
{

}

 GLindex GLElementRectangleFactory::addShape(const QPointF &point)
{
    return addShape(QRectF(point.x(), point.y(), point.y(), point.x()));
}

GLindex GLElementRectangleFactory::addShape(const QRectF &rectangle)
{
    GLindex index = m_data->addShape(rectangle);

    if (m_flags & AutoAddColor) {
        m_data->addColorRect(m_color);
    }
    if (m_flags & AutoAddTexture)  {
        m_data->addTexture(QRectF(QPointF(0,0), QPointF(1,1)));
    }
    if (m_flags & AutoAddConnection) {
        m_data->connectRect(index);
    }

    return translateInternalIndex(index);
}

GLindex GLElementRectangleFactory::connect(const GLindex &externalIndex)
{
    const GLindex indexIn = translateExternalIndex(externalIndex);
    GLindex IndexOut;
    m_data->connectRect(indexIn, &IndexOut);
    return translateInternalIndex(IndexOut);
}

void GLElementRectangleFactory::deconnect(const GLindex &externalIndex)
{
    const GLindex internalIndex = translateExternalIndex(externalIndex);
    m_data->deconnectRect(internalIndex);
}

void GLElementRectangleFactory::setColor(const QColor4ub &color)
{
    m_color = color;
}

void GLElementRectangleFactory::setSize(const qreal &size)
{
    m_size = QPointF(size, size);
}

void GLElementRectangleFactory::setSize(const qreal &width, const qreal &height)
{
    m_size = QPointF(width, height);
}

GLindex GLElementRectangleFactory::size() const
{
    const GLElementData::IndexesType &indicies = m_data->indices();
    return translateInternalIndex( indicies.size() );
}

void GLElementRectangleFactory::setShape(const GLindex &index, const QRectF &rectangle)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setShape(internalIndex, rectangle);
}

void GLElementRectangleFactory::setColor(const GLindex &index, const QColor4ub &color)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setColorRect(internalIndex, color);
}

void GLElementRectangleFactory::setTexture(const GLindex &index, const QRectF &texture)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setTexture(internalIndex, texture);
}

const QColor4ub GLElementRectangleFactory::getColor(const GLindex &index) const
{
    const GLindex internalIndex = translateExternalIndex(index);
    return m_data->getColor(internalIndex); // assume colors are reflected over all vertices
}

GLindex GLElementRectangleFactory::translateInternalIndex(const GLindex &internalIndex) const
{
    Q_ASSERT(internalIndex % size_value == 0);
    return internalIndex / size_value;
}

GLindex GLElementRectangleFactory::translateExternalIndex(const GLindex &externalIndex) const
{
    return externalIndex * size_value;
}

} // namespace GL

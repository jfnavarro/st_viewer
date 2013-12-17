/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementShapeFactory.h"

namespace GL
{

// GLElementShapeFactory
const GLfloat GLElementShapeFactory::DEFAULT_SIZE = 1.0f;

GLElementShapeFactory::GLElementShapeFactory(GLElementData &data, GLflag flags)
    : m_data(&data), m_flags(flags)
{
    // initialize data
    m_data->setMode(GL_LINES);
}

GLElementShapeFactory::~GLElementShapeFactory()
{

}

void GLElementShapeFactory::clear(GLflag flags)
{
    m_data->clear(flags);
}

// GLElementRectangleFactory
void GLElementRectangleFactory::setShape(const GLindex index, const GLrectangle &rectangle)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setShape(internalIndex, rectangle);
}

void GLElementRectangleFactory::setColor(const GLindex index, const GLcolor &color)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setColor(internalIndex, GLrectanglecolor(color));
}

void GLElementRectangleFactory::setTexture(const GLindex index, const GLrectangletexture &texture)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setTexture(internalIndex, texture);
}

void GLElementRectangleFactory::setOption(const GLindex index, const GLoption option)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setOption(internalIndex, GLrectangleoption(option));
}

const GLcolor GLElementRectangleFactory::getColor(const GLindex index) const
{
    const GLindex internalIndex = translateExternalIndex(index);
    return m_data->getColor<GLrectanglecolor::COLORS>(internalIndex).c[0]; // assume colors are reflected over all vertices
}

GLoption GLElementRectangleFactory::getOption(const GLindex index) const
{
    const GLindex internalIndex = translateExternalIndex(index);
    return m_data->getOption<GLrectangleoption::POINTS>(internalIndex).p[0]; // assume options are reflected over all vertices
}

GLindex GLElementRectangleFactory::translateInternalIndex(const GLindex internalIndex) const
{
    Q_ASSERT(internalIndex % GLrectangle::POINTS == 0);
    return internalIndex / GLrectangle::POINTS;
}

GLindex GLElementRectangleFactory::translateExternalIndex(const GLindex externalIndex) const
{
    return externalIndex * GLrectangle::POINTS;
}

// GLElementRectangleFactory::GLFactoryHandle
 GLElementRectangleFactory::GLFactoryHandle::GLFactoryHandle(GLElementRectangleFactory &factory,
                                                                   const GLindex index)
    : m_factory(factory), m_index(index)
{
}

GLElementRectangleFactory::GLFactoryHandle::GLFactoryHandle(const GLFactoryHandle &o)
    : m_factory(o.m_factory), m_index(o.m_index)
{
}

GLElementRectangleFactory::GLFactoryHandle::~GLFactoryHandle()
{
}

GLElementRectangleFactory::GLFactoryHandle
GLElementRectangleFactory::GLFactoryHandle::withColor(const GLcolor &color) const
{
    m_factory.setColor(m_index, color);
    return *(this);
}

GLElementRectangleFactory::GLFactoryHandle
GLElementRectangleFactory::GLFactoryHandle::withTexture(const GLrectangletexture &texture) const
{
    m_factory.setTexture(m_index, texture);
    return *(this);
}

GLElementRectangleFactory::GLFactoryHandle
GLElementRectangleFactory::GLFactoryHandle::withOption(const GLoption &option) const
{
    m_factory.setOption(m_index, option);
    return *(this);
}

GLindex GLElementRectangleFactory::GLFactoryHandle::index() const
{
    return m_index;
}

// GLElementRectangleFactory
GLElementRectangleFactory::GLElementRectangleFactory(GLElementData &data, GLflag flags)
    : GLElementShapeFactory(data, flags), m_color(GL::White), m_size(DEFAULT_SIZE, DEFAULT_SIZE)
{
    // initialize data
    m_data->setMode(GL_QUADS);
}

GLElementRectangleFactory::~GLElementRectangleFactory()
{

}

GLElementRectangleFactory::GLFactoryHandle GLElementRectangleFactory::addShape(const GLpoint &point)
{
    return addShape(GLrectangle(point, m_size.width, m_size.height));
}

GLElementRectangleFactory::GLFactoryHandle GLElementRectangleFactory::addShape(const GLrectangle &rectangle)
{
    GLindex index;
    m_data->addShape<GLrectangle::POINTS>(rectangle, &index);
    if (m_flags & AutoAddColor)
    {
        m_data->addColor<GLrectangle::POINTS>(GLrectanglecolor(m_color));
    }
    if (m_flags & AutoAddTexture)
    {
        m_data->addTexture<GLrectangle::POINTS>(GLrectangletexture());
    }
    if (m_flags & AutoAddOption)
    {
        m_data->addOption<GLrectangle::POINTS>(GLrectangleoption());
    }
    if (m_flags & AutoAddConnection)
    {
        m_data->connect<GLrectangle::POINTS>(GLrectangleindex(index));
    }

    const GLindex internalIndex = translateInternalIndex(index);
    return GLFactoryHandle(*(this), internalIndex);
}

GLindex GLElementRectangleFactory::connect(const GLindex externalIndex)
{
    GLindex index;
    GLrectangleindex rectangleIndex = GLrectangleindex(translateExternalIndex(externalIndex));
    m_data->connect<GLrectangle::POINTS>(rectangleIndex, &index);
    return translateInternalIndex(index);
}

void GLElementRectangleFactory::deconnect(const GLindex externalIndex)
{
    GLindex internalIndex = translateExternalIndex(externalIndex);
    m_data->deconnect<GLrectangle::POINTS>(internalIndex);
}

void GLElementRectangleFactory::setColor(const GLcolor &color)
{
    m_color = color;
}

void GLElementRectangleFactory::setSize(const GLfloat size)
{
    m_size = GLpoint(size, size);
}

void GLElementRectangleFactory::setSize(const GLfloat width, const GLfloat height)
{
    m_size = GLpoint(width, height);
}

GLindex GLElementRectangleFactory::size() const
{
    const GLarray<GLindex> indicies = m_data->indices();
    return translateInternalIndex((GLindex) indicies.size);
}

} // namespace GL //

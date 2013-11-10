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

// GLElementLineFactory
GLElementLineFactory::GLElementLineFactory(GLElementData &data, GLflag flags)
    : GLElementShapeFactory(data, flags), m_color(GL::White)
{
    // initialize data
    m_data->setMode(GL_LINES);
}

GLElementLineFactory::~GLElementLineFactory()
{

}

const GLindex GLElementLineFactory::addShape(const GLpoint &point)
{
    //TODO implement
    return INVALID_INDEX;
}

void GLElementLineFactory::setColor(const GLindex index, const GLcolor &color)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setColor(internalIndex, GLlinecolor(color));
}

const GLindex GLElementLineFactory::connect(const GLindex externalIndex)
{
    GLindex internalIndex = translateExternalIndex(externalIndex);
    GLlineindex lineIndex = GLlineindex(internalIndex);
    m_data->connect<GLline::POINTS>(lineIndex, &internalIndex);
    return translateInternalIndex(internalIndex);
}

void GLElementLineFactory::deconnect(const GLindex externalIndex)
{
    GLindex internalIndex = translateExternalIndex(externalIndex);
    m_data->deconnect<GLline::POINTS>(internalIndex);
}

void GLElementLineFactory::setColor(const GLcolor &color)
{
    m_color = color;
}

void GLElementLineFactory::setSize(const GLfloat size)
{
    //NOTE lines don't have sizes, 'tis a silly notion!
}

GLindex GLElementLineFactory::size() const
{
    const GLarray<GLindex> indicies = m_data->indices();
    return translateInternalIndex((GLindex) indicies.size);
}

const GLindex GLElementLineFactory::translateInternalIndex(const GLindex internalIndex) const
{
    Q_ASSERT(internalIndex % GLline::POINTS == 0);
    return internalIndex / GLline::POINTS;
}

const GLindex GLElementLineFactory::translateExternalIndex(const GLindex externalIndex) const
{
    return externalIndex * GLline::POINTS;
}

// GLElementTriangleFactory
GLElementTriangleFactory::GLElementTriangleFactory(GLElementData &data, GLflag flags)
    : GLElementShapeFactory(data, flags), m_color(GL::White), m_size(DEFAULT_SIZE)
{
    // initialize data
    m_data->setMode(GL_TRIANGLES);

}

GLElementTriangleFactory::~GLElementTriangleFactory()
{

}

const GLindex GLElementTriangleFactory::addShape(const GLpoint &point)
{
    return addShape(GLtriangle(point, m_size));
}

const GLindex GLElementTriangleFactory::addShape(const GLtriangle &triangle)
{
    GLindex index;
    m_data->addShape<GLtriangle::POINTS>(triangle, &index);
    if (m_flags & AutoAddColor)
    {
        m_data->addColor<GLtriangle::POINTS>(GLtrianglecolor(m_color));
    }
    if (m_flags & AutoAddTexture)
    {
        m_data->addTexture<GLtriangle::POINTS>(GLtriangletexture());
    }
    if (m_flags & AutoAddOption)
    {
        m_data->addOption<GLtriangle::POINTS>(GLtriangleoption());
    }
    if (m_flags & AutoAddConnection)
    {
        m_data->connect<GLtriangle::POINTS>(GLtriangleindex(index));
    }
    return translateInternalIndex(index);
}



const GLindex GLElementTriangleFactory::connect(const GLindex externalIndex)
{
    GLindex internalIndex = translateExternalIndex(externalIndex);
    GLtriangleindex triangleIndex = GLtriangleindex(internalIndex);
    m_data->connect<GLtriangle::POINTS>(triangleIndex, &internalIndex);
    return translateInternalIndex(internalIndex);
}

void GLElementTriangleFactory::deconnect(const GLindex externalIndex)
{
    GLindex internalIndex = translateExternalIndex(externalIndex);
    m_data->deconnect<GLtriangle::POINTS>(internalIndex);
}

void GLElementTriangleFactory::setColor(const GLcolor &color)
{
    m_color = color;
}

void GLElementTriangleFactory::setSize(const GLfloat size)
{
    m_size = size;
}

GLindex GLElementTriangleFactory::size() const
{
    const GLarray<GLindex> indicies = m_data->indices();
    return translateInternalIndex((GLindex) indicies.size);
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

const GLindex GLElementRectangleFactory::connect(const GLindex externalIndex)
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

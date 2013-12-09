/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTSHAPEFACTORY_H
#define GLELEMENTSHAPEFACTORY_H

#include "data/GLElementData.h"

// GLElementShapeFactory and subclasses use a factory design pattern to provide
// a means of generating and manipulating primitive geometry data. These
// classes should be used together with the GLElementData class to generate and
// store rendering data. Each factory hides the element data indicies and
// allows data to be accessed using shape dependent indicies
// (ie. 1st rectangle, 2nd rectangle, ...).
namespace GL
{
// GLElementShapeFactory
class GLElementShapeFactory
{

public:

    enum BuildFlag
    {
        Empty = 0x00,
        AutoAddColor = 0x01,
        AutoAddTexture = 0x02,
        AutoAddOption = 0x04,
        AutoAddConnection = 0x08
    };

    static const GLflag DEFAULT_CLEAR_FLAGS = GLElementData::Arrays;
    static const GLflag DEFAULT_BUILD_FLAGS = (AutoAddColor | AutoAddTexture);
    static const GLfloat DEFAULT_SIZE;

    GLElementShapeFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementShapeFactory();

    virtual void clear(GLflag flags = GLElementShapeFactory::DEFAULT_CLEAR_FLAGS);

    // data modifiers
    virtual GLindex connect(const GLindex index) = 0;
    virtual void deconnect(const GLindex index) = 0;

    // state modifiers
    virtual void setColor(const GLcolor& color) = 0;
    //         virtual void setTexture();
    virtual void setSize(const GLfloat size) = 0;

    virtual GLindex size() const = 0;

protected:

    GLElementData* m_data;
    GLflag m_flags;
};

// GLElementRectangleFactory
class GLElementRectangleFactory : public GLElementShapeFactory
{
public:
    class GLFactoryHandle
    {
    public:

        inline GLFactoryHandle(GLElementRectangleFactory &factory, const GLindex index);
        inline GLFactoryHandle(const GLFactoryHandle &o);
        inline virtual ~GLFactoryHandle();

        inline GLFactoryHandle withColor(const GLcolor &color) const;
        inline GLFactoryHandle withTexture(const GLrectangletexture &texture) const;
        inline GLFactoryHandle withOption(const GLoption &option) const;

        inline GLindex index() const;

    private:
        GLElementRectangleFactory &m_factory;
        GLindex m_index;
    };

    GLElementRectangleFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementRectangleFactory();

    // shape factory functions
    virtual GLindex connect(const GLindex index);
    virtual void deconnect(const GLindex index);

    inline void setShape(const GLindex index, const GLrectangle &rectangle);
    inline void setColor(const GLindex index, const GLcolor& color);
    inline void setTexture(const GLindex index, const GLrectangletexture &texture);
    inline void setOption(const GLindex index, const GLoption option);

    inline const GLcolor getColor(const GLindex index) const;
    inline GLoption getOption(const GLindex index) const;

    // rectangle shape factory functions
    GLFactoryHandle addShape(const GLpoint &point);
    GLFactoryHandle addShape(const GLrectangle& rectangle);

    // state modifiers
    virtual void setColor(const GLcolor& color);
    virtual void setSize(const GLfloat size);
    void setSize(const GLfloat width, const GLfloat height);

    virtual GLindex size() const;

private:

    inline GLindex translateInternalIndex(const GLindex internalIndex) const;
    inline GLindex translateExternalIndex(const GLindex externalIndex) const;

    GLcolor m_color;
    GLpoint m_size;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

// GLElementRectangleFactory
inline void GLElementRectangleFactory::setShape(const GLindex index, const GLrectangle &rectangle)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setShape(internalIndex, rectangle);
}

inline void GLElementRectangleFactory::setColor(const GLindex index, const GLcolor &color)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setColor(internalIndex, GLrectanglecolor(color));
}

inline void GLElementRectangleFactory::setTexture(const GLindex index, const GLrectangletexture &texture)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setTexture(internalIndex, texture);
}

inline void GLElementRectangleFactory::setOption(const GLindex index, const GLoption option)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setOption(internalIndex, GLrectangleoption(option));
}

inline const GLcolor GLElementRectangleFactory::getColor(const GLindex index) const
{
    const GLindex internalIndex = translateExternalIndex(index);
    return m_data->getColor<GLrectanglecolor::COLORS>(internalIndex).c[0]; // assume colors are reflected over all vertices
}

inline GLoption GLElementRectangleFactory::getOption(const GLindex index) const
{
    const GLindex internalIndex = translateExternalIndex(index);
    return m_data->getOption<GLrectangleoption::POINTS>(internalIndex).p[0]; // assume options are reflected over all vertices
}

inline GLindex GLElementRectangleFactory::translateInternalIndex(const GLindex internalIndex) const
{
    Q_ASSERT(internalIndex % GLrectangle::POINTS == 0);
    return internalIndex / GLrectangle::POINTS;
}

inline GLindex GLElementRectangleFactory::translateExternalIndex(const GLindex externalIndex) const
{
    return externalIndex * GLrectangle::POINTS;
}

// GLElementRectangleFactory::GLFactoryHandle
inline GLElementRectangleFactory::GLFactoryHandle::GLFactoryHandle(GLElementRectangleFactory &factory,
                                                                   const GLindex index)
    : m_factory(factory), m_index(index)
{
}

inline GLElementRectangleFactory::GLFactoryHandle::GLFactoryHandle(const GLFactoryHandle &o)
    : m_factory(o.m_factory), m_index(o.m_index)
{
}

inline GLElementRectangleFactory::GLFactoryHandle::~GLFactoryHandle()
{
}

inline GLElementRectangleFactory::GLFactoryHandle
GLElementRectangleFactory::GLFactoryHandle::withColor(const GLcolor &color) const
{
    m_factory.setColor(m_index, color);
    return *(this);
}

inline GLElementRectangleFactory::GLFactoryHandle
GLElementRectangleFactory::GLFactoryHandle::withTexture(const GLrectangletexture &texture) const
{
    m_factory.setTexture(m_index, texture);
    return *(this);
}

inline GLElementRectangleFactory::GLFactoryHandle
GLElementRectangleFactory::GLFactoryHandle::withOption(const GLoption &option) const
{
    m_factory.setOption(m_index, option);
    return *(this);
}

inline GLindex GLElementRectangleFactory::GLFactoryHandle::index() const
{
    return m_index;
}

} // namespace GL //

#endif // GLELEMENTSHAPEFACTORY_H //

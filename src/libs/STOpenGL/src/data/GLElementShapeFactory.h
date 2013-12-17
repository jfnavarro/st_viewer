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
    // virtual void setTexture();
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

        GLFactoryHandle(GLElementRectangleFactory &factory, const GLindex index);
        GLFactoryHandle(const GLFactoryHandle &o);
        virtual ~GLFactoryHandle();

        GLFactoryHandle withColor(const GLcolor &color) const;
        GLFactoryHandle withTexture(const GLrectangletexture &texture) const;
        GLFactoryHandle withOption(const GLoption &option) const;

        GLindex index() const;

    private:
        GLElementRectangleFactory &m_factory;
        GLindex m_index;
    };

    GLElementRectangleFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementRectangleFactory();

    // shape factory functions
    virtual GLindex connect(const GLindex index);
    virtual void deconnect(const GLindex index);

    void setShape(const GLindex index, const GLrectangle &rectangle);
    void setColor(const GLindex index, const GLcolor& color);
    void setTexture(const GLindex index, const GLrectangletexture &texture);
    void setOption(const GLindex index, const GLoption option);

    const GLcolor getColor(const GLindex index) const;
    GLoption getOption(const GLindex index) const;

    // rectangle shape factory functions
    GLFactoryHandle addShape(const GLpoint &point);
    GLFactoryHandle addShape(const GLrectangle& rectangle);

    // state modifiers
    virtual void setColor(const GLcolor& color);
    virtual void setSize(const GLfloat size);
    void setSize(const GLfloat width, const GLfloat height);

    virtual GLindex size() const;

private:

    GLindex translateInternalIndex(const GLindex internalIndex) const;
    GLindex translateExternalIndex(const GLindex externalIndex) const;

    GLcolor m_color;
    GLpoint m_size;
};

} // namespace GL //

#endif // GLELEMENTSHAPEFACTORY_H //

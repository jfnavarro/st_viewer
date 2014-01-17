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
        AutoAddConnection = 0x04
    };

    static const GLflag DEFAULT_CLEAR_FLAGS = GLElementData::Arrays;
    static const GLflag DEFAULT_BUILD_FLAGS = (AutoAddColor | AutoAddTexture);
    static const GLfloat DEFAULT_SIZE;

    GLElementShapeFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementShapeFactory();

    virtual void clear(GLflag flags = DEFAULT_CLEAR_FLAGS);

    // data modifiers
    virtual GLindex connect(const GLindex& index) = 0;
    virtual void deconnect(const GLindex& index) = 0;

    // state modifiers
    virtual void setColor(const GLcolor& color) = 0;
    //virtual GLcolor color() = 0;

    // virtual void setTexture();
    // virtual QGLTexture* texture();

    virtual void setSize(const GLfloat& size) = 0;
    virtual void setSize(const GLfloat &width, const GLfloat &height) = 0;
    virtual GLindex size() const = 0;

protected:

    GLElementData* m_data;
    GLflag m_flags;
    GLcolor m_color;
    QPointF m_size;
};

// GLElementRectangleFactory


} // namespace GL //

#endif // GLELEMENTSHAPEFACTORY_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTRECTANGLEFACTORY_H
#define GLELEMENTRECTANGLEFACTORY_H

#include "GLElementShapeFactory.h"

namespace GL
{

class GLElementRectangleFactory : public GLElementShapeFactory
{
public:

    GLElementRectangleFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementRectangleFactory();

    // shape factory functions
    virtual GLindex connect(const GLindex& index);
    virtual void deconnect(const GLindex& index);

    void setShape(const GLindex& index, const GLrectangle &rectangle);
    void setColor(const GLindex& index, const GLcolor& color);
    void setTexture(const GLindex& index, const GLrectangletexture &texture);
    void setOption(const GLindex& index, const GLoption option);

    const GLcolor getColor(const GLindex& index) const;
    const GLoption& getOption(const GLindex& index) const;

    // rectangle shape factory functions
    GLindex addShape(const GLpoint &point);
    GLindex addShape(const GLrectangle& rectangle);

    // state modifiers
    virtual void setColor(const GLcolor& color);
    virtual void setSize(const GLfloat& size);
    void setSize(const GLfloat &width, const GLfloat &height);

    virtual GLindex size() const;

private:

    GLindex translateInternalIndex(const GLindex &internalIndex) const;
    GLindex translateExternalIndex(const GLindex &externalIndex) const;

    GLcolor m_color;
    GLpoint m_size;
};

} // namespace GL //

#endif // GLELEMENTRECTANGLEFACTORY_H

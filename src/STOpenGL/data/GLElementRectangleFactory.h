/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTRECTANGLEFACTORY_H
#define GLELEMENTRECTANGLEFACTORY_H

#include "GLElementShapeFactory.h"

class QRectF;
class QPointF;

namespace GL
{

class GLElementRectangleFactory : public GLElementShapeFactory
{
public:

    static const int size_value = 2;

    GLElementRectangleFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementRectangleFactory();

    // shape factory functions
    virtual GLindex connect(const GLindex& index);
    virtual void deconnect(const GLindex& index);

    // setters
    void setShape(const GLindex& index, const QRectF &rectangle);
    void setColor(const GLindex& index, const QColor4ub &color);
    void setTexture(const GLindex& index, const QRectF &texture);

    // getters
    const QColor4ub getColor(const GLindex& index) const;

    // rectangle shape factory functions
    GLindex addShape(const QPointF &point);
    GLindex addShape(const QRectF& rectangle);

    // state modifiers
    virtual void setColor(const QColor4ub& color);
    virtual void setSize(const qreal &size);
    virtual void setSize(const qreal &width, const qreal &height);

    virtual GLindex size() const;

private:

    GLindex translateInternalIndex(const GLindex &internalIndex) const;
    GLindex translateExternalIndex(const GLindex &externalIndex) const;
};

} // namespace GL //

#endif // GLELEMENTRECTANGLEFACTORY_H

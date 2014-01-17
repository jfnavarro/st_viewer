#ifndef GLELEMENTLINEFACTORY_H
#define GLELEMENTLINEFACTORY_H

#include "GLElementShapeFactory.h"

class QLineF;
class QPointF;

namespace GL
{

class GLElementLineFactory : public GLElementShapeFactory
{
public:

    static const int size_value = 2;

    GLElementLineFactory(GLElementData& data, GLflag flags = DEFAULT_BUILD_FLAGS);
    virtual ~GLElementLineFactory();

    // shape factory functions
    virtual GLindex connect(const GLindex& index);
    virtual void deconnect(const GLindex& index);

    // setters
    void setShape(const GLindex& index, const QLineF &line);
    void setColor(const GLindex& index, const GLcolor &color);

    // getters
    const GLcolor getColor(const GLindex& index) const;

    // rectangle shape factory functions
    GLindex addShape(const QPointF &point1, const QPointF &point2);
    GLindex addShape(const QLineF& line);

    // state modifiers
    virtual void setColor(const GLcolor& color);
    virtual void setSize(const GLfloat& size);
    virtual void setSize(const GLfloat &width, const GLfloat &height);

    virtual GLindex size() const;

private:

    GLindex translateInternalIndex(const GLindex &internalIndex) const;
    GLindex translateExternalIndex(const GLindex &externalIndex) const;
};

} // namespace GL //

#endif // GLELEMENTLINEFACTORY_H

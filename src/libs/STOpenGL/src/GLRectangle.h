#ifndef GLRECTANGLE_H
#define GLRECTANGLE_H

class GLRectangle
{
public:
    typedef GLpointdata<4> shape_type;
    inline GLrectangle();
    inline explicit GLrectangle(GL::Initialization);
    inline GLrectangle(GLfloat x, GLfloat y, GLfloat size);
    inline GLrectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height);
    inline GLrectangle(const GLpoint &point, GLfloat size);
    inline GLrectangle(const GLpoint &point, GLfloat width, GLfloat height);
    inline GLrectangle(const GLpoint &p0, const GLpoint &p1, const GLpoint &p2, const GLpoint &p3);
    inline static const GLrectangle fromCorners(const GLpoint &topLeft, const GLpoint &bottomRight);
    static const GLrectangle fromLine(const GLpoint &from, const GLpoint &to, GLfloat lineSize = 1.0f);
};

#endif // GLRECTANGLE_H

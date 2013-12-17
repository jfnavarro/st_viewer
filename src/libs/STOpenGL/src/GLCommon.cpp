/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "math/GLVector.h"

#include "GLCommon.h"

namespace GL
{

// misc functions
bool fuzzyEqual(GLfloat s0, GLfloat s1, GLfloat e)
{
    return fabs(s1 - s0) <= e;
}

bool fuzzyNotEqual(GLfloat s0, GLfloat s1, GLfloat e)
{
    return fabs(s1 - s0) >= e;
}

// vertex data structures

// simple point
GLpoint::GLpoint() : x(0.0f), y(0.0f)
{

}

GLpoint::GLpoint(GLfloat x, GLfloat y) : x(x), y(y)
{

}

const GLpoint min(const GLpoint &a, const GLpoint &b)
{
    return GLpoint(GL::min<GLfloat>(a.x, b.x), GL::min<GLfloat>(a.y, b.y));
}

const GLpoint max(const GLpoint &a, const GLpoint &b)
{
    return GLpoint(GL::max<GLfloat>(a.x, b.x), GL::max<GLfloat>(a.y, b.y));
}

bool fuzzyEqual(const GLpoint &p0, const GLpoint &p1, GLfloat e)
{
    return fuzzyEqual(p0.x, p1.x, e) && fuzzyEqual(p0.y, p1.y, e);
}

bool fuzzyNotEqual(const GLpoint &p0, const GLpoint &p1, GLfloat e)
{
    return fuzzyNotEqual(p0.x, p1.x, e) || fuzzyNotEqual(p0.y, p1.y, e);
}

bool operator !=(const GLpoint &lhs, const GLpoint &rhs)
{
    return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}

bool operator ==(const GLpoint &lhs, const GLpoint &rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

// simple options
GLlineoption::GLlineoption(const GLoption option)
{
    p[0] = p[1] = option;
}

GLrectangleoption::GLrectangleoption(const GLoption option)
{
    p[0] = p[1] = p[2] = p[3] = option;
}

GLline::GLline()
{
    shape_type::p[0] = GLpoint();
    shape_type::p[1] = GLpoint();
}

GLline::GLline(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1)
{
    shape_type::p[0] = GLpoint(x0, y0);
    shape_type::p[1] = GLpoint(x1, y1);
}

GLline::GLline(const GLpoint &point0, const GLpoint &point1)
{
    shape_type::p[0] = point0;
    shape_type::p[1] = point1;
}

bool fuzzyEqual(const GLline &l0, const GLline &l1, const GLfloat e)
{
    return fuzzyEqual(l0.p[0], l1.p[0], e) && fuzzyEqual(l0.p[1], l1.p[1], e);
}

bool fuzzyNotEqual(const GLline &l0, const GLline &l1, const GLfloat e)
{
    return fuzzyNotEqual(l0.p[0], l1.p[0], e) || fuzzyNotEqual(l0.p[1], l1.p[1], e);
}

// simple rectangle
GLrectangle::GLrectangle()
{
    shape_type::p[0] = GLpoint();
    shape_type::p[1] = GLpoint();
    shape_type::p[2] = GLpoint();
    shape_type::p[3] = GLpoint();
}

GLrectangle::GLrectangle(GLfloat x, GLfloat y, GLfloat size)
{
    shape_type::p[0] = GLpoint(x - size / 2.0f, y - size / 2.0f);
    shape_type::p[1] = GLpoint(x + size / 2.0f, y - size / 2.0f);
    shape_type::p[2] = GLpoint(x + size / 2.0f, y + size / 2.0f);
    shape_type::p[3] = GLpoint(x - size / 2.0f, y + size / 2.0f);
}

GLrectangle::GLrectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
    shape_type::p[0] = GLpoint(x - width / 2.0f, y - height / 2.0f);
    shape_type::p[1] = GLpoint(x + width / 2.0f, y - height / 2.0f);
    shape_type::p[2] = GLpoint(x + width / 2.0f, y + height / 2.0f);
    shape_type::p[3] = GLpoint(x - width / 2.0f, y + height / 2.0f);
}

GLrectangle::GLrectangle(const GLpoint &point, GLfloat size)
{
    shape_type::p[0] = GLpoint(point.x - size / 2.0f, point.y - size / 2.0f);
    shape_type::p[1] = GLpoint(point.x + size / 2.0f, point.y - size / 2.0f);
    shape_type::p[2] = GLpoint(point.x + size / 2.0f, point.y + size / 2.0f);
    shape_type::p[3] = GLpoint(point.x - size / 2.0f, point.y + size / 2.0f);
}

GLrectangle::GLrectangle(const GLpoint &point, GLfloat width, GLfloat height)
{
    shape_type::p[0] = GLpoint(point.x - width / 2.0f, point.y - height / 2.0f);
    shape_type::p[1] = GLpoint(point.x + width / 2.0f, point.y - height / 2.0f);
    shape_type::p[2] = GLpoint(point.x + width / 2.0f, point.y + height / 2.0f);
    shape_type::p[3] = GLpoint(point.x - width / 2.0f, point.y + height / 2.0f);
}

GLrectangle::GLrectangle(const GLpoint &p0, const GLpoint &p1, const GLpoint &p2, const GLpoint &p3)
{
    shape_type::p[0] = p0;
    shape_type::p[1] = p1;
    shape_type::p[2] = p2;
    shape_type::p[3] = p3;
}

const GLrectangle GLrectangle::fromCorners(const GLpoint &topLeft, const GLpoint &bottomRight)
{
    GLrectangle rectangle;
    rectangle.p[0] = topLeft;
    rectangle.p[1] = GLpoint(bottomRight.x, topLeft.y);
    rectangle.p[2] = bottomRight;
    rectangle.p[3] = GLpoint(topLeft.x, bottomRight.y);
    return rectangle;
}

// texture data structures

// simple rectangle
GLrectangletexture::GLrectangletexture()
{
    shape_type::p[0] = GLpoint(0.0f, 0.0f); // 0 ___ 1
    shape_type::p[1] = GLpoint(1.0f, 0.0f); //  |   |
    shape_type::p[2] = GLpoint(1.0f, 1.0f); //  |___|
    shape_type::p[3] = GLpoint(0.0f, 1.0f); // 3     4
}

GLrectangletexture::GLrectangletexture(const GLrectangle &rectangle)
{
    shape_type::p[0] = rectangle.p[0];
    shape_type::p[1] = rectangle.p[1];
    shape_type::p[2] = rectangle.p[2];
    shape_type::p[3] = rectangle.p[3];
}

GLrectangletexture::GLrectangletexture(const GLpoint &point0, const GLpoint &point1,
        const GLpoint &point2, const GLpoint &point3)
{
    shape_type::p[0] = point0;
    shape_type::p[1] = point1;
    shape_type::p[2] = point2;
    shape_type::p[3] = point3;
}

const GLrectangle GLrectangle::fromLine(const GLpoint& from, const GLpoint& to, GLfloat lineSize)
{
    Q_ASSERT((from != to) && lineSize > 0.0f);

    const GLvector base0 = (0.5f * lineSize) * (GLvector::fromPoints(from, to).normalize());
    const GLvector base1 = GL::rotate<-90>(base0);

    GLrectangle rectangle;
    rectangle.p[0] = (from - base0 + base1);
    rectangle.p[1] = (to + base0 + base1);
    rectangle.p[2] = (to + base0 - base1);
    rectangle.p[3] = (from - base0 - base1);
    return rectangle;
}

} // namespace GL //

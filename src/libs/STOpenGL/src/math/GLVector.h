/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLVECTOR_H
#define GLVECTOR_H

#include <qmath.h>

#include "GLCommon.h"

namespace GL
{
// Simple implementation of linear algebraical vector (using Cartesian
// coordinates). Provides functionality to add and subtract and extends
// functionality to incorporate interactions with GLpoint objects.
//TODO extend if needed.

struct GLvector {

    inline GLvector();
    inline virtual ~GLvector();
    inline GLvector(const GLfloat x, const GLfloat y);

    static inline const GLvector fromPoints(const GLpoint& from, const GLpoint& to);

    inline GLfloat length() const;

    // length squared
    inline GLfloat length2() const;
    inline GLvector normalize() const;
    inline GLfloat dot(const GLvector &o) const;

    //      | y
    // +----+----+      returns the quadrant indicated by the vector
    // | Q1 | Q0 |  x   ie. interprets the direction of the vector in
    // |----+----+--    terms of quadrants
    // | Q2 | Q3 |
    // +---------+
    inline GLuint quadrant() const;

    union {
        GLfloat x;
        GLfloat width;
    };
    union {
        GLfloat y;
        GLfloat height;
    };
};

inline bool fuzzyEqual(const GLvector& v0, const GLvector& v1, const GLfloat e = EPSILON);
inline bool fuzzyNotEqual(const GLvector& v0, const GLvector& v1, const GLfloat e = EPSILON);

inline bool operator ==(const GLvector& v0, const GLvector& v1);
inline bool operator !=(const GLvector& v0, const GLvector& v1);

inline const GLvector operator +(const GLvector& v);
inline const GLvector operator -(const GLvector& v);

inline const GLvector operator *(const GLfloat s, const GLvector& v);
inline const GLvector operator *(const GLvector& v, const GLfloat s);
inline const GLvector operator /(const GLvector& v, const GLfloat s);

inline const GLvector operator +(const GLvector& v0, const GLvector& v1);
inline const GLvector operator -(const GLvector& v0, const GLvector& v1);

inline const GLpoint operator +(const GLpoint& p, const GLvector& v);
inline const GLpoint operator -(const GLpoint& p, const GLvector& v);

template <int N>
inline const GLvector rotate(const GLvector& v);

} // namespace GL //


/****************************************** DEFINITION ******************************************/

namespace GL
{

inline GLvector::GLvector() : x(0.0f), y(0.0f)
{

}

inline GLvector::GLvector(const GLfloat x, const GLfloat y) : x(x), y(y)
{

}

inline GLvector::~GLvector()
{

}

inline const GLvector GLvector::fromPoints(const GLpoint& from, const GLpoint& to)
{
    return GLvector(to.x - from.x, to.y - from.y);
}

inline GLfloat GLvector::length() const
{
    return qSqrt(x * x + y * y);
}

inline GLfloat GLvector::length2() const
{
    return (x * x + y * y);
}

inline GLvector GLvector::normalize() const
{
    const GLfloat len = length();
    return (fuzzyEqual(len, 0.0f)) ? GLvector() : GLvector(x / len, y / len);
}

inline GLfloat GLvector::dot(const GLvector &o) const
{
    return (x * o.x) + (y * o.y);
}

inline GLuint GLvector::quadrant() const
{
    static const GLuint table[] = {0u, 1u, 3u, 2u};
    const GLuint idx = ((x < 0.0f) ? 1u : 0u) + ((y < 0.0f) ? 2u : 0u);
    return table[idx];
}

inline bool fuzzyEqual(const GLvector& v0, const GLvector& v1, const GLfloat e)
{
    return fuzzyEqual(v0.x, v1.x, e) && fuzzyEqual(v0.y, v1.y, e);
}

inline bool fuzzyNotEqual(const GLvector& v0, const GLvector& v1, const GLfloat e)
{
    return fuzzyNotEqual(v0.x, v1.x, e) || fuzzyNotEqual(v0.y, v1.y, e);
}

inline bool operator ==(const GLvector& v0, const GLvector& v1)
{
    return v0.x == v1.x && v0.y == v1.y;
}

inline bool operator !=(const GLvector& v0, const GLvector& v1)
{
    return v0.x != v1.x || v0.y != v1.y;
}

inline const GLvector operator +(const GLvector& v)
{
    return v;
}

inline const GLvector operator -(const GLvector& v)
{
    return GLvector(-v.x, -v.y);
}

inline const GLvector operator*(const GLfloat s, const GLvector& v)
{
    return GLvector(s * v.x, s * v.y);
}

inline const GLvector operator*(const GLvector& v, const GLfloat s)
{
    return GLvector(v.x * s, v.y * s);
}

inline const GLvector operator/(const GLvector& v, const GLfloat s)
{
    return GLvector(v.x / s, v.y / s);
}

inline const GLvector operator +(const GLvector& v0, const GLvector& v1)
{
    return GLvector(v0.x + v1.x, v0.y + v1.y);
}

inline const GLvector operator -(const GLvector& v0, const GLvector& v1)
{
    return GLvector(v0.x - v1.x, v0.y - v1.y);
}

inline const GLpoint operator +(const GLpoint& p, const GLvector& v)
{
    return GLpoint(p.x + v.x, p.y + v.y);
}

inline const GLpoint operator -(const GLpoint& p, const GLvector& v)
{
    return GLpoint(p.x - v.x, p.y - v.y);
}

template <> inline const GLvector rotate < -270 > (const GLvector& v)
{
    return GLvector(-v.y, v.x);
}

template <> inline const GLvector rotate < -90 > (const GLvector& v)
{
    return GLvector(v.y, -v.x);
}

template <> inline const GLvector rotate<90>(const GLvector& v)
{
    return GLvector(-v.y, v.x);
}

template <> inline const GLvector rotate<270>(const GLvector& v)
{
    return GLvector(v.y, -v.x);
}

} // namespace GL //

#endif // GLVECTOR_H //

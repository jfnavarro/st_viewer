/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef GLCOMMON_H
#define GLCOMMON_H

#include <qopengl.h>
#include <math/GLMath.h>

// Common provides miscellaneous functionality related to the opengl library.
// It defines the a set of common geometrical types each representing one of
// the common rendering types (ie. vertex, colour, texture coordinates, etc.)
// These types are primarily intended to be used in constructing rendering
// array structures (such as vertex arrays, colour arrays, etc.).
namespace GL
{

// static constants & typedefs
typedef GLuint GLflag;
typedef GLuint GLindex;
static const GLindex INVALID_INDEX = GLindex(-1); //NOTE GLuint is unsigned integer
static const GLfloat EPSILON = GLfloat(1.0e-5);

// misc functions
bool fuzzyEqual(GLfloat s0, GLfloat s1, GLfloat e = EPSILON);
bool fuzzyNotEqual(GLfloat s0, GLfloat s1, GLfloat e = EPSILON);

// misc data structures
// simple data pair
template <typename T, typename U>
struct GLpair {
    T first;
    U second;
};

// simple read-only array
template <typename T>
struct GLarray {

    GLarray();
    GLarray(GLsizei size, const T *data);
    virtual ~GLarray();

    const T &operator [](GLint i) const;

    GLsizei size;
    const T *data;
};

// geometry data structures

// simple point
struct GLpoint {

    GLpoint();
    GLpoint(GLfloat x, GLfloat y);

    union {
        GLfloat x;
        GLfloat width;
    };
    union {
        GLfloat y;
        GLfloat height;
    };
};

// math function specialization
const GLpoint min(const GLpoint &a, const GLpoint &b);
const GLpoint max(const GLpoint &a, const GLpoint &b);

bool fuzzyEqual(const GLpoint &p0, const GLpoint &p1, GLfloat e = EPSILON);
bool fuzzyNotEqual(const GLpoint &p0, const GLpoint &p1, GLfloat e = EPSILON);

bool operator !=(const GLpoint &lhs, const GLpoint &rhs);
bool operator ==(const GLpoint &lhs, const GLpoint &rhs);

GLfloat distance(const GLpoint &lhs, const GLpoint &rhs);

// simple base data
template <typename T, int N>
struct GLdata {
    static const GLuint POINTS = GLuint(N);
    T p[N];
};

// simple options
typedef GLfloat GLoption;

template <int N>
struct GLoptiondata : public GLdata<GLoption, N>
{

};

struct GLlineoption : public GLoptiondata<2> {
    typedef GLoptiondata<2> option_type;
    explicit GLlineoption(const GLoption option = GLoption());
};

struct GLrectangleoption : public GLoptiondata<4> {
    typedef GLoptiondata<4> option_type;
    explicit GLrectangleoption(const GLoption option = GLoption());
};

// simple base shape
template <int N>
struct GLpointdata : public GLdata<GLpoint, N>
{

};

template <int N>
bool fuzzyEqual(const GLpointdata<N> &p0, const GLpointdata<N> &p1, const GLfloat e = EPSILON);
template <int N>
bool fuzzyNotEqual(const GLpointdata<N> &p0, const GLpointdata<N> &p1, const GLfloat e = EPSILON);

// simple line
struct GLline : public GLpointdata<2> {
    typedef GLpointdata<2> shape_type;

    GLline();
    GLline(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);
    GLline(const GLpoint &point0, const GLpoint &point1);
};

bool fuzzyEqual(const GLline &l0, const GLline &l1, const GLfloat e = EPSILON);
bool fuzzyNotEqual(const GLline &l0, const GLline &l1, const GLfloat e = EPSILON);

// simple rectangle
struct GLrectangle : public GLpointdata<4> {
    typedef GLpointdata<4> shape_type;

    GLrectangle();

    GLrectangle(GLfloat x, GLfloat y, GLfloat size);
    GLrectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height);
    GLrectangle(const GLpoint &point, GLfloat size);
    GLrectangle(const GLpoint &point, GLfloat width, GLfloat height);
    GLrectangle(const GLpoint &p0, const GLpoint &p1, const GLpoint &p2, const GLpoint &p3);

    static const GLrectangle fromCorners(const GLpoint &topLeft, const GLpoint &bottomRight);
    static const GLrectangle fromLine(const GLpoint &from, const GLpoint &to, GLfloat lineSize = 1.0f);
};

bool fuzzyEqual(GLfloat s0, GLfloat s1, GLfloat e);
bool fuzzyNotEqual(GLfloat s0, GLfloat s1, GLfloat e);

// textures

// simple rectangle
struct GLrectangletexture : public GLpointdata<4> {

    typedef GLpointdata<4> shape_type;

    GLrectangletexture();
    GLrectangletexture(const GLrectangle &rectangle);
    GLrectangletexture(const GLpoint &point0, const GLpoint &point1, const GLpoint &point2, const GLpoint &point3);
};

// index data structures
template <int N>
struct GLindexdata {
    static const GLuint INDICIES = N;

    GLindexdata();
    explicit GLindexdata(const GLindex first);

    GLindex i[N];
};
typedef GLindexdata<2> GLlineindex; //used for grid
typedef GLindexdata<4> GLrectangleindex; //used for genes

} // namespace GL //


/****************************************** DEFINITION ******************************************/

namespace GL
{

// simple read-only array
template <typename T>
GLarray<T>::GLarray() : size(0), data(0)
{

}

template <typename T>
GLarray<T>::GLarray(GLsizei size, const T *data)
    : size(size), data(data)
{

}

template <typename T>
GLarray<T>::~GLarray()
{

}

template <typename T>
const T &GLarray<T>::operator [](GLint i) const
{
    return data[i];
}

// simple base shape
template <int N>
bool fuzzyEqual(const GLpointdata<N> &p0, const GLpointdata<N> &p1, const GLfloat e)
{
    for (int i = 1; i < N; ++i) {
        if (!fuzzyEqual(p0.p[i], p1.p[i], e)) {
            return false;
        }
    }
    return true;
}

template <int N>
bool fuzzyNotEqual(const GLpointdata<N> &p0, const GLpointdata<N> &p1, const GLfloat e)
{
    for (int i = 1; i < N; ++i) {
        if (fuzzyNotEqual(p0.p[i], p1.p[i], e)) {
            return true;
        }
    }
    return false;
}

// index data structures

template <int N>
GLindexdata<N>::GLindexdata()
{
    for (int i = 0; i < N; ++i) {
        GLindexdata<N>::i[i] = GL::INVALID_INDEX;
    }
}

template <int N>
GLindexdata<N>::GLindexdata(const GLindex first)
{
    for (GLuint i = 0; i < GLindexdata<N>::INDICIES; ++i) {
        GLindexdata<N>::i[i] = (first + i);
    }
}

} // namespace GL //

#endif // GLCOMMON_H //

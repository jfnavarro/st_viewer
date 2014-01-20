/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef GLCOMMON_H
#define GLCOMMON_H

#include <qopengl.h>

#include <QPointF>
#include <QColor4ub>

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

inline const QPointF min(const QPointF &a, const QPointF &b)
{
    return QPointF(std::min(a.x(), b.x()), std::min(a.y(), b.y()));
}

inline const QPointF max(const QPointF &a, const QPointF &b)
{
    return QPointF(std::max(a.x(), b.x()), std::max(a.y(), b.y()));
}

template <typename T>
inline T clamp(T in, T low, T high)
{
    return std::min(std::max(in, low), high);
}

template <typename T, typename R>
inline const R norm(const T v, const T t0, const T t1)
{
    const T vh = clamp(v, t0, t1);
    return R(vh - t0) / R(t1 - t0);
}

template <typename T, typename R>
inline const T denorm(const R nv, const T t0, const T t1)
{
    const R vh = clamp(nv, R(0.0), R(1.0));
    return T(vh * (t1 - t0)) + t0;
}

inline const QColor4ub lerp(const qreal t, const QColor4ub &c0, const QColor4ub &c1)
{
    return QColor4ub(
                (c0.red() + (c1.red() - c0.red()) * t),
                (c0.green() + (c1.green() - c0.green()) * t),
                (c0.blue() + (c1.blue() - c0.blue()) * t),
                (c0.alpha() + (c1.alpha() - c0.alpha()) * t)
                );
}

inline const QColor4ub invlerp(const qreal t, const QColor4ub &c0, const QColor4ub &c1)
{
    const qreal invt = 1.0 / (1.0 - t);
    return QColor4ub(
                (c0.red() - t * c1.red()) * invt,
                (c0.green() - t * c1.green()) * invt,
                (c0.blue() - t * c1.blue()) * invt,
                (c0.alpha() - t * c1.alpha()) * invt
                );
}

} // namespace GL //

#endif // GLCOMMON_H //

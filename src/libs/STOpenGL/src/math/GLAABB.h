/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLAABB_H
#define GLAABB_H

#include "GLCommon.h"

#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <cmath>

namespace GL
{
// Simple implementation of an Axis Aligned Bounding Box
// The AABB provides simple functionality for testing collisions and
// intersections. In addition convenience functions for splitting the
// AABB is provided to simplify implementations of for instance quad trees.
struct GLaabb {

    inline GLaabb();
    inline virtual ~GLaabb();
    inline GLaabb(const qreal x, const qreal y, const qreal width, const qreal height);
    inline GLaabb(const QPointF &p, const QSizeF &size);
    inline GLaabb(const QRectF &rect);

    // create an aabb between two given points
    static inline const GLaabb fromPoints(const QPointF &p0, const QPointF &p1);

    // create a rectangle from the given aabb
    static inline const QRectF toRectangle(const GLaabb &b);

    // SplitHalf: splits the aabb in half
    enum SplitHalf { H0, H1, V0, V1 };
    inline const GLaabb split(SplitHalf split) const;

    // SplitQuad: splits the aabb in four equally large segments
    enum SplitQuad { Q0 = 0x00, Q1 = 0x01, Q2 = 0x02, Q3 = 0x03 };
    inline const GLaabb split(SplitQuad split) const;

    // attribute access
    inline const QPointF position() const;
    inline const QPointF middle() const;
    inline const QPointF end() const;
    inline const QPointF size() const;

    inline bool contains(const QPointF &p) const;
    inline bool contains(const GLaabb &o) const;
    inline bool intersects(const GLaabb &o) const;

    // Cut: returns the AABB defined as the shared area
    // between the two given AABBs, or an empty AABB
    // if no area is shared.
    // ie. C = A & B
    inline const GLaabb cut(const GLaabb &o) const;

    // Join: returns the AABB defined as the smallest AABB that
    // contains both given AABBs (may be disjoint).
    // ie. C = A | B
    inline const GLaabb join(const GLaabb &o) const;

    //NOTE consider use a QRectF instead
    qreal x;
    qreal y;
    qreal width;
    qreal height;
};

inline bool fuzzyEqual(const GLaabb &b0, const GLaabb &b1);
inline bool fuzzyNotEqual(const GLaabb &b0, const GLaabb &b1);

inline bool operator ==(const GLaabb &b0, const GLaabb &b1);
inline bool operator !=(const GLaabb &b0, const GLaabb &b1);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

inline GLaabb::GLaabb()
    : x(0),
      y(0),
      width(0),
      height(0)
{

}

inline GLaabb::~GLaabb()
{

}

inline GLaabb::GLaabb(const qreal x, const qreal y, const qreal width, const qreal height)
    : x(x),
      y(y),
      width(width),
      height(height)
{

}

inline GLaabb::GLaabb(const QPointF &p, const QSizeF &size)
    : x(p.x()),
      y(p.y()),
      width(size.width()),
      height(size.height())
{

}

inline GLaabb::GLaabb(const QRectF &rect)
    : x( rect.topLeft().x() ),
      y( rect.topLeft().x() ),
      width( rect.size().width() ),
      height( rect.size().height() )
{

}

inline const GLaabb GLaabb::fromPoints(const QPointF &p0, const QPointF &p1)
{
    return GLaabb(std::min(p0.x(), p1.x()), std::min(p0.y(), p1.y()),
                  std::fabs(p1.x() - p0.x()), std::fabs(p1.y() - p0.y()));
}

inline const QRectF GLaabb::toRectangle(const GLaabb &b)
{
    const qreal hW = 0.5f * b.width;
    const qreal hH = 0.5f * b.height;
    return QRectF( QPointF(b.x + hW, b.y+ hH ),
                          QPointF(b.width, b.height));
}

const GLaabb GLaabb::split(SplitHalf split) const
{
    const qreal fW = width;
    const qreal hW = 0.5f * width;
    const qreal fH = height;
    const qreal hH = 0.5f * height;

    switch (split) {
    case H0: return GLaabb(x + 0, y + 0, fW, hH); break;
    case H1: return GLaabb(x + 0, y + hH, fW, hH); break;
    case V0: return GLaabb(x + 0, y + 0, hW, fH); break;
    case V1: return GLaabb(x + hW, y + 0, hW, fH); break;
    }

    return GLaabb();
}

const GLaabb GLaabb::split(SplitQuad split) const
{
    const qreal hW = 0.5f * width;
    const qreal hH = 0.5f * height;

    switch (split) {
    case Q0: return GLaabb(x + hW, y + hH, hW, hH); break;
    case Q1: return GLaabb(x + 0, y + hH, hW, hH); break;
    case Q2: return GLaabb(x + 0, y + 0, hW, hH); break;
    case Q3: return GLaabb(x + hW, y + 0, hW, hH); break;
    }

    return GLaabb();
}

inline const QPointF GLaabb::position() const
{
    return QPointF(x, y);
}

inline const QPointF GLaabb::middle() const
{
    return QPointF(x + (0.5f * width), y + (0.5f * height));
}

inline const QPointF GLaabb::end() const
{
    return QPointF(x + width, y + height);
}

inline const QPointF GLaabb::size() const
{
    return QPointF(width, height);
}

bool GLaabb::contains(const QPointF &p) const
{
    const QRectF rectangle( QPointF(x,y) , QPointF(width, height) ) ;
    return rectangle.contains(p);
}

inline bool GLaabb::contains(const GLaabb &o) const
{
    return (
               (x <= o.x) && ((o.x + o.width) <= (x + width)) &&
               (y <= o.y) && ((o.y + o.height) <= (y + height))
           );
}

inline bool GLaabb::intersects(const GLaabb &o) const
{
    // simple SAT (Separating Axis Theorem) approach
    return !(
               (x >= (o.x + o.width)) || (y >= (o.y + o.height)) ||
               ((x + width) <= o.x) || ((y + height) <= o.y)
           );
}

const GLaabb GLaabb::cut(const GLaabb &o) const
{
    if (intersects(o)) {
        const QPointF p0 = GL::max(position(), o.position());
        const QPointF p1 = GL::min(end(), o.end());
        return GLaabb::fromPoints(p0, p1);
    } else {
        return GLaabb(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

const GLaabb GLaabb::join(const GLaabb &o) const
{
    const QPointF p0 = GL::min(position(), o.position());
    const QPointF p1 = GL::max(end(), o.end());
    return GLaabb::fromPoints(p0, p1);
}

inline bool fuzzyEqual(const GLaabb &b0, const GLaabb &b1)
{
    return qFuzzyCompare(b0.x, b1.x)
           && qFuzzyCompare(b0.y, b1.y)
           && qFuzzyCompare(b0.width, b1.width)
           && qFuzzyCompare(b0.height, b1.height);

}

inline bool fuzzyNotEqual(const GLaabb &b0, const GLaabb &b1)
{
    return qFuzzyCompare(b0.x, b1.x)
           || qFuzzyCompare(b0.y, b1.y)
           || qFuzzyCompare(b0.width, b1.width)
           || qFuzzyCompare(b0.height, b1.height);
}

inline bool operator ==(const GLaabb &b0, const GLaabb &b1)
{
    return (b0.x == b1.x)
           && (b0.y == b1.y)
           && (b0.width == b1.width)
           && (b0.height == b1.height);
}

inline bool operator !=(const GLaabb &b0, const GLaabb &b1)
{
    return (b0.x != b1.x)
           || (b0.y != b1.y)
           || (b0.width != b1.width)
           || (b0.height != b1.height);
}

} // namespace GL //

#endif // GLAABB_H //

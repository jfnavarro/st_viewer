/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLAABB_H
#define GLAABB_H

#include "GLCommon.h"
#include "math/GLMath.h"

namespace GL
{
    // Simple implementation of an Axis Aligned Bounding Box
    // The AABB provides simple functionality for testing collisions and
    // intersections. In addition convenience functions for splitting the
    // AABB is provided to simplify implementations of for instance quad trees.
    struct GLaabb
    {
        inline GLaabb();
        inline GLaabb(const GLfloat x, const GLfloat y, const GLfloat width, const GLfloat height);
        inline GLaabb(const GLpoint &p, const GLpoint &size);

        // create an aabb between two given points
        static inline const GLaabb fromPoints(const GLpoint &p0, const GLpoint &p1);
        // create a rectangle from the given aabb
        static inline const GLrectangle toRectangle(const GLaabb &b);

        // split aabb into a smaller version

        // SplitHalf: splits the aabb in half
        //             +----+ Vx - splits vertically
        // +---------+ ¦ H0 ¦ Hx - splits horizontally
        // ¦ V0 ¦ V1 ¦ ¦----¦ where x indicates the segment number as depicted.
        // +---------+ ¦ H1 ¦
        //             +----+
        enum SplitHalf { H0, H1, V0, V1 };
        inline const GLaabb split(SplitHalf split) const;
        // SplitQuad: splits the aabb in four equally large segments
        // +---------+ Qx - splits into quads
        // ¦ Q1 ¦ Q0 ¦ where x indicates the segment number as depicted.
        // ¦----+----¦
        // ¦ Q2 ¦ Q3 ¦
        // +---------+
        enum SplitQuad { Q0 = 0x00, Q1 = 0x01, Q2 = 0x02, Q3 = 0x03 };
        inline const GLaabb split(SplitQuad split) const;

        // attribute access
        inline const GLpoint position() const;
        inline const GLpoint middle() const;
        inline const GLpoint end() const;
        inline const GLpoint size() const;

        inline const bool contains(const GLpoint &p) const;
        inline const bool contains(const GLaabb &o) const;
        inline const bool intersects(const GLaabb &o) const;

        // +---+-----+
        // ¦   ¦  A  ¦ Cut: returns the AABB defined as the shared area
        // +---¦-+   ¦      between the two given AABBs, or an empty AABB
        // ¦   ¦C¦   ¦      if no area is shared.
        // ¦   +-+---¦      ie. C = A & B
        // ¦  B  ¦   ¦
        // +-----+---+
        inline const GLaabb cut(const GLaabb &o) const;
        // +-----+---+
        // ¦ C   ¦ A ¦ Join: returns the AABB defined as the smallest AABB that
        // ¦     +---+       contains both given AABBs (may be disjoint).
        // +---+     ¦       ie. C = A | B
        // ¦ B ¦     ¦
        // +---+-----+
        inline const GLaabb join(const GLaabb &o) const;

        GLfloat x, y;
        GLfloat width, height;
    };

    inline const bool fuzzyEqual(const GLaabb &b0, const GLaabb &b1, GLfloat e = EPSILON);
    inline const bool fuzzyNotEqual(const GLaabb &b0, const GLaabb &b1, GLfloat e = EPSILON);

    inline const bool operator ==(const GLaabb &b0, const GLaabb &b1);
    inline const bool operator !=(const GLaabb &b0, const GLaabb &b1);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

    inline GLaabb::GLaabb()
        : x(0.0f), y(0.0f), width(0.0f), height(0.0f)
    {

    }
    inline GLaabb::GLaabb(const GLfloat x, const GLfloat y, const GLfloat width, const GLfloat height)
        : x(x), y(y), width(width), height(height)
    {

    }
    inline GLaabb::GLaabb(const GLpoint &p, const GLpoint &size)
        : x(p.x), y(p.y), width(size.width), height(size.height)
    {

    }

    inline const GLaabb GLaabb::fromPoints(const GLpoint &p0, const GLpoint &p1)
    {
        return GLaabb(min(p0.x, p1.x), min(p0.y, p1.y), fabs(p1.x - p0.x), fabs(p1.y - p0.y));
    }
    inline const GLrectangle GLaabb::toRectangle(const GLaabb &b)
    {
        const GLfloat hW = 0.5f * b.width;
        const GLfloat hH = 0.5f * b.height;
        return GLrectangle(b.x + hW, b.y + hH, b.width, b.height);
    }

    inline const GLaabb GLaabb::split(SplitHalf split) const
    {
        const GLfloat fW = width, hW = 0.5f * width;
        const GLfloat fH = height, hH = 0.5f * height;
        switch (split)
        {
        case H0: return GLaabb(x + 0, y + 0, fW, hH); break;
        case H1: return GLaabb(x + 0, y + hH, fW, hH); break;
        case V0: return GLaabb(x + 0, y + 0, hW, fH); break;
        case V1: return GLaabb(x + hW, y + 0, hW, fH); break;
        }
        return GLaabb();
    }
    inline const GLaabb GLaabb::split(SplitQuad split) const
    {
        const GLfloat hW = 0.5f * width;
        const GLfloat hH = 0.5f * height;
        switch (split)
        {
        case Q0: return GLaabb(x + hW, y + hH, hW, hH); break;
        case Q1: return GLaabb(x + 0, y + hH, hW, hH); break;
        case Q2: return GLaabb(x + 0, y + 0, hW, hH); break;
        case Q3: return GLaabb(x + hW, y + 0, hW, hH); break;
        }
        return GLaabb();
    }

    inline const GLpoint GLaabb::position() const
    {
        return GLpoint(x, y);
    }
    inline const GLpoint GLaabb::middle() const
    {
        return GLpoint(x + (0.5f * width), y + (0.5f * height));
    }
    inline const GLpoint GLaabb::end() const
    {
        return GLpoint(x + width, y + height);
    }
    inline const GLpoint GLaabb::size() const
    {
        return GLpoint(width, height);
    }

    inline const bool GLaabb::contains(const GLpoint &p) const
    {
        typedef range<GLfloat, comp_op_ge<GLfloat>, comp_op_le<GLfloat> > range_t;
        return range_t::compare(p.x, x, x + width) && range_t::compare(p.y, y, y + height);
    }
    inline const bool GLaabb::contains(const GLaabb &o) const
    {
        return (
            (x <= o.x) && ((o.x + o.width) <= (x + width)) &&
            (y <= o.y) && ((o.y + o.height) <= (y + height))
        );
    }
    inline const bool GLaabb::intersects(const GLaabb &o) const
    {
        // simple SAT (Separating Axis Theorem) approach
        return !(
            (x >= (o.x + o.width)) || (y >= (o.y + o.height)) ||
            ((x + width) <= o.x) || ((y + height) <= o.y)
        );
    }

    inline const GLaabb GLaabb::cut(const GLaabb &o) const
    {
        if (intersects(o))
        {
            const GLpoint p0 = GL::max(position(), o.position());
            const GLpoint p1 = GL::min(end(), o.end());
            return GLaabb::fromPoints(p0, p1);
        }
        else
        {
            return GLaabb(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    inline const GLaabb GLaabb::join(const GLaabb &o) const
    {
        const GLpoint p0 = GL::min(position(), o.position());
        const GLpoint p1 = GL::max(end(), o.end());
        return GLaabb::fromPoints(p0, p1);
    }

    inline const bool fuzzyEqual(const GLaabb &b0, const GLaabb &b1, GLfloat e)
    {
        return fuzzyEqual(b0.x, b1.x, e)
            && fuzzyEqual(b0.y, b1.y, e)
            && fuzzyEqual(b0.width, b1.width, e)
            && fuzzyEqual(b0.height, b1.height, e);

    }
    inline const bool fuzzyNotEqual(const GLaabb &b0, const GLaabb &b1, GLfloat e)
    {
        return fuzzyNotEqual(b0.x, b1.x, e)
            || fuzzyNotEqual(b0.y, b1.y, e)
            || fuzzyNotEqual(b0.width, b1.width, e)
            || fuzzyNotEqual(b0.height, b1.height, e);
    }

    inline const bool operator ==(const GLaabb &b0, const GLaabb &b1)
    {
        return (b0.x == b1.x)
            && (b0.y == b1.y)
            && (b0.width == b1.width)
            && (b0.height == b1.height);
    }

    inline const bool operator !=(const GLaabb &b0, const GLaabb &b1)
    {
        return (b0.x != b1.x)
            || (b0.y != b1.y)
            || (b0.width != b1.width)
            || (b0.height != b1.height);
    }

} // namespace GL //

#endif // GLAABB_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef QUADTREEAABB_H
#define QUADTREEAABB_H_H

#include "Common.h"

#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <cmath>

// Simple implementation of an Axis Aligned Bounding Box
// The AABB provides simple functionality for testing collisions and
// intersections. In addition convenience functions for splitting the
// AABB is provided to simplify implementations of for instance quad trees.
struct QuadTreeAABB {

    QuadTreeAABB();
    virtual ~QuadTreeAABB();
    QuadTreeAABB(const qreal x, const qreal y, const qreal width, const qreal height);
    QuadTreeAABB(const QPointF &p, const QSizeF &size);
    QuadTreeAABB(const QRectF &rect);

    // create an QuadTreeAABB between two given points
    static const QuadTreeAABB fromPoints(const QPointF &p0, const QPointF &p1);

    // create a rectangle from the given QuadTreeAABB
    static const QRectF toRectangle(const QuadTreeAABB &b);

    // SplitHalf: splits the QuadTreeAABB in half
    enum SplitHalf { H0, H1, V0, V1 };
    const QuadTreeAABB split(SplitHalf split) const;

    // SplitQuad: splits the QuadTreeAABB in four equally large segments
    enum SplitQuad { Q0 = 0x00, Q1 = 0x01, Q2 = 0x02, Q3 = 0x03 };
    const QuadTreeAABB split(SplitQuad split) const;

    // attribute access
    const QPointF position() const;
    const QPointF middle() const;
    const QPointF end() const;
    const QPointF size() const;

    bool contains(const QPointF &p) const;
    bool contains(const QuadTreeAABB &o) const;
    bool intersects(const QuadTreeAABB &o) const;

    // Cut: returns the AABB defined as the shared area
    // between the two given AABBs, or an empty AABB
    // if no area is shared.
    // ie. C = A & B
    const QuadTreeAABB cut(const QuadTreeAABB &o) const;

    // Join: returns the AABB defined as the smallest AABB that
    // contains both given AABBs (may be disjoint).
    // ie. C = A | B
    const QuadTreeAABB join(const QuadTreeAABB &o) const;

    //NOTE consider use a QRectF instead
    qreal x;
    qreal y;
    qreal width;
    qreal height;
};

bool fuzzyEqual(const QuadTreeAABB &b0, const QuadTreeAABB &b1);
bool fuzzyNotEqual(const QuadTreeAABB &b0, const QuadTreeAABB &b1);

bool operator ==(const QuadTreeAABB &b0, const QuadTreeAABB &b1);
bool operator !=(const QuadTreeAABB &b0, const QuadTreeAABB &b1);

#endif // QUADTREEAABB_H_H //

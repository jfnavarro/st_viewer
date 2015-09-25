/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#ifndef STCOLOREDQUADS_H
#define STCOLOREDQUADS_H

#include <QLineF>
#include <QColor>
#include <QPolygonF>
#include <vector>
#include <cstdint>

// Stores a set of colored 2D quads (rectangles). There is no Qt quad type, so here we use quad
// and rectangle interchangeably. Each quad has its own color.
//
// The default object contains no quads.
//
// ColoredQuads objects are safe to copy and manage no OpenGL state themselves.
class ColoredQuads
{
public:
    // Adds a new quad q0 to the set. Empty, invalid and null quads are not culled, because the
    // user may be tracking quads by their indices.
    void addQuad(const QRectF& q0, const QColor& color);

    // Sets the existing quad at index n to be q0. If there is no quad at this index, a
    // std::out_of_range exception is thrown.
    void setQuad(const unsigned int n, const QRectF& q0, const QColor& color);

    // Returns a copy of the indexed quad, or throws std::out_of_range exception if there is no
    // quad at that index.
    QRectF quad(const unsigned int n) const;

    // Returns the color of the indexed quad, or throws std::out_of_range exception if there is no
    // quad at that index.
    QColor quadColor(const unsigned int n) const;

    // Returns the number of quads stored in the set.
    unsigned int quadCount() const;

    // Resets the number of stored quads to zero.
    void clear();

    bool operator==(const ColoredQuads& rhs) const;
    bool operator!=(const ColoredQuads& rhs) const;

private:
    friend class Renderer;

    // Each quad is stored as 2 x 2D triangles = 2 x 3 x 2 = 12.
    std::vector<float> m_quads;

    // Each triangle stores one BGRA = 2 x 3 x 4 = 24.
    std::vector<std::uint8_t> m_quadColors;
};

#endif

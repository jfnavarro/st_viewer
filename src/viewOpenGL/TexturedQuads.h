/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#ifndef STTEXTUREDQUADS_H
#define STTEXTUREDQUADS_H

#include "ColoredQuads.h"
#include <QLineF>
#include <QColor>
#include <array>
#include <vector>
#include <cstdint>

// Stores a set of colored 2D quads (rectangles) and their 2D texture (UV) coordinates. There is
// no Qt quad type, so here we use the word quad and rectangle interchangeably. Each quad has its
// own color and texture coordinates.
//
// Alpha blending is supported (0 is fully transparent, 255 is fully opaque).
//
// The default object contains no quads.
//
// STTexturedQuads objects are safe to copy and manage no OpenGL state themselves.
class TexturedQuads
{
public:
    // An array of clockwise UV coordinates. Coordinate 0 corresponds to QRectF topLeft,
    // coordinate 1 is topRight, 2 is bottomRight and 3 is bottomLeft.
    typedef std::array<QPointF, 4> RectUVCoords;

    // Adds a new quad q0 with the texture coords uvCoords to the set. Empty, invalid and null
    // quads or texture coordinate quads are not culled, because the user may be tracking quads by
    // their indices.
    void addQuad(const QRectF& q0, const RectUVCoords& uvCoords, const QColor& color);

    // Sets the existing quad at index n to be q0 with texture coords uvCoords. If there is no quad
    // at this index, a std::out_of_range exception is thrown.
    void setQuad(const unsigned int n,
                 const QRectF& q0,
                 const RectUVCoords& uvCoords,
                 const QColor& color);

    // Returns a copy of the indexed quad, or throws std::out_of_range exception if there is no
    // quad at that index.
    QRectF quad(const unsigned int n) const;

    // Returns the texture coordinates of the indexed quad, or throws std::out_of_range exception
    // if there is no quad at that index.
    RectUVCoords quadTextureCoords(const unsigned int n) const;

    // Returns the color of the indexed quad, or throws std::out_of_range exception if there is no
    // quad at that index.
    QColor quadColor(const unsigned int n) const;

    // Returns the number of quads stored in the set.
    unsigned int quadCount() const;

    // Resets the number of stored quads to zero.
    void clear();

    // Static function returning the texture coords for the full texture:
    // [0] = topLeft     (0,0)
    // [1] = topRight    (1,0) 
    // [2] = bottomRight (1,1)
    // [3] = bottomLeft  (0,1)
    static RectUVCoords defaultTextureCoords();

    bool operator==(const TexturedQuads& rhs) const;
    bool operator!=(const TexturedQuads& rhs) const;

private:
    friend class Renderer;

    ColoredQuads m_quads;

    // Each quad stores 12 UV coordinate, as 2 x 2D triangles = 2 x 3 x 2 (U and V) = 12.
    std::vector<float> m_textureCoords;
};

#endif

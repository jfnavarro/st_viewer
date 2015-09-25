/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "STTexturedQuads.h"

void STTexturedQuads::addQuad(const QRectF& q0,
                              const STTexturedQuads::RectUVCoords& uvCoords,
                              const QColor& color)
{
    // This will throw if we cannot allocate space for the new quad data.
    m_textureCoords.reserve(m_textureCoords.size() + 12u);

    m_quads.addQuad(q0, color);

    // Triangle A
    m_textureCoords.push_back(uvCoords[0].x());
    m_textureCoords.push_back(uvCoords[0].y());
    m_textureCoords.push_back(uvCoords[1].x());
    m_textureCoords.push_back(uvCoords[1].y());
    m_textureCoords.push_back(uvCoords[3].x());
    m_textureCoords.push_back(uvCoords[3].y());

    // Triangle B
    m_textureCoords.push_back(uvCoords[3].x());
    m_textureCoords.push_back(uvCoords[3].y());
    m_textureCoords.push_back(uvCoords[1].x());
    m_textureCoords.push_back(uvCoords[1].y());
    m_textureCoords.push_back(uvCoords[2].x());
    m_textureCoords.push_back(uvCoords[2].y());
}

void STTexturedQuads::setQuad(const unsigned int n,
                              const QRectF& q0,
                              const STTexturedQuads::RectUVCoords& uvCoords,
                              const QColor& color)
{
    m_quads.setQuad(n, q0, color);

    const size_t quadIndex = n * 12u;

    // Triangle A
    m_textureCoords.at(quadIndex + 0u) = uvCoords[0].x();
    m_textureCoords.at(quadIndex + 1u) = uvCoords[0].y();
    m_textureCoords.at(quadIndex + 2u) = uvCoords[1].x();
    m_textureCoords.at(quadIndex + 3u) = uvCoords[1].y();
    m_textureCoords.at(quadIndex + 4u) = uvCoords[3].x();
    m_textureCoords.at(quadIndex + 5u) = uvCoords[3].y();

    // Triangle B
    m_textureCoords.at(quadIndex + 6u) = uvCoords[3].x();
    m_textureCoords.at(quadIndex + 7u) = uvCoords[3].y();
    m_textureCoords.at(quadIndex + 8u) = uvCoords[1].x();
    m_textureCoords.at(quadIndex + 8u) = uvCoords[1].y();
    m_textureCoords.at(quadIndex + 10u) = uvCoords[2].x();
    m_textureCoords.at(quadIndex + 11u) = uvCoords[2].y();
}

STTexturedQuads::RectUVCoords STTexturedQuads::quadTextureCoords(const unsigned int n) const
{
    const size_t quadIndex = n * 12u;

    const auto topLeftX = m_textureCoords.at(quadIndex + 0u);
    const auto topLeftY = m_textureCoords.at(quadIndex + 1u);

    const auto topRightX = m_textureCoords.at(quadIndex + 2u);
    const auto topRightY = m_textureCoords.at(quadIndex + 3u);

    const auto bottomLeftX = m_textureCoords.at(quadIndex + 4u);
    const auto bottomLeftY = m_textureCoords.at(quadIndex + 5u);

    const auto bottomRightX = m_textureCoords.at(quadIndex + 10u);
    const auto bottomRightY = m_textureCoords.at(quadIndex + 11u);

    const QPointF topLeft(topLeftX, topLeftY);
    const QPointF topRight(topRightX, topRightY);
    const QPointF bottomRight(bottomRightX, bottomRightY);
    const QPointF bottomLeft(bottomLeftX, bottomLeftY);

    STTexturedQuads::RectUVCoords coords;

    coords[0] = topLeft;
    coords[1] = topRight;
    coords[2] = bottomRight;
    coords[3] = bottomLeft;

    return coords;
}

QRectF STTexturedQuads::quad(const unsigned int n) const
{
    return m_quads.quad(n);
}

QColor STTexturedQuads::quadColor(const unsigned int n) const
{
    return m_quads.quadColor(n);
}

unsigned int STTexturedQuads::quadCount() const
{
    return m_quads.quadCount();
}

void STTexturedQuads::clear()
{
    m_quads.clear();
    m_textureCoords.clear();
}

STTexturedQuads::RectUVCoords STTexturedQuads::defaultTextureCoords()
{
    STTexturedQuads::RectUVCoords uvCoords;

    uvCoords[0] = QPointF(0.0, 0.0);
    uvCoords[1] = QPointF(1.0, 0.0);
    uvCoords[2] = QPointF(1.0, 1.0);
    uvCoords[3] = QPointF(0.0, 1.0);

    return uvCoords;
}

bool STTexturedQuads::operator==(const STTexturedQuads& rhs) const
{
    return m_quads == rhs.m_quads && m_textureCoords == rhs.m_textureCoords;
}

bool STTexturedQuads::operator!=(const STTexturedQuads& rhs) const
{
    return !(*this == rhs);
}

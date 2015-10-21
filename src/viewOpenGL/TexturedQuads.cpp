/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "TexturedQuads.h"

void TexturedQuads::addQuad(const QRectF& q0,
                              const TexturedQuads::RectUVCoords& uvCoords,
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

void TexturedQuads::setQuad(const unsigned int n,
                              const QRectF& q0,
                              const TexturedQuads::RectUVCoords& uvCoords,
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

TexturedQuads::RectUVCoords TexturedQuads::quadTextureCoords(const unsigned int n) const
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

    TexturedQuads::RectUVCoords coords;

    coords[0] = topLeft;
    coords[1] = topRight;
    coords[2] = bottomRight;
    coords[3] = bottomLeft;

    return coords;
}

QRectF TexturedQuads::quad(const unsigned int n) const
{
    return m_quads.quad(n);
}

QColor TexturedQuads::quadColor(const unsigned int n) const
{
    return m_quads.quadColor(n);
}

unsigned int TexturedQuads::quadCount() const
{
    return m_quads.quadCount();
}

void TexturedQuads::clear()
{
    m_quads.clear();
    m_textureCoords.clear();
}

TexturedQuads::RectUVCoords TexturedQuads::defaultTextureCoords()
{
    TexturedQuads::RectUVCoords uvCoords;

    uvCoords[0] = QPointF(0.0, 0.0);
    uvCoords[1] = QPointF(1.0, 0.0);
    uvCoords[2] = QPointF(1.0, 1.0);
    uvCoords[3] = QPointF(0.0, 1.0);

    return uvCoords;
}

bool TexturedQuads::operator==(const TexturedQuads& rhs) const
{
    return m_quads == rhs.m_quads && m_textureCoords == rhs.m_textureCoords;
}

bool TexturedQuads::operator!=(const TexturedQuads& rhs) const
{
    return !(*this == rhs);
}

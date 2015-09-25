/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ColoredQuads.h"

void ColoredQuads::addQuad(const QRectF& q0, const QColor& color)
{
    // These will throw if we cannot allocate space for the new quad data.
    m_quads.reserve(m_quads.size() + 12u);
    m_quadColors.reserve(m_quadColors.size() + 24u);

    // Triangle A
    m_quads.push_back(q0.topLeft().x());
    m_quads.push_back(q0.topLeft().y());
    m_quads.push_back(q0.topRight().x());
    m_quads.push_back(q0.topRight().y());
    m_quads.push_back(q0.bottomLeft().x());
    m_quads.push_back(q0.bottomLeft().y());

    // Triangle B
    m_quads.push_back(q0.bottomLeft().x());
    m_quads.push_back(q0.bottomLeft().y());
    m_quads.push_back(q0.topRight().x());
    m_quads.push_back(q0.topRight().y());
    m_quads.push_back(q0.bottomRight().x());
    m_quads.push_back(q0.bottomRight().y());

    for (unsigned int i = 0u; i < 6u; ++i) {
        m_quadColors.push_back(color.blue());
        m_quadColors.push_back(color.green());
        m_quadColors.push_back(color.red());
        m_quadColors.push_back(color.alpha());
    }
}

void ColoredQuads::setQuad(const unsigned int n, const QRectF& q0, const QColor& color)
{
    const size_t quadIndex = n * 12u;
    const size_t colrIndex = n * 24u;

    // Triangle A
    m_quads.at(quadIndex + 0u) = q0.topLeft().x();
    m_quads.at(quadIndex + 1u) = q0.topLeft().y();
    m_quads.at(quadIndex + 2u) = q0.topRight().x();
    m_quads.at(quadIndex + 3u) = q0.topRight().y();
    m_quads.at(quadIndex + 4u) = q0.bottomLeft().x();
    m_quads.at(quadIndex + 5u) = q0.bottomLeft().y();

    // Triangle B
    m_quads.at(quadIndex + 6u) = q0.bottomLeft().x();
    m_quads.at(quadIndex + 7u) = q0.bottomLeft().y();
    m_quads.at(quadIndex + 8u) = q0.topRight().x();
    m_quads.at(quadIndex + 8u) = q0.topRight().y();
    m_quads.at(quadIndex + 10u) = q0.bottomRight().x();
    m_quads.at(quadIndex + 11u) = q0.bottomRight().y();

    for (unsigned int i = 0u; i < 6u; ++i) {
        m_quadColors.at(colrIndex + (i * 4u) + 0u) = color.blue();
        m_quadColors.at(colrIndex + (i * 4u) + 1u) = color.green();
        m_quadColors.at(colrIndex + (i * 4u) + 2u) = color.red();
        m_quadColors.at(colrIndex + (i * 4u) + 3u) = color.alpha();
    }
}

QRectF ColoredQuads::quad(const unsigned int n) const
{
    const size_t quadIndex = n * 12u;

    const auto topLeftX = m_quads.at(quadIndex + 0u);
    const auto topLeftY = m_quads.at(quadIndex + 1u);

    const auto bottomRightX = m_quads.at(quadIndex + 10u);
    const auto bottomRightY = m_quads.at(quadIndex + 11u);

    const QPointF topLeft(topLeftX, topLeftY);
    const QPointF bottomRight(bottomRightX, bottomRightY);

    return QRectF(topLeft, bottomRight);
}

QColor ColoredQuads::quadColor(const unsigned int n) const
{
    const size_t colrIndex = n * 24u;

    const auto blue = m_quadColors.at(colrIndex + 0u);
    const auto green = m_quadColors.at(colrIndex + 1u);
    const auto red = m_quadColors.at(colrIndex + 2u);
    const auto alpha = m_quadColors.at(colrIndex + 3u);

    return QColor(red, green, blue, alpha);
}

unsigned int ColoredQuads::quadCount() const
{
    return static_cast<unsigned int>(m_quads.size() / 12u);
}

void ColoredQuads::clear()
{
    m_quads.clear();
    m_quadColors.clear();
}

bool ColoredQuads::operator==(const ColoredQuads& rhs) const
{
    return m_quads == rhs.m_quads && m_quadColors == rhs.m_quadColors;
}

bool ColoredQuads::operator!=(const ColoredQuads& rhs) const
{
    return !(*this == rhs);
}

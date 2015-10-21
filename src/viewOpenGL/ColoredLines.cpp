/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ColoredLines.h"

ColoredLines::ColoredLines()
    : m_lines()
    , m_colors()
{
}

void ColoredLines::addRectOutline(const QRectF& rect, const QColor& color)
{
    addLine(QLineF(rect.topLeft(), rect.topRight()), color);
    addLine(QLineF(rect.topRight(), rect.bottomRight()), color);
    addLine(QLineF(rect.bottomRight(), rect.bottomLeft()), color);
    addLine(QLineF(rect.bottomLeft(), rect.topLeft()), color);
}

void ColoredLines::addLine(const QLineF& line, const QColor& color)
{
    // Check that the new elements can be added before we change the contents.
    m_lines.reserve(m_lines.size() + 4u);
    m_colors.reserve(m_colors.size() + 8u);

    m_lines.push_back(line.p1().x());
    m_lines.push_back(line.p1().y());
    m_lines.push_back(line.p2().x());
    m_lines.push_back(line.p2().y());

    m_colors.push_back(static_cast<std::uint8_t>(color.blue()));
    m_colors.push_back(static_cast<std::uint8_t>(color.green()));
    m_colors.push_back(static_cast<std::uint8_t>(color.red()));
    m_colors.push_back(static_cast<std::uint8_t>(color.alpha()));

    m_colors.push_back(static_cast<std::uint8_t>(color.blue()));
    m_colors.push_back(static_cast<std::uint8_t>(color.green()));
    m_colors.push_back(static_cast<std::uint8_t>(color.red()));
    m_colors.push_back(static_cast<std::uint8_t>(color.alpha()));
}

void ColoredLines::setLine(const unsigned int n, const QLineF& line, const QColor& color)
{
    const size_t lineIndex = n * 4u;
    const size_t colrIndex = n * 8u;

    m_lines.at(lineIndex + 0u) = line.p1().x();
    m_lines.at(lineIndex + 1u) = line.p1().y();
    m_lines.at(lineIndex + 2u) = line.p2().x();
    m_lines.at(lineIndex + 3u) = line.p2().y();

    m_colors.at(colrIndex + 0u) = static_cast<std::uint8_t>(color.blue());
    m_colors.at(colrIndex + 1u) = static_cast<std::uint8_t>(color.green());
    m_colors.at(colrIndex + 2u) = static_cast<std::uint8_t>(color.red());
    m_colors.at(colrIndex + 3u) = static_cast<std::uint8_t>(color.alpha());

    m_colors.at(colrIndex + 4u) = static_cast<std::uint8_t>(color.blue());
    m_colors.at(colrIndex + 5u) = static_cast<std::uint8_t>(color.green());
    m_colors.at(colrIndex + 6u) = static_cast<std::uint8_t>(color.red());
    m_colors.at(colrIndex + 7u) = static_cast<std::uint8_t>(color.alpha());
}

QLineF ColoredLines::line(const unsigned int n) const
{
    const size_t index = n * 4u;
    return QLineF(m_lines.at(index + 0u),
                  m_lines.at(index + 1u),
                  m_lines.at(index + 2u),
                  m_lines.at(index + 3u));
}

QColor ColoredLines::lineColor(const unsigned int n) const
{
    const size_t index = n * 8u;
    const int b = m_colors.at(index + 0u);
    const int g = m_colors.at(index + 1u);
    const int r = m_colors.at(index + 2u);
    const int a = m_colors.at(index + 3u);
    return QColor(r, g, b, a);
}

unsigned int ColoredLines::lineCount() const
{
    return static_cast<unsigned int>(m_lines.size() / 4u);
}

void ColoredLines::clear()
{
    m_lines.clear();
    m_colors.clear();
}

bool ColoredLines::operator==(const ColoredLines& rhs) const
{
    return m_lines == rhs.m_lines && m_colors == rhs.m_colors;
}

bool ColoredLines::operator!=(const ColoredLines& rhs) const
{
    return !(*this == rhs);
}

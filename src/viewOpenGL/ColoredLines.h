/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#ifndef STCOLOREDLINES_H
#define STCOLOREDLINES_H

#include <QLineF>
#include <QRectF>
#include <QColor>
#include <vector>
#include <cstdint>

// A set of 2D colored OpenGL lines. Each managed line has a head and a tail QPointF, and its own
// color.
// Colors are stored as unsigned bytes internally.
// Alpha blending is supported (0 is fully transparent, 255 is fully opaque).
// Lines are zero indexed.
// The default constructed object contains no lines.
// ColoredLines objects are safe to copy and manage no OpenGL state themselves.
class ColoredLines
{
public:
    ColoredLines();

    // Adds the 4 lines making up the outline of the given rect, topLeft->topRight, topRight to
    // bottomRight, bottomRight to bottomLeft, and bottomLeft to topLeft.
    void addRectOutline(const QRectF& rect, const QColor& color);

    // Appends a new colored line to the set of lines.
    void addLine(const QLineF& line, const QColor& color);

    // If the lines n exists, updates its head and tail points and its color. If the line n
    // does not exist, throws a std::out_of_range exception.
    void setLine(const unsigned int n, const QLineF& line, const QColor& color);

    // Allows the user to set a thicker or thinner line. Corresponds to the OpenGL line
    // width (see glLineWidth). The default width is 1.0. All lines in this set will be rendered
    // with this thickness. lineWidth must be greater than zero.
    void setLineWidth(const float lineWidth);

    // Returns the thickness the lines will be drawn with.
    float getLineWidth() const;

    // Returns the line n. If the line n does not exist, throws a std::out_of_range exception.
    QLineF line(const unsigned int n) const;

    // Returns the color of line n. If the line n does not exist, throws a
    // std::out_of_range exception.
    QColor lineColor(const unsigned int n) const;

    // Returns the number of lines managed by the object. Initially this is zero lines.
    unsigned int lineCount() const;

    // Resets the number of lines to zero.
    void clear();

    bool operator==(const ColoredLines& rhs) const;
    bool operator!=(const ColoredLines& rhs) const;

private:
    friend class Renderer;

    // Passed to glLineWidth.
    float m_lineWidth;

    // Lines are stored as 2 x 2 floats, {head{x,y}, tail{x,y}}
    std::vector<float> m_lines;

    // Colors are stored as 1 x 4 unsigned bytes {b,g,r,a}
    std::vector<std::uint8_t> m_colors;
};

#endif

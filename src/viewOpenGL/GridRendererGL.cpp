/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#include "GridRendererGL.h"

#include <QVector2D>
#include "qopengl.h"
#include "math/Common.h"
#include "utils/Utils.h"

GridRendererGL::GridRendererGL(QObject* parent)
    : GraphicItemGL(parent)
    , m_gridLines()
    , m_rect()
    , m_border()
    , m_centerColor(Qt::darkGreen)
    , m_borderColor(Qt::darkRed)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, false);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, false);
}

GridRendererGL::~GridRendererGL()
{
}

void GridRendererGL::doDraw(Renderer& renderer)
{
    drawLines(renderer, m_gridLines);
}

void GridRendererGL::createGridlines()
{
    const qreal GRID_LINE_SIZE = 1.0;

    m_gridLines.clear();

    // TODO: BORDERS STEP IN 1.0 AND LINES STEP IN GRID_LINE_SIZE WHICH IS ALSO 1.0.
    // IS THERE A REQUIREMENT THESE ARE THE SAME VALUE?

    // TODO: WHAT ARE THE CONSTRAINTS ON THE BORDER AND THE "RECT"? DO THEY IMPOSE
    // REQUIREMENTS ON ONE ANOTHER? DOES ONE NEED TO BE SMALLER THAN THE OTHER?

    const bool hasBorder = !m_border.isEmpty();
    const bool hasCentre = !m_rect.isEmpty();

    // Generate borders
    if (hasBorder && hasCentre) {
        for (qreal y = m_border.top(); y <= m_border.bottom(); y += 1.0) {
            if (m_rect.top() <= y && y <= m_rect.bottom()) {
                m_gridLines.addLine(QLineF(m_border.left(), y, m_rect.left(), y), m_borderColor);
                m_gridLines.addLine(QLineF(m_rect.right(), y, m_border.right(), y), m_borderColor);
            } else {
                m_gridLines.addLine(QLineF(m_border.left(), y, m_border.right(), y), m_borderColor);
            }
        }

        for (qreal x = m_border.left(); x <= m_border.right(); x += 1.0) {
            if (m_rect.left() <= x && x <= m_rect.right()) {
                m_gridLines.addLine(QLineF(x, m_border.top(), x, m_rect.top()), m_borderColor);
                m_gridLines.addLine(QLineF(x, m_rect.bottom(), x, m_border.bottom()),
                                    m_borderColor);
            } else {
                m_gridLines.addLine(QLineF(x, m_border.top(), x, m_border.bottom()), m_borderColor);
            }
        }
    }

    // Generate grid
    if (hasCentre) {
        for (qreal y = m_rect.top(); y <= m_rect.bottom(); y += GRID_LINE_SIZE) {
            m_gridLines.addLine(QLineF(m_rect.left(), y, m_rect.right(), y), m_centerColor);
        }
        for (qreal x = m_rect.left(); x <= m_rect.right(); x += GRID_LINE_SIZE) {
            m_gridLines.addLine(QLineF(x, m_rect.top(), x, m_rect.bottom()), m_centerColor);
        }

        // check boundaries
        if (!qFuzzyCompare(STMath::qMod(m_rect.bottom() - m_rect.top(), GRID_LINE_SIZE), 0.0)) {
            m_gridLines.addLine(QLineF(m_rect.bottomLeft(), m_rect.bottomRight()), m_centerColor);
        }

        if (!qFuzzyCompare(STMath::qMod(m_rect.right() - m_rect.left(), GRID_LINE_SIZE), 0.0)) {
            m_gridLines.addLine(QLineF(m_rect.topRight(), m_rect.bottomRight()), m_centerColor);
        }
    }
}

void GridRendererGL::setDimensions(const QRectF& border, const QRectF& rect)
{
    m_border = border;
    m_rect = rect;
    createGridlines();
}

void GridRendererGL::slotSetGridColor(const QColor& color)
{
    if (m_centerColor != color) {
        m_centerColor = color;
        createGridlines();
        emit updated();
    }
}

QRectF GridRendererGL::border() const
{
    return m_border;
}

QRectF GridRendererGL::rectangle() const
{
    return m_rect;
}

QColor GridRendererGL::gridColor() const
{
    return m_centerColor;
}

QRectF GridRendererGL::boundingRect() const
{
    return m_border;
}

void GridRendererGL::setSelectionArea(const SelectionEvent*)
{
}

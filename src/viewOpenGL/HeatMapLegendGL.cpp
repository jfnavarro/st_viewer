/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#include "HeatMapLegendGL.h"
#include "ColoredLines.h"
#include "TexturedQuads.h"
#include "Renderer.h"
#include "math/Common.h"
#include "color/HeatMap.h"

#include <QPainter>
#include <QImage>
#include <QApplication>
#include <QVector2D>
#include <QLabel>

static const qreal legend_x = 0.0;
static const qreal legend_y = 0.0;
static const qreal legend_width = 25.0;
static const qreal legend_height = 150.0;
static const qreal bars_width = 35.0;

static const char HeatmapTextureName[] = "HEATMAP";

HeatMapLegendGL::HeatMapLegendGL(QObject* parent)
    : GraphicItemGL(parent)
    , m_maxReads(1)
    , m_minReads(1)
    , m_minGenes(1)
    , m_maxGenes(1)
    , m_colorComputingMode(Globals::LinearColor)
    , m_valueComputation(Reads)
    , m_isInitialized(false)
{
    setVisualOption(GraphicItemGL::Transformable, false);
    setVisualOption(GraphicItemGL::Visible, false);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, false);
}

HeatMapLegendGL::~HeatMapLegendGL()
{
    clearData();
}

void HeatMapLegendGL::clearData()
{
    m_valueComputation = Reads;
    m_maxReads = 1;
    m_minReads = 1;
    m_minGenes = 1;
    m_maxGenes = 1;
    m_isInitialized = false;
}

void HeatMapLegendGL::setSelectionArea(const SelectionEvent*)
{
}

void HeatMapLegendGL::setMinMaxValues(const int readsMin,
                                      const int readsMax,
                                      const int genesMin,
                                      const int genesMax)
{
    m_minReads = readsMin;
    m_maxReads = readsMax;
    m_minGenes = genesMin;
    m_maxGenes = genesMax;
}

void HeatMapLegendGL::setLowerLimitReads(const int limit)
{
    if (m_minReads != limit) {
        m_minReads = limit;
        generateHeatMap();
    }
}

void HeatMapLegendGL::setUpperLimitReads(const int limit)
{
    if (m_maxReads != limit) {
        m_maxReads = limit;
        generateHeatMap();
    }
}

void HeatMapLegendGL::setValueComputation(ValueComputation mode)
{
    if (m_valueComputation != mode) {
        m_valueComputation = mode;
        generateHeatMap();
    }
}

void HeatMapLegendGL::setColorComputingMode(const Globals::GeneColorMode& mode)
{
    // Update color computing mode
    if (m_colorComputingMode != mode) {
        m_colorComputingMode = mode;
        generateHeatMap();
    }
}

void HeatMapLegendGL::setLowerLimitGenes(const int limit)
{
    if (m_minGenes != limit) {
        m_minGenes = limit;
        generateHeatMap();
    }
}

void HeatMapLegendGL::setUpperLimitGenes(const int limit)
{
    if (m_maxGenes != limit) {
        m_maxGenes = limit;
        generateHeatMap();
    }
}

QRectF HeatMapLegendGL::heatmapCoordinates() const
{
    return QRectF(QPointF(legend_x, legend_y), QSizeF(legend_width, legend_height));
}

void HeatMapLegendGL::generateHeatMap()
{
    const int min = m_valueComputation == Reads ? m_minReads : m_minGenes;
    const int max = m_valueComputation == Reads ? m_maxReads : m_maxGenes;

    // Generate image texture
    std::unique_ptr<QImage> nextHeatmap(
        new QImage(legend_width, legend_height, QImage::Format_ARGB32));

    // Here we can choose the type of Spectrum (linear, log or exp) and
    // the type of color mapping (wavelength or linear interpolation)
    Heatmap::createHeatMapImage(*nextHeatmap, min, max, m_colorComputingMode);

    m_pendingHeatmap.swap(nextHeatmap);

    m_isInitialized = true;

    emit updated();
}

void HeatMapLegendGL::doDraw(Renderer& renderer)
{
    if (m_isInitialized) {

        if (m_pendingHeatmap) {
            renderer.addTexture(HeatmapTextureName, *m_pendingHeatmap, false);
            m_pendingHeatmap.reset();
        }

        drawHeatmap(renderer);
        drawHeatMapBorderLines(renderer);

        // draw text (add 5 pixels offset to the right)
        const int min = m_valueComputation == Reads ? m_minReads : m_minGenes;
        const int max = m_valueComputation == Reads ? m_maxReads : m_maxGenes;
        drawText(renderer, QPointF(legend_x + legend_width + 5, 0), QString::number(max));
        drawText(renderer,
                 QPointF(legend_x + legend_width + 5, legend_height),
                 QString::number(min));
    }
}

void HeatMapLegendGL::drawHeatmap(Renderer& renderer)
{
    TexturedQuads quad;
    quad.addQuad(heatmapCoordinates(), TexturedQuads::defaultTextureCoords(), Qt::white);
    drawTexturedQuads(renderer, quad, HeatmapTextureName);
}

void HeatMapLegendGL::drawHeatMapBorderLines(Renderer& renderer)
{
    ColoredLines borderLines;
    borderLines.addRectOutline(heatmapCoordinates(), Qt::white);
    drawLines(renderer, borderLines);
}

void HeatMapLegendGL::drawText(Renderer& renderer, const QPointF& posn, const QString& str)
{
    // Isn't this both a very expensive AND also usefully generic function?
    // TODO: Factor out?
    //
    const QFont monoFont("Courier", 12, QFont::Normal);
    const QFontMetrics metrics(monoFont);
    const QRect textRect = metrics.boundingRect(str);

    QImage image(textRect.size(), QImage::Format_ARGB32);
    image.fill(0);

    QPainter qpainter(&image);
    qpainter.setFont(monoFont);
    qpainter.setPen(Qt::white);
    qpainter.setRenderHint(QPainter::Antialiasing, true);
    qpainter.drawText(textRect.x(), metrics.ascent(), str);
    qpainter.end();

    const QPointF topLeft(posn.x() + textRect.x(), posn.y());
    const QPointF bottomRight(posn.x() + textRect.x() + textRect.width(),
                              posn.y() + metrics.ascent());

    TexturedQuads quads;
    quads.addQuad(QRectF(topLeft, bottomRight), TexturedQuads::defaultTextureCoords(), Qt::white);

    const QString textureName = "HMTEXT";
    renderer.addTexture(textureName, image.mirrored(), true);
    drawTexturedQuads(renderer, quads, textureName);
    renderer.removeTexture(textureName);
}

QRectF HeatMapLegendGL::boundingRect() const
{
    const QRectF coords = heatmapCoordinates();
    const QSizeF boundingSize(coords.width() + bars_width, coords.height());
    return QRectF(coords.topLeft(), boundingSize);
}

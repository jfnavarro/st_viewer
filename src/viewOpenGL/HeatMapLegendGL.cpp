/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HeatMapLegendGL.h"
#include "ColoredLines.h"
#include "STTexturedQuads.h"
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

HeatMapLegendGL::HeatMapLegendGL(QObject* parent)
    : GraphicItemGL(parent)
    , m_maxReads(1)
    , m_minReads(1)
    , m_minGenes(1)
    , m_maxGenes(1)
    , m_colorComputingMode(Globals::LinearColor)
    , m_textureText(QOpenGLTexture::Target2D)
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
    if (m_textureText.isCreated()) {
        m_textureText.destroy();
    }

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
            renderer.addTexture("heatmap", *m_pendingHeatmap, false);
            m_pendingHeatmap.reset();
        }

        drawHeatmap(renderer);
        drawHeatMapBorderLines(renderer);

        // draw text (add 5 pixels offset to the right)
        const int min = m_valueComputation == Reads ? m_minReads : m_minGenes;
        const int max = m_valueComputation == Reads ? m_maxReads : m_maxGenes;
        drawText(QPointF(legend_x + legend_width + 5, 0), QString::number(max));
        drawText(QPointF(legend_x + legend_width + 5, legend_height), QString::number(min));
    }
}

void HeatMapLegendGL::drawHeatmap(Renderer& renderer)
{
    STTexturedQuads quad;
    quad.addQuad(heatmapCoordinates(), STTexturedQuads::defaultTextureCoords(), Qt::white);
    drawTexturedQuads(renderer, quad, "heatmap");
}

void HeatMapLegendGL::drawHeatMapBorderLines(Renderer& renderer)
{
    ColoredLines borderLines;
    borderLines.addRectOutline(heatmapCoordinates(), Qt::white);
    drawLines(renderer, borderLines);
}

void HeatMapLegendGL::drawText(const QPointF& posn, const QString& str)
{
    // Isn't this both a very expensive AND also usefully generic function?
    // TODO: Factor out?
    //
    QFont monoFont("Courier", 12, QFont::Normal);
    QFontMetrics metrics(monoFont);
    QRect textRect = metrics.boundingRect(str);

    QImage image(textRect.size(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter qpainter(&image);
    qpainter.setFont(monoFont);
    qpainter.setPen(Qt::white);
    qpainter.setRenderHint(QPainter::Antialiasing, true);
    qpainter.drawText(textRect.x(), metrics.ascent(), str);
    qpainter.end();

    m_textureText.destroy();
    m_textureText.create();
    m_textureText.setData(image.mirrored());
    const int x = posn.x();
    const int y = posn.y();

    QVector<QVector2D> vertices;
    vertices.append(QVector2D(x + textRect.x(), y + metrics.ascent()));
    vertices.append(QVector2D(x + textRect.x(), y - metrics.descent()));
    vertices.append(QVector2D(x + textRect.x() + textRect.width(), y - metrics.descent()));
    vertices.append(QVector2D(x + textRect.x() + textRect.width(), y + metrics.ascent()));

    QVector<QVector2D> texCoord;
    texCoord.append(QVector2D(0.0, 0.0));
    texCoord.append(QVector2D(0.0, 1.0));
    texCoord.append(QVector2D(1.0, 1.0));
    texCoord.append(QVector2D(1.0, 0.0));

    m_textureText.bind();
    glBegin(GL_QUADS);
    {
        for (int i = 0; i < vertices.size(); ++i) {
            glTexCoord2f(texCoord.at(i).x(), texCoord.at(i).y());
            glVertex2f(vertices.at(i).x(), vertices.at(i).y());
        }
    }
    glEnd();
    m_textureText.release();
}

QRectF HeatMapLegendGL::boundingRect() const
{
    const QRectF coords = heatmapCoordinates();
    const QSizeF boundingSize(coords.width() + bars_width, coords.height());
    return QRectF(coords.topLeft(), boundingSize);
}

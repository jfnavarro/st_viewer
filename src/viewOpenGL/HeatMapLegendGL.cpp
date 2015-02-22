/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HeatMapLegendGL.h"

#include "math/Common.h"
#include "color/HeatMap.h"

#include <QImage>
#include <QGLTexture2D>
#include <QApplication>
#include <QVector2D>
#include <QGLPainter>
#include <QVector2DArray>
#include <QLabel>

static const qreal legend_x = 0.0;
static const qreal legend_y = 0.0;
static const qreal legend_width = 25.0;
static const qreal legend_height = 150.0;
static const qreal bars_width = 35.0;

HeatMapLegendGL::HeatMapLegendGL(QObject* parent)
    : GraphicItemGL(parent),
      m_maxReads(1),
      m_minReads(1),
      m_minGenes(1),
      m_maxGenes(1),
      m_colorComputingMode(Globals::LinearColor),
      m_valueComputation(Reads)
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
    m_texture.cleanupResources();
    m_texture.release();
    m_texture.clearImage();
    m_texture_vertices.clear();
    m_texture_cords.clear();
    m_texture_cords.clear();
    m_textureText.cleanupResources();
    m_textureText.release();
    m_textureText.clearImage();
    m_borders.clear();
    //m_valueComputation = Reads;
}

void HeatMapLegendGL::draw(QGLPainter *painter)
{

    glEnable(GL_TEXTURE_2D);
    {
        // draw image texture
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatReplaceTexture2D);
        m_texture.bind();
        painter->setVertexAttribute(QGL::Position, m_texture_vertices);
        painter->setVertexAttribute(QGL::TextureCoord0, m_texture_cords);
        painter->draw(QGL::TriangleFan, m_texture_vertices.size());
        m_texture.release();

        // draw borders
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(Qt::white);
        painter->setVertexAttribute(QGL::Position, m_borders);
        painter->draw(QGL::LineLoop, m_borders.size());

        // draw text (add 5 pixels offset to the right)
        const int min = m_valueComputation == Reads ? m_minReads : m_minGenes;
        const int max = m_valueComputation == Reads ? m_maxReads : m_maxGenes;
        drawText(painter, QPointF(legend_x + legend_width + 5, 0), QString::number(max));
        drawText(painter, QPointF(legend_x + legend_width + 5, legend_height), QString::number(min));
    }
    glDisable(GL_TEXTURE_2D);
}

void HeatMapLegendGL::setSelectionArea(const SelectionEvent *)
{

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

void HeatMapLegendGL::setColorComputingMode(const Globals::GeneColorMode &mode)
{
    // update color computing mode
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

void HeatMapLegendGL::generateHeatMap()
{
    clearData();

    const int min = m_valueComputation == Reads ? m_minReads : m_minGenes;
    const int max = m_valueComputation == Reads ? m_maxReads : m_maxGenes;

    // generate image texture
    QImage image(legend_width, legend_height, QImage::Format_ARGB32);
    //here we can chose the type of Spectrum (linear, log or exp) and the type
    //of color mapping (wavelenght or linear interpolation)
    Heatmap::createHeatMapImage(image, min, max, m_colorComputingMode);

    m_texture.setImage(image);
    m_texture.setVerticalWrap(QGL::ClampToEdge);
    m_texture.setHorizontalWrap(QGL::ClampToEdge);
    m_texture.setBindOptions(QGLTexture2D::LinearFilteringBindOption
                             | QGLTexture2D::MipmapBindOption);

    m_texture_vertices.append(legend_x, legend_y);
    m_texture_vertices.append(legend_x + legend_width, legend_y);
    m_texture_vertices.append(legend_x + legend_width, legend_y + legend_height);
    m_texture_vertices.append(legend_x, legend_y + legend_height);

    m_texture_cords.append(0.0, 0.0);
    m_texture_cords.append(1.0, 0.0);
    m_texture_cords.append(1.0, 1.0);
    m_texture_cords.append(0.0, 1.0);

    m_borders.append(legend_x, legend_y);
    m_borders.append(legend_x + legend_width, legend_y);
    m_borders.append(legend_x + legend_width, legend_y + legend_height);
    m_borders.append(legend_x, legend_y + legend_height);

    emit updated();
}

void HeatMapLegendGL::drawText(QGLPainter *painter, const QPointF &posn,
                               const QString& str)
{
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

    m_textureText.cleanupResources();
    m_textureText.release();
    m_textureText.clearImage();
    m_textureText.setImage(image);
    const int x = posn.x();
    const int y = posn.y();

    QVector2DArray vertices;
    vertices.append(x + textRect.x(), y + metrics.ascent());
    vertices.append(x + textRect.x(), y - metrics.descent());
    vertices.append(x + textRect.x() + textRect.width(), y - metrics.descent());
    vertices.append(x + textRect.x() + textRect.width(), y + metrics.ascent());

    QVector2DArray texCoord;
    texCoord.append(0.0, 0.0);
    texCoord.append(0.0, 1.0);
    texCoord.append(1.0, 1.0);
    texCoord.append(1.0, 0.0);

    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatReplaceTexture2D);
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::TextureCoord0, texCoord);
    m_textureText.bind();
    painter->draw(QGL::TriangleFan, vertices.size());
    m_textureText.release();
}

const QRectF HeatMapLegendGL::boundingRect() const
{
    return QRectF(legend_x, legend_y,
                  legend_width + bars_width, legend_height);
}

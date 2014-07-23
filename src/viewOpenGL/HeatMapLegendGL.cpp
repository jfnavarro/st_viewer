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

static const qreal legend_x = 0.0;
static const qreal legend_y = 0.0;
static const qreal legend_width = 25.0;
static const qreal legend_height = 150.0;
static const qreal bars_width = 35.0;

HeatMapLegendGL::HeatMapLegendGL(QObject* parent)
    : GraphicItemGL(parent),
      m_lower_threshold(Globals::GENE_THRESHOLD_MIN),
      m_upper_threshold(Globals::GENE_THRESHOLD_MAX),
      m_max(Globals::GENE_THRESHOLD_MAX),
      m_min(Globals::GENE_THRESHOLD_MIN)
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
    m_texture.cleanupResources();
    m_texture.release();
    m_texture.clearImage();

    m_textureText.cleanupResources();
    m_textureText.release();
    m_textureText.clearImage();
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

        // render text
        drawText(painter, m_lower_text_position, m_lower_text);
        drawText(painter, m_upper_text_position, m_upper_text);

        // draw borders
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(Qt::white);
        painter->setVertexAttribute(QGL::Position, m_borders);
        painter->draw(QGL::LineLoop, m_borders.size());

        // draw threshold bars
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(Qt::red);
        painter->setVertexAttribute(QGL::Position, m_bars);
        painter->draw(QGL::Lines, m_bars.size());

    }
    glDisable(GL_TEXTURE_2D);
}

void HeatMapLegendGL::setSelectionArea(const SelectionEvent *)
{

}

void HeatMapLegendGL::setBoundaries(qreal min, qreal max)
{
    m_min = min;
    m_max = max;
    setLowerLimit(Globals::GENE_THRESHOLD_MIN);
    setUpperLimit(Globals::GENE_THRESHOLD_MAX);
    generateHeatMap();
    emit updated();
}

void HeatMapLegendGL::setLowerLimit(int limit)
{
    //TODO this formula might not be correct
    static const qreal offlimit = Globals::GENE_THRESHOLD_MAX
            - Globals::GENE_THRESHOLD_MIN;
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (limit / offlimit) * range;
    const qreal normalized_limit = adjusted_limit / range;
    m_lower_threshold = STMath::clamp( normalized_limit, 0.0, 1.0 );
    m_lower_text = QString::number(limit);
    generateBarAndTexts();
    emit updated();
}

void HeatMapLegendGL::setUpperLimit(int limit)
{
    //TODO this formula might not be correct
    static const qreal offlimit = Globals::GENE_THRESHOLD_MAX
            - Globals::GENE_THRESHOLD_MIN;
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (limit / offlimit) * range;
    const qreal normalized_limit = adjusted_limit / range;
    m_upper_threshold = STMath::clamp( normalized_limit, 0.0, 1.0 );
    m_upper_text = QString::number(limit);
    generateBarAndTexts();
    emit updated();
}

void HeatMapLegendGL::generateHeatMap()
{
    // generate image texture
    QImage image(m_min, m_max, QImage::Format_ARGB32);
    Heatmap::createHeatMapImage(image, Heatmap::SpectrumExp, m_min, m_max);

    m_texture.cleanupResources();
    m_texture.release();
    m_texture.clearImage();
    m_texture.setImage(image);
    m_texture.setVerticalWrap(QGL::ClampToEdge);
    m_texture.setHorizontalWrap(QGL::ClampToEdge);
    m_texture.setBindOptions(QGLTexture2D::LinearFilteringBindOption
                             | QGLTexture2D::MipmapBindOption);

    m_texture_vertices.clear();
    m_texture_vertices.append(legend_x, legend_y);
    m_texture_vertices.append(legend_x + legend_width, legend_y);
    m_texture_vertices.append(legend_x + legend_width, legend_y + legend_height);
    m_texture_vertices.append(legend_x, legend_y + legend_height);

    m_texture_cords.clear();
    m_texture_cords.append(0.0f, 0.0f);
    m_texture_cords.append(1.0f, 0.0f);
    m_texture_cords.append(1.0f, 1.0f);
    m_texture_cords.append(0.0f, 1.0f);

    m_borders.clear();
    m_borders.append(legend_x, legend_y);
    m_borders.append(legend_x + legend_width, legend_y);
    m_borders.append(legend_x + legend_width, legend_y + legend_height);
    m_borders.append(legend_x, legend_y + legend_height);
}

void HeatMapLegendGL::generateBarAndTexts()
{
    // threshold bars
    const qreal thresholdLowerHeight = (1.0 - m_lower_threshold) * legend_height;
    const qreal thresholdUpperHeight = (1.0 - m_upper_threshold) * legend_height;

    m_bars.clear();
    m_bars.append(legend_x, thresholdLowerHeight);
    m_bars.append(legend_x + bars_width, thresholdLowerHeight);
    m_bars.append(legend_x, thresholdUpperHeight);
    m_bars.append(legend_x + bars_width, thresholdUpperHeight);

    // create text
    m_lower_text_position = QPointF(legend_x + bars_width, thresholdLowerHeight);
    m_upper_text_position = QPointF(legend_x + bars_width, thresholdUpperHeight);
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
    texCoord.append(0.0f, 0.0f);
    texCoord.append(0.0f, 1.0f);
    texCoord.append(1.0f, 1.0f);
    texCoord.append(1.0f, 0.0f);

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

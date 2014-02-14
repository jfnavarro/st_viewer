/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HeatMapLegendGL.h"

#include "utils/Utils.h"
#include "math/Common.h"
#include "color/HeatMap.h"

#include <QImage>
#include <QGLTexture2D>
#include <QApplication>
#include <QVector2D>
#include <QGLPainter>
#include <QVector2DArray>

static const qreal legend_x = 0.0f;
static const qreal legend_y = 0.0f;
static const qreal legend_width = 25.0f;
static const qreal legend_height = 150.0f;
static const qreal bars_width = 35.0f;
static const qreal spectra_min = 1.0f;
static const qreal spectra_max = 100.0f;
constexpr qreal boundaries ()
{
    return Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;
}

HeatMapLegendGL::HeatMapLegendGL(QObject* parent)
    : QGLSceneNode(parent)
{

}

HeatMapLegendGL::~HeatMapLegendGL()
{
    m_texture.cleanupResources();
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

        // render text
        drawText(painter, m_lower_text_position, m_lower_text);
        drawText(painter, m_upper_text_position, m_upper_text);
    }
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LINE_SMOOTH);
    {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        // draw borders
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(Qt::black);
        painter->setVertexAttribute(QGL::Position, m_borders);
        painter->draw(QGL::LineLoop, m_borders.size());

        // draw threshold bars
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(Qt::red);
        painter->setVertexAttribute(QGL::Position, m_bars);
        painter->draw(QGL::Lines, m_bars.size());
    }
    glDisable(GL_LINE_SMOOTH);
}

void HeatMapLegendGL::drawGeometry(QGLPainter *painter)
{
    QGLSceneNode::drawGeometry(painter);
}

void HeatMapLegendGL::setBoundaries(qreal min, qreal max)
{
    m_min = min;
    m_max = max;
    generateHeatMap();
    emit updated();
}

void HeatMapLegendGL::setLowerLimit(int limit)
{
    const qreal offlimit = boundaries();
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (qreal(limit) / offlimit ) * range;
    const qreal normalized_limit = adjusted_limit / range;
    m_lower_threshold = STMath::clamp(normalized_limit , 0.0, 1.0);
    m_lower_text = QString::number( (int)adjusted_limit );
    generateBarAndTexts();
    emit updated();
}

void HeatMapLegendGL::setUpperLimit(int limit)
{
    const qreal offlimit = boundaries();
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (qreal(limit) / offlimit ) * range;
    const qreal normalized_limit =  adjusted_limit / range;
    m_upper_threshold = STMath::clamp( normalized_limit, 0.0, 1.0);
    m_upper_text = QString::number( (int)adjusted_limit );
    generateBarAndTexts();
    emit updated();
}

void HeatMapLegendGL::generateHeatMap()
{
    // generate image texture
    QImage image(1, legend_height, QImage::Format_ARGB32);
    Heatmap::createHeatMapImage(image, Heatmap::SpectrumExp,
                                std::min(spectra_min, m_min), std::max(spectra_max, m_max));
    m_texture.cleanupResources();
    m_texture.setImage(image);
    m_texture.setVerticalWrap(QGL::ClampToEdge);
    m_texture.setHorizontalWrap(QGL::ClampToEdge);
    //m_texture.setBindOptions(QGLTexture2D::LinearFilteringBindOption);

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
    const qreal thresholdLowerHeight = m_lower_threshold * legend_height;
    const qreal thresholdUpperHeight = m_upper_threshold * legend_height;

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
    QFont monoFont("Courier", 10, QFont::Normal);
    //monoFont.setStyleHint(QFont::TypeWriter);
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

    QGLTexture2D texture;
    texture.setImage(image.mirrored());
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
    texture.bind();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::TextureCoord0, texCoord);
    painter->draw(QGL::TriangleFan, vertices.size());
    texture.cleanupResources();
}

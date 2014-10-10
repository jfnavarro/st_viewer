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
      m_max(1),
      m_min(1),
      m_colorComputingMode(Globals::LinearColor)

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

    }
    glDisable(GL_TEXTURE_2D);
}

void HeatMapLegendGL::setSelectionArea(const SelectionEvent *)
{

}

void HeatMapLegendGL::setLowerLimit(const qreal limit)
{
    if (m_min != limit) {
        m_min = limit;
        generateHeatMap();
    }
}

void HeatMapLegendGL::setUpperLimit(const qreal limit)
{
    if (m_max != limit) {
        m_max = limit;
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

void HeatMapLegendGL::generateHeatMap()
{
    // generate image texture
    QImage image(legend_width, legend_height, QImage::Format_ARGB32);
    //here we can chose the type of Spectrum (linear, log or exp) and the type
    //of color mapping (wavelenght or linear interpolation)
    Heatmap::createHeatMapImage(image, m_min, m_max, m_colorComputingMode);

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
    m_texture_cords.append(0.0, 0.0);
    m_texture_cords.append(1.0, 0.0);
    m_texture_cords.append(1.0, 1.0);
    m_texture_cords.append(0.0, 1.0);

    m_borders.clear();
    m_borders.append(legend_x, legend_y);
    m_borders.append(legend_x + legend_width, legend_y);
    m_borders.append(legend_x + legend_width, legend_y + legend_height);
    m_borders.append(legend_x, legend_y + legend_height);

    emit updated();
}

const QRectF HeatMapLegendGL::boundingRect() const
{
    return QRectF(legend_x, legend_y,
                  legend_width + bars_width, legend_height);
}

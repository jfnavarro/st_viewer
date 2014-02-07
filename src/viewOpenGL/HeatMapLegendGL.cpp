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
#include <QColor4ub>
#include <QGLTexture2D>
#include <QGLBuilder>
#include <QGeometryData>
#include <QGLMaterial>
#include <QVector2D>
#include <QGLPainter>

static const QRectF bound_rect =
        QRectF(0.0f, 0.0f,
        Globals::heatmap_bar_width * 2,
        Globals::heatmap_height);

static const qreal boundaries = (Globals::gene_threshold_max - Globals::gene_threshold_min);

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
        painter->setStandardEffect(QGL::FlatReplaceTexture2D);
        m_texture.bind();
        m_rectangle->draw(painter);
    }
    glDisable(GL_TEXTURE_2D);

    // render text
}

void HeatMapLegendGL::drawGeometry(QGLPainter *painter)
{
    QGLSceneNode::drawGeometry(painter);
}


void HeatMapLegendGL::setBoundaries(qreal min, qreal max)
{
    m_min = min;
    m_max = max;
}

void HeatMapLegendGL::setLowerLimit(int limit)
{
    const qreal adjusted_limit =  (qreal(limit) / boundaries) * (m_max - m_min);
    m_lower_threshold = STMath::clamp( (m_max - adjusted_limit) / (m_max - m_min), 0.0, 1.0);
    clearData();
    generateHeatMapData();
}

void HeatMapLegendGL::setUpperLimit(int limit)
{
    const qreal adjusted_limit =  (qreal(limit) / boundaries) * (m_max - m_min);
    m_upper_threshold = STMath::clamp( (m_max - adjusted_limit) / (m_max - m_min), 0.0, 1.0);
    clearData();
    generateHeatMapData();
}

void HeatMapLegendGL::clearData()
{

}

void HeatMapLegendGL::generateHeatMapData()
{
    // generate image
    QImage image(1, Globals::heatmap_height, QImage::Format_ARGB32);
    Heatmap::createHeatMapImage(image, Heatmap::SpectrumExp, m_min, m_max);

    m_texture.cleanupResources();
    m_texture.setImage(image);
    //m_texture.setVerticalWrap(QGL::Repeat);
    //m_texture.setHorizontalWrap(QGL::Repeat);
    //m_texture.setBindOptions(QGLTexture2D::LinearFilteringBindOption);

    QGLBuilder builder;
    QGeometryData data;

    const qreal x = 0.0f;
    const qreal y = 0.0f;
    const qreal width = Globals::heatmap_width;
    const qreal height = Globals::heatmap_height;
    //const qreal aspect_ratio = height / width;

    QVector2D a(x, y);
    QVector2D b(x + width, y);
    QVector2D c(x + width, y + height);
    QVector2D d(x, y + height);
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    QVector2D td(0.0f, 1.0f);

    data.appendVertex(a, b, c, d);
    data.appendTexCoord(ta, tb, tc, td);

    builder.addQuads(data);
    m_rectangle = builder.finalizedSceneNode();

    /*
    // make up heatmap rectangle
    const QSizeF size = m_bounds.size();
    m_rect = QRectF(
                 QPointF(0.0f, 0.0f),
                 QSizeF(Globals::heatmap_width, size.height())
             );

    // threshold height
    const qreal height = qreal(size.height());
    const qreal thresholdLowerHeight = (1.0 - m_lower_threshold) * height;
    const qreal thresholdUpperHeight = (m_upper_threshold) * height;

    // color of the border
    const QColor4ub borderColor = QColor4ub(Qt::white);

    //create the legend
    factory.setColor(borderColor);
    factory.addShape(m_rect);
    factory.setColor(QColor4ub(Qt::black));

    factory.addShape( QRectF(m_rect.topLeft(), m_rect.topRight()) );
    factory.addShape( QRectF(m_rect.topRight(), m_rect.bottomLeft()) );
    factory.addShape( QRectF(m_rect.bottomLeft(), m_rect.bottomRight()) );
    factory.addShape( QRectF(m_rect.bottomRight(), m_rect.topLeft()) );

    factory.setColor(QColor4ub(Qt::black));
    //size of rect should be 3
    factory.addShape( QRectF(
                          QPointF(0.0f, thresholdLowerHeight),
                          QPointF(Globals::heatmap_bar_width, thresholdLowerHeight) ) );
    factory.addShape( QRectF(
                          QPointF(0.0f, thresholdUpperHeight),
                          QPointF(Globals::heatmap_bar_width, thresholdUpperHeight) ) );

    factory.setColor(QColor4ub(Qt::red));
    //size of rect should be 1
    factory.addShape( QRectF(
                          QPointF(0.0f, thresholdLowerHeight),
                          QPointF(Globals::heatmap_bar_width, thresholdLowerHeight) ) );
    factory.addShape( QRectF(
                         QPointF(0.0f, thresholdUpperHeight),
                         QPointF(Globals::heatmap_bar_width, thresholdUpperHeight) ) );

    // create text
    const qreal height = qreal(m_bounds.height());
    QFont monoFont("Courier", 10, QFont::Normal);
    monoFont.setStyleHint(QFont::TypeWriter);
    m_text.addText(Globals::heatmap_bar_width, 10, monoFont, QString("%1").arg(m_hitCountMax));
    m_text.addText(Globals::heatmap_bar_width, height, monoFont, QString("%1").arg(m_hitCountMin));
*/
}

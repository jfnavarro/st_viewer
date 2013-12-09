/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include "data/GLElementData.h"
#include "data/GLElementRender.h"
#include "data/GLTexture.h"
#include "image/GLImage.h"

#include "ViewItemGL.h"

// HeatMapLegend is an view port GUI item that visualizes the heat map spectrum
// in order to give a reference point in determining each features hit count.
class HeatMapLegendGL : public ViewItemGL
{
    Q_OBJECT

public:

    explicit HeatMapLegendGL(QObject* parent = 0);
    virtual ~HeatMapLegendGL();

    void setBounds(const QRectF& bounds);

    virtual void render(QPainter* painter);

    virtual const QRectF boundingRect() const;
    virtual bool contains(const QPointF& point) const;

public slots:
    void setHitCountLimits(int min, int max, int sum);
    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

private:

    static const QRectF DEFAULT_BOUNDS;

    QRectF m_bounds;
    QRectF m_rect;

    //limit variables
    int m_hitCountMin;
    int m_hitCountMax;
    int m_hitCountSum;
    int m_lowerLimit;
    int m_upperLimit;
    qreal m_lower_threshold;
    qreal m_upper_threshold;

    // render data
    GL::GLElementData m_data;
    GL::GLElementRenderQueue m_queue;
    GL::GLimage m_image;
    GL::GLtexture m_texture;
    QPainterPath m_text;

    //rendering functions
    void rebuildHeatMapData();
    void generateHeatMapData();
    void rebuildHeatMapText();
    void generateHeatMapText();
    void rebuildHeatMapStaticData();
    void generateStaticHeatMapData();
};

#endif // HEATMAPLEGEND_H //

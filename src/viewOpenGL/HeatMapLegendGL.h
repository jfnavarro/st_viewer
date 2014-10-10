/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include "GraphicItemGL.h"
#include "utils/Utils.h"

class QGLPainter;
class QImage;
class QGLTexture2D;
class QVector2DArray;

// HeatMapLegend is an view port GUI item that visualizes the heat map spectrum
// in order to give a reference point in determining each features hit count.
class HeatMapLegendGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit HeatMapLegendGL(QObject* parent = 0);
    virtual ~HeatMapLegendGL();

public slots:
    
    //TODO slots should have the prefix "slot"

    // slots to adjust the boundaries when the threshold is changed
    void setLowerLimit(const qreal limit);
    void setUpperLimit(const qreal limit);

    // slot to change the function to compute color values
    void setColorComputingMode(const Globals::GeneColorMode &mode);

protected:

    void draw(QGLPainter *painter) override;
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;

private:

    // rendering functions
    void generateHeatMap();

    // draw text centered on the bottom of the "posn" rectangle.
    void drawText(QGLPainter *painter, const QPointF& posn, const QString& str);

    // min and max boundaries values to compute colors from
    qreal m_max;
    qreal m_min;

    // color computing mode (exp - log - linear)
    Globals::GeneColorMode m_colorComputingMode;

    // texture color data
    QGLTexture2D m_texture;
    QVector2DArray m_texture_vertices;
    QVector2DArray m_texture_cords;

    // data vertex arrays
    QVector2DArray m_borders;

    Q_DISABLE_COPY(HeatMapLegendGL);
};

#endif // HEATMAPLEGEND_H //

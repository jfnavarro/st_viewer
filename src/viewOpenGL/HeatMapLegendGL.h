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
    
    void setBoundaries(qreal min, qreal max);
    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

protected:

    void draw(QGLPainter *painter);

    const QRectF boundingRect() const;

    void setSelectionArea(const SelectionEvent *) {}

private:

    // rendering functions
    void generateHeatMap();
    void generateBarAndTexts();

    // draw text centered on the bottom of the "posn" rectangle.
    void drawText(QGLPainter *painter, const QPointF& posn, const QString& str);

    // limits and boundaries
    qreal m_lower_threshold;
    qreal m_upper_threshold;
    qreal m_max;
    qreal m_min;

    // texture color data
    QGLTexture2D m_texture;
    QVector2DArray m_texture_vertices;
    QVector2DArray m_texture_cords;

    // data vertex arrays
    QVector2DArray m_bars;
    QVector2DArray m_borders;

    // texture text data
    QString m_lower_text;
    QPointF m_lower_text_position;
    QString m_upper_text;
    QPointF m_upper_text_position;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include "GraphicItemGL.h"

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

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    const QRectF boundingRect() const;

public slots:
    
    void setBoundaries(qreal min, qreal max);
    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

private:

    //rendering functions
    void generateHeatMap();
    void generateBarAndTexts();

    // Draw text centered on the bottom of the "posn" rectangle.
    void drawText(QGLPainter *painter, const QPointF& posn, const QString& str);

    // limits and boundaries
    qreal m_lower_threshold = 0.0f;
    qreal m_upper_threshold = 1.0f;
    qreal m_max = 1.0f;
    qreal m_min = 0.0f;

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
};

#endif // HEATMAPLEGEND_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include <QGLSceneNode>
#include "data/DataProxy.h"

class QGLPainter;
class QImage;
class QGLTexture2D;

// HeatMapLegend is an view port GUI item that visualizes the heat map spectrum
// in order to give a reference point in determining each features hit count.
class HeatMapLegendGL : public QGLSceneNode
{
    Q_OBJECT

public:

    explicit HeatMapLegendGL(QObject* parent = 0);
    virtual ~HeatMapLegendGL();

    //rendering functions
    void generateHeatMapData();
    void clearData();

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

public slots:
    
    void setBoundaries(qreal min, qreal max);
    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

private:

    // limits and boundaries
    qreal m_lower_threshold = 0.0f;
    qreal m_upper_threshold = 1.0f;
    qreal m_max = 1.0f;
    qreal m_min = 0.0f;

    // render data
    QGLTexture2D m_texture;
    QGLSceneNode *m_rectangle = nullptr;
    QPainterPath m_text;
};

#endif // HEATMAPLEGEND_H //

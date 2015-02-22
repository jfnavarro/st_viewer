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

    enum ValueComputation {
        Reads = 1,
        Genes = 2,
        TPM = 3
    };

    explicit HeatMapLegendGL(QObject* parent = 0);
    virtual ~HeatMapLegendGL();

    void clearData();

public slots:
    
    //TODO slots should have the prefix "slot"

    // slots to adjust the boundaries when the threshold is changed
    void setLowerLimitReads(const int limit);
    void setUpperLimitReads(const int limit);
    void setLowerLimitGenes(const int limit);
    void setUpperLimitGenes(const int limit);

    void setValueComputation(ValueComputation mode);

    // slot to change the function to compute color values
    void setColorComputingMode(const Globals::GeneColorMode &mode);

protected:

    void draw(QGLPainter *painter) override;
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;

private:

    // rendering functions (heatmap is created as a texture)
    void generateHeatMap();

    // internal function to render text as a texture
    void drawText(QGLPainter *painter, const QPointF &posn, const QString& str);

    // min and max boundaries values to compute colors from
    int m_maxReads;
    int m_minReads;
    int m_minGenes;
    int m_maxGenes;

    // color computing mode (exp - log - linear)
    Globals::GeneColorMode m_colorComputingMode;

    // texture color data
    QGLTexture2D m_texture;
    QGLTexture2D m_textureText;
    QVector2DArray m_texture_vertices;
    QVector2DArray m_texture_cords;

    // data vertex arrays
    QVector2DArray m_borders;

    ValueComputation m_valueComputation;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

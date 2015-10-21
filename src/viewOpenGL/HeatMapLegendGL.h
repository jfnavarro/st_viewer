/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include "GraphicItemGL.h"
#include "utils/Utils.h"

#include <memory>
#include <QOpenGLTexture>

class QImage;

// HeatMapLegend is an visual item that is used to represent the heat map spectrum
// in order to give a reference point about the color-value relationship for the gene data
class HeatMapLegendGL : public GraphicItemGL
{
    Q_OBJECT

public:
    enum ValueComputation { Reads = 1, Genes = 2, TPM = 3 };

    explicit HeatMapLegendGL(QObject* parent = 0);
    virtual ~HeatMapLegendGL();

    void clearData();

    // set the min-values of thresholds without invoking generateData
    void setMinMaxValues(const int readsMin,
                         const int readsMax,
                         const int genesMin,
                         const int genesMax);

    // rendering functions (heatmap is created as a texture)
    void generateHeatMap();

public slots:

    // TODO slots should have the prefix "slot"

    // slots to adjust the boundaries when the threshold is changed
    void setLowerLimitReads(const int limit);
    void setUpperLimitReads(const int limit);
    void setLowerLimitGenes(const int limit);
    void setUpperLimitGenes(const int limit);

    // slow to change the value computation type (genes or reads)
    void setValueComputation(ValueComputation mode);

    // slot to change the function to compute color values
    void setColorComputingMode(const Globals::GeneColorMode& mode);

protected:
    QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent*) override;

private:
    void doDraw(Renderer& renderer) override;

    // Draws the heat map.
    void drawHeatmap(Renderer& renderer);

    // Draws the border lines around the heat map.
    void drawHeatMapBorderLines(Renderer& renderer);

    // internal function to render text as a texture
    void drawText(Renderer& renderer, const QPointF& posn, const QString& str);

    // Returns the heat map coordinates.
    QRectF heatmapCoordinates() const;

    // min and max boundaries values to compute colors from
    int m_maxReads;
    int m_minReads;
    int m_minGenes;
    int m_maxGenes;

    // color computing mode (exp - log - linear)
    Globals::GeneColorMode m_colorComputingMode;

    // If the heatmap texture is out of synch with the intended heatmap, this
    // pointer will be non null. It will be set correctly
    std::unique_ptr<QImage> m_pendingHeatmap;

    // use genes or reads to compute min-max
    ValueComputation m_valueComputation;

    // to know when the rendering data is initialized
    bool m_isInitialized;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

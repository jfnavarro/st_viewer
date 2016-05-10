#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include <QOpenGLTexture>

#include "GraphicItemGL.h"

class QImage;

// HeatMapLegend is an visual item that is used to represent the heat map
// spectrum
// in order to give a reference point about the color-value relationship for the
// gene data
// when the user selects heat map mode
class HeatMapLegendGL : public GraphicItemGL
{
    Q_OBJECT

public:
    enum ValueComputation { Reads = 1, Genes = 2, TPM = 3 };

    explicit HeatMapLegendGL(QObject *parent = 0);
    virtual ~HeatMapLegendGL();

    // clear up all data
    void clearData();

    // set the min-values of thresholds without invoking generateData
    void setMinMaxValues(const unsigned readsMin,
                         const unsigned readsMax,
                         const unsigned genesMin,
                         const unsigned genesMax);

    // rendering functions (heatmap is created as a texture)
    void generateHeatMap();

public slots:

    // TODO slots should have the prefix "slot"

    // slots to adjust the boundaries when the threshold is changed
    void setLowerLimitReads(const unsigned limit);
    void setUpperLimitReads(const unsigned limit);
    void setLowerLimitGenes(const unsigned limit);
    void setUpperLimitGenes(const unsigned limit);

    // slow to change the value computation type (genes or reads)
    void setValueComputation(ValueComputation mode);

    // slot to change the function to compute color values
    void setColorComputingMode(const Visual::GeneColorMode &mode);

protected:
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // internal function to render text as a texture
    void drawText(const QPointF &posn, const QString &str);

    // min and max boundaries values to compute colors from
    unsigned m_maxReads;
    unsigned m_minReads;
    unsigned m_minGenes;
    unsigned m_maxGenes;

    // color computing mode (exp - log - linear)
    Visual::GeneColorMode m_colorComputingMode;

    // texture color data
    QOpenGLTexture m_texture;
    QOpenGLTexture m_textureText;
    QVector<QVector2D> m_texture_vertices;
    QVector<QVector2D> m_texture_cords;

    // use genes or reads to compute min-max
    ValueComputation m_valueComputation;

    // to know when the rendering data is initialized
    bool m_isInitialized;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

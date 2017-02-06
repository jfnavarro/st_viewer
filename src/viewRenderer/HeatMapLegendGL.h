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
//TODO the threshold values and methods are duplicated in geneRenderedGL. They should
//be factored out into an object
class HeatMapLegendGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit HeatMapLegendGL(QObject *parent = 0);
    virtual ~HeatMapLegendGL();

    // clear up all data
    void clearData();

    // set the min-values of thresholds without invoking generateData
    void setMinMaxValues(const int readsMin,
                         const int readsMax,
                         const int genesMin,
                         const int genesMax);

    // rendering functions (heatmap is created as a texture)
    void generateHeatMap();

public slots:

    // slots for the thresholds
    void setReadsLowerLimit(const int limit);
    void setReadsUpperLimit(const int limit);
    void setGenesLowerLimit(const int limit);
    void setGenesUpperLimit(const int limit);

    // slots to set visual modes and color computations modes
    void setPoolingMode(const Visual::GenePooledMode &mode);
    void setColorComputingMode(const Visual::GeneColorMode &mode);

protected:
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // internal function to render text as a texture
    void drawText(const QPointF &posn, const QString &str,
                  QOpenGLFunctionsVersion &qopengl_functions);

    // threshold limits for gene hits
    int m_thresholdReadsLower;
    int m_thresholdReadsUpper;
    int m_thresholdGenesLower;
    int m_thresholdGenesUpper;

    // color computing mode (exp - log - linear)
    Visual::GeneColorMode m_colorComputingMode;

    // texture color data
    QOpenGLTexture m_texture;
    QOpenGLTexture m_textureText;
    QVector<QVector2D> m_texture_vertices;
    QVector<QVector2D> m_texture_cords;

    // (gene counts, reads counts or tpm)
    Visual::GenePooledMode m_valueComputation;

    // to know when the rendering data is initialized
    bool m_isInitialized;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

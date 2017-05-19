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

    explicit HeatMapLegendGL(QObject *parent = 0);
    virtual ~HeatMapLegendGL();

    // clear up all data
    void clearData();

    // create the heatmap
    void generateHeatMap(const int min, const int max);
    void generateHeatMap(const int min, const int max,
                         const QColor &low, const QColor &up);
public slots:

protected:
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // internal function to render text as a texture
    void drawText(const QPointF &posn, const QString &str,
                  QOpenGLFunctionsVersion &qopengl_functions);

    // texture color data
    QOpenGLTexture m_texture;
    QOpenGLTexture m_textureText;
    QVector<QVector2D> m_texture_vertices;
    QVector<QVector2D> m_texture_cords;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

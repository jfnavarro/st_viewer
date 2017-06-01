#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include <QOpenGLTexture>
#include "GraphicItemGL.h"
#include "viewPages/SettingsWidget.h"

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

    HeatMapLegendGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent = 0);
    virtual ~HeatMapLegendGL();

    // clear up all data
    void clearData();

public slots:

    // Update the rendering data
    void slotUpdate();

protected:
    const QRectF boundingRect() const override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;
    void setSelectionArea(const SelectionEvent &event);

private:

    // create the legend
    void generateHeatMap();

    // internal function to render text as a texture
    void drawText(const QPointF &posn, const QString &str,
                  QOpenGLFunctionsVersion &qopengl_functions);

    // texture color data
    QOpenGLTexture m_texture;
    QOpenGLTexture m_textureText;
    QVector<QVector2D> m_texture_vertices;
    QVector<QVector2D> m_texture_cords;
    // rendering settings
    const SettingsWidget::Rendering &m_rendering_settings;
    // true when rendering data has been computed
    bool m_initialized;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

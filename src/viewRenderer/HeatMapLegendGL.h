#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include "viewPages/SettingsWidget.h"

class QImage;

// HeatMapLegend is an visual item that is used to represent the heat map
// spectrum
// in order to give a reference point about the color-value relationship for the
// gene data
// when the user selects heat map mode
class HeatMapLegendGL
{

public:

    HeatMapLegendGL();
    virtual ~HeatMapLegendGL();

    // clear up all data
    void clearData();

    void draw(const SettingsWidget::Rendering &rendering_settings, QPainter &painter);

    // Update the rendering data
    void update();

private:

    // legend image
    QImage m_image;

    // true when the rendering data must be recomputed
    bool m_dirty;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //

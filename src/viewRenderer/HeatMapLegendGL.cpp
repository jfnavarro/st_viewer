#include "HeatMapLegendGL.h"

#include <QPainter>
#include <QImage>
#include <QLabel>

#include "math/Common.h"
#include "color/HeatMap.h"

static const double legend_x = 10.0;
static const double legend_y = 10.0;
static const int legend_width = 30;
static const int legend_height = 200;

HeatMapLegendGL::HeatMapLegendGL()
    : m_image()
    , m_dirty(true)
{
}

HeatMapLegendGL::~HeatMapLegendGL()
{
}

void HeatMapLegendGL::clearData()
{
    m_image = QImage();
    m_dirty = true;
}

void HeatMapLegendGL::update()
{
    m_dirty = true;
}

void HeatMapLegendGL::draw(const SettingsWidget::Rendering &rendering_setting, QPainter &painter)
{
    // get the min max values
    const double min = rendering_setting.legend_min;
    const double max = rendering_setting.legend_max;

    if (m_dirty) {
        // generate image texture with the size of the legend and then fill it up with the colors
        // using the min-max values of the threshold and the color mode
        Color::ColorGradients cmap =
                rendering_setting.visual_mode == SettingsWidget::VisualMode::ColorRange?
                    Color::ColorGradients::gpHot : Color::ColorGradients::gpSpectrum;
        m_image = Color::createLegend(legend_width, legend_height, min, max, cmap);
        m_dirty = false;
    }

    // draw the image
    painter.drawImage(QPointF(legend_x, legend_y), m_image);

    // draw text (add 5 pixels offset to the right)
    painter.setBrush(Qt::darkBlue);
    painter.drawText(QPointF(legend_x + legend_width + 5, 20.0), QString::number(max));
    painter.drawText(QPointF(legend_x + legend_width + 5, 20.0 + legend_height), QString::number(min));
}

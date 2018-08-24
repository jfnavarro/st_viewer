#include "HeatMapLegendGL.h"

#include <QPainter>
#include <QImage>
#include <QApplication>
#include <QVector2D>
#include <QLabel>

#include "math/Common.h"
#include "color/HeatMap.h"

static const double legend_x = 0.0;
static const double legend_y = 0.0;
static const double legend_width = 30.0;
static const double legend_height = 200.0;
static const double bars_width = 40.0;

HeatMapLegendGL::HeatMapLegendGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent)
    : GraphicItemGL(parent)
    , m_image()
    , m_rendering_settings(rendering_settings)
    , m_initialized(false)
{
    setVisualOption(GraphicItemGL::Transformable, false);
    setVisualOption(GraphicItemGL::Visible, false);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, false);
    setAnchor(Anchor::NorthEast);
}

HeatMapLegendGL::~HeatMapLegendGL()
{
}

void HeatMapLegendGL::clearData()
{
    m_image = QImage();
    m_initialized = false;
}

void HeatMapLegendGL::slotUpdate()
{
    generateLegend();
}

void HeatMapLegendGL::draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter)
{
    Q_UNUSED(qopengl_functions)

    if (!m_initialized || m_rendering_settings.visual_mode == SettingsWidget::VisualMode::Normal
            || m_rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange) {
        return;
    }

    // draw the image
    painter.drawImage(QPointF(legend_x, legend_y), m_image);
    // get the min max values
    const double min = m_rendering_settings.legend_min;
    const double max = m_rendering_settings.legend_max;
    // draw text (add 5 pixels offset to the right)
    painter.setBrush(Qt::darkBlue);
    painter.drawText(QPointF(legend_x + legend_width + 5, 0), QString::number(max));
    painter.drawText(QPointF(legend_x + legend_width + 5, legend_height), QString::number(min));
}

void HeatMapLegendGL::generateLegend()
{
    // get the min max values
    const double min = m_rendering_settings.legend_min;
    const double max = m_rendering_settings.legend_max;
    // generate image texture with the size of the legend and then fill it up with the colors
    // using the min-max values of the threshold and the color mode
    m_image = QImage(legend_width, legend_height, QImage::Format_ARGB32);
    Color::ColorGradients cmap =
            m_rendering_settings.visual_mode == SettingsWidget::VisualMode::ColorRange?
                Color::ColorGradients::gpHot : Color::ColorGradients::gpSpectrum;
    Color::createLegend(m_image, min, max, cmap);
    m_initialized = true;
}

const QRectF HeatMapLegendGL::boundingRect() const
{
    return QRectF(legend_x, legend_y, legend_width + bars_width, legend_height);
}

void HeatMapLegendGL::setSelectionArea(const SelectionEvent &event)
{
    Q_UNUSED(event)
}

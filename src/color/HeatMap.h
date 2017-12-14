#ifndef HEATMAP_H
#define HEATMAP_H

#include "math/Common.h"
#include "qcustomplot.h"
#include "viewPages/SettingsWidget.h"

class QImage;

// Heatmap is a convenience namespace containing functions to generate
// heatmap related images and data.
namespace Color
{

typedef QCPColorGradient::GradientPreset ColorGradients;

// Convenience function to generate a heatmap spectrum image given specific
// mapping function
// using the wave lenght spectra or a linear interpolation spectra between two
// colors
// the input image will be transformed with the new colors
void createLegend(QImage &image, const float lowerbound,
                  const float upperbound, const ColorGradients cmap);

// Convenience function to generate a QColor color from a real value
QColor createHeatMapWaveLenghtColor(const float value);

// Convenience function to generate a QColor color from a real value given a
// range
QColor createHeatMapLinearColor(const float value, const float min, const float max);

// Function that creates a dynamic color (alpha adjusted to the value given and min-max)
QColor createDynamicRangeColor(const float value, const float min, const float max, const QColor color);

// Functions to create a color mapped in the color range given
QColor createRangeColor(const float value, const float min, const float max,
                        const QColor init, const QColor end);

// Functions to create a color from a pre-set color map
QColor createCMapColor(const float value, const float min, const float max,
                       const ColorGradients cmap);

// helper fuctions to adjust a spot's color according to the rendering settings
QColor adjustVisualMode(const QColor merged_color,
                        const float &merged_value,
                        const float &min_reads,
                        const float &max_reads,
                        const SettingsWidget::VisualMode mode);

static QStringList color_list = (QStringList() << "red" << "green"
                                 << "blue" << "cyan" << "magenta"
                                 << "yellow" << "black" << "grey"
                                 << "darkBlue" << "darkGreen" << "darkRed"
                                 << "darkCyan" << "darkYellow" << "darkMagenta");
}

#endif // HEATMAP_H //

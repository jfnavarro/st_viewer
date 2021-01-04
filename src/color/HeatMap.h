#ifndef HEATMAP_H
#define HEATMAP_H

#include "math/Common.h"
#include "qcustomplot.h"
#include "viewPages/SettingsWidget.h"

class QImage;

// Heatmap is a convenience namespace containing functions to generate
// heatmap related images and data as well as colors.
namespace Color
{

typedef QCPColorGradient::GradientPreset ColorGradients;

// Convenience function to generate a heatmap spectrum image
// using a linear interpolation spectra in the gradient given as input
// using the upper and lower bounds given as parameters
QImage createLegend(const int width,
                    const int height,
                    const double lowerbound,
                    const double upperbound,
                    const ColorGradients cmap);

// Convenience function to generate a QColor color from a real value
// using a wavelength function
QColor createHeatMapWaveLenghtColor(const double value);

// Convenience function to generate a QColor from a real value given a range using linear
// interpolation
QColor createHeatMapLinearColor(const double value, const double min, const double max);

// Convenience function to adjust a QColor (alpha adjusted to the value given and min-max)
QColor createDynamicRangeColor(const double value, const double min, const double max, const QColor color);

// Convenience function to generate a QColor using linear interpolation and color range
QColor createRangeColor(const double value, const double min, const double max,
                        const QColor init, const QColor end);

// Convenience function to generate a QColor using linear interpolation and gpHot color gradient
QColor createCMapColorGpHot(const double value, const double min, const double max);

// Convenience function to generate a QColor using linear interpolation and color gradient
QColor createCMapColor(const double value, const double min, const double max,
                       const ColorGradients cmap);

// Convenience function to adjust a QColor given a range and a visualization mode
QColor adjustVisualMode(const QColor color,
                        const double value,
                        const double mix,
                        const double max,
                        const SettingsWidget::VisualMode mode);

//TODO increase the number of colors
static QStringList color_list = (QStringList() << "red" << "green"
                                 << "blue" << "cyan" << "magenta"
                                 << "yellow" << "black" << "grey"
                                 << "darkBlue" << "darkGreen" << "darkRed"
                                 << "darkCyan" << "darkYellow" << "darkMagenta");
}

#endif // HEATMAP_H //

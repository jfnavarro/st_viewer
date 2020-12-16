#include "HeatMap.h"

#include <QImage>
#include <QColor>

namespace Color
{

QImage createLegend(const int height,
                    const int width,
                    const double lowerbound,
                    const double upperbound,
                    const ColorGradients cmap)
{

    QImage image(width, height, QImage::Format_ARGB32);

    for (int y = 0; y < height; ++y) {
        // get the color of each line of the image as the heatmap
        // color normalized to the lower and upper bound of the image
        const int value = height - y - 1;
        const double adjusted_value
                = STMath::linearConversion<double, double>(static_cast<double>(value),
                                                       0.0,
                                                       static_cast<double>(height),
                                                       lowerbound,
                                                       upperbound);
        const QColor color = Color::createCMapColor(adjusted_value, lowerbound, upperbound, cmap);
        const QRgb rgb_color = color.rgb();
        for (int x = 0; x < width; ++x) {
            image.setPixel(x, y, rgb_color);
        }
    }

    return image;
}

QColor createHeatMapLinearColor(const double value, const double min, const double max)
{
    const double halfmax = (min + max) / 2;
    const double blue = std::max(0.0, 255 * (1 - (value / halfmax)));
    const double red = std::max(0.0, 255 * ((value / halfmax) - 1));
    const double green = 255 - blue - red;
    return QColor::fromRgb(red, green, blue);
}

QColor createDynamicRangeColor(const double value, const double min,
                               const double max, const QColor color)
{
    const double adjusted_value
            = STMath::norm<double, double>(value, min, max);
    QColor newcolor(color);
    newcolor.setAlphaF(adjusted_value);
    return newcolor;
}

QColor createHeatMapWaveLenghtColor(const double value)
{
    constexpr double gamma = 0.8f;

    // denorm value to range (380-780)
    const double cwavelength = STMath::denorm(value, 380.0, 780.0);

    // define colors according to wave lenght spectra
    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;

    if (380.0 <= cwavelength && cwavelength < 440.0) {
        red = -(cwavelength - 440.0) / (440.0 - 380.0);
        green = 0.0;
        blue = 1.0;
    } else if (440.0 <= cwavelength && cwavelength < 490.0) {
        red = 0.0;
        green = (cwavelength - 440.0) / (490.0 - 440.0);
        blue = 1.0;
    } else if (490.0 <= cwavelength && cwavelength < 510.0) {
        red = 0.0;
        green = 1.0;
        blue = -(cwavelength - 510.0) / (510.0 - 490.0);
    } else if (510.0 <= cwavelength && cwavelength < 580.0) {
        red = (cwavelength - 510.0) / (580.0 - 510.0);
        green = 1.0;
        blue = 0.0;
    } else if (580.0 <= cwavelength && cwavelength < 645.0) {
        red = 1.0;
        green = -(cwavelength - 645.0) / (645.0 - 580.0);
        blue = 0.0;
    } else if (645.0 <= cwavelength && cwavelength <= 780.0) {
        red = 1.0;
        green = 0.0;
        blue = 0.0;
    }

    // Let the intensity fall off near the vision limits
    double factor = 0.3;
    if (380.0 <= cwavelength && cwavelength < 420.0) {
        factor = 0.3 + 0.7 * (cwavelength - 380.0) / (420.0 - 380.0);
    } else if (420.0 <= cwavelength && cwavelength < 700.0) {
        factor = 1.0;
    } else if (700.0 <= cwavelength && cwavelength <= 780.0) {
        factor = 0.3 + 0.7 * (780.0 - cwavelength) / (780.0 - 700.0);
    }

    // Gamma adjustments (clamp to [0.0, 1.0])
    red = std::clamp(qPow(red * factor, gamma), 0.0, 1.0);
    green = std::clamp(qPow(green * factor, gamma), 0.0, 1.0);
    blue = std::clamp(qPow(blue * factor, gamma), 0.0, 1.0);

    // return QColor from rgb
    return QColor::fromRgbF(red, green, blue, 1.0);
}

QColor createRangeColor(const double value, const double min, const double max,
                        QColor init, QColor end)
{
    const double norm_value = STMath::norm<double, double>(value, min, max);
    return STMath::lerp(norm_value, init, end);
}

QColor createCMapColorGpHot(const double value, const double min, const double max)
{
    const QCPRange range(min, max);
    QCPColorGradient cmap(QCPColorGradient::gpHot);
    return QColor(cmap.color(value, range));
}

QColor createCMapColor(const double value, const double min,
                       const double max, const ColorGradients cmap)
{
    const QCPRange range(min, max);
    QCPColorGradient cmapper(cmap);
    return QColor(cmapper.color(value, range));
}

QColor adjustVisualMode(const QColor color,
                        const double value,
                        const double min,
                        const double max,
                        const SettingsWidget::VisualMode mode)
{
    QColor new_color = color;
    switch (mode) {
    case (SettingsWidget::VisualMode::Normal): {
    } break;
    case (SettingsWidget::VisualMode::DynamicRange): {
        new_color = Color::createDynamicRangeColor(value, min, max, color);
    } break;
    case (SettingsWidget::VisualMode::HeatMap): {
        new_color = Color::createCMapColor(value, min, max, Color::ColorGradients::gpSpectrum);
    } break;
    case (SettingsWidget::VisualMode::ColorRange): {
        new_color = Color::createCMapColor(value, min, max, Color::ColorGradients::gpHot);
    }
    }
    return new_color;
}
}

#include "HeatMap.h"

#include <QImage>
#include <QColor>

namespace Color
{

void createHeatMapImage(QImage &image,
                        const float lowerbound,
                        const float upperbound,
                        const Visual::GeneColorMode &colorMode)
{
    // TODO it appears from now that the color mode must be disregarded as the color
    // spectra for the legend using a linear function should be correct for other color modes
    // Alternatively, adjusted_Value, lowerbound and upperbound can be transformed using colorMode
    Q_UNUSED(colorMode);

    const unsigned height = image.height();
    const unsigned width = image.width();

    for (unsigned y = 0; y < height; ++y) {
        // get the color of each line of the image as the heatmap
        // color normalized to the lower and upper bound of the image
        const unsigned value = height - y - 1;
        const float adjusted_value
            = Math::linearConversion<float, float>(static_cast<float>(value),
                                                   0.0,
                                                   static_cast<float>(height),
                                                   lowerbound,
                                                   upperbound);
        const float normalizedValue
            = Math::norm<float, float>(adjusted_value, lowerbound, upperbound);
        const QColor color = Color::createHeatMapWaveLenghtColor(normalizedValue);
        const QRgb rgb_color = color.rgb();
        for (unsigned x = 0; x < width; ++x) {
            image.setPixel(x, y, rgb_color);
        }
    }
}

// simple function that computes color from a min-max range
// using linear Interpolation
QColor createHeatMapLinearColor(const double value, const double min, const double max)
{
    const double halfmax = (min + max) / 2;
    const double blue = std::max(0.0, 255 * (1 - (value / halfmax)));
    const double red = std::max(0.0, 255 * ((value / halfmax) - 1));
    const double green = 255 - blue - red;
    return QColor::fromRgb(red, green, blue);
}

// simple function that computes color from a value
// using the human wave lenght spectra
QColor createHeatMapWaveLenghtColor(const float value)
{
    static const float gamma = 0.8;

    // denorm value to range (380-780)
    const float cwavelength = Math::denorm(value, 380.0, 780.0);

    // define colors according to wave lenght spectra
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

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
    float factor = 0.3;
    if (380.0 <= cwavelength && cwavelength < 420.0) {
        factor = 0.3 + 0.7 * (cwavelength - 380.0) / (420.0 - 380.0);
    } else if (420.0 <= cwavelength && cwavelength < 700.0) {
        factor = 1.0;
    } else if (700.0 <= cwavelength && cwavelength <= 780.0) {
        factor = 0.3 + 0.7 * (780.0 - cwavelength) / (780.0 - 700.0);
    }

    // Gamma adjustments (clamp to [0.0, 1.0])
    red = Math::clamp(qPow(red * factor, gamma), 0.0, 1.0);
    green = Math::clamp(qPow(green * factor, gamma), 0.0, 1.0);
    blue = Math::clamp(qPow(blue * factor, gamma), 0.0, 1.0);

    // return color
    return QColor::fromRgbF(red, green, blue, 1.0);
}

// normalizes a value to wave lenghts range using different modes (to be used
// with the function above)
float normalizeValueSpectrumFunction(const float value, const Visual::GeneColorMode &colorMode)
{
    float transformedValue = value;

    switch (colorMode) {
    case Visual::LogColor:
        transformedValue = std::log1p(value);
        break;
    case Visual::ExpColor:
        transformedValue = qSqrt(value);
        break;
    case Visual::LinearColor:
    default:
        // do nothing
        break;
    }

    return transformedValue;
}
}

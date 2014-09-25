/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HeatMap.h"

#include <QImage>
#include <QColor>
#include <QColor4ub>

void Heatmap::createHeatMapImage(QImage &image,
                                 const int lowerbound,
                                 const int upperbound,
                                 const ColorMode colorMode,
                                 const SpectrumMode mode)
{
    const int height = image.height();
    const int width = image.width();

    for (int y = 0; y < height; ++y) {
        //get the color of each line of the image as the heatmap
        //color normalized to the lower and upper bound of the image
        QColor4ub color;
        const int value = height - y - 1;
        if (colorMode == SpectrumRaibow) {
            const qreal normalizedValue = STMath::norm<qreal,int>(value, lowerbound, upperbound);
            const qreal adjusted = Heatmap::normalizeValueSpectrumFunction(normalizedValue, mode);
            const qreal wavelength = STMath::denorm(adjusted, 380.0, 780.0);
            color = Heatmap::createHeatMapWaveLenghtColor(wavelength);
        } else {
            const qreal normalizedValue = Heatmap::normalizeValueSpectrumFunction(value, mode);
            color = Heatmap::createHeatMapLinearColor(normalizedValue, lowerbound, upperbound);
        }

        const QRgb rgb_color = color.toColor().rgb();
        for(int x = 0; x < width; ++x) {
            image.setPixel(x, y, rgb_color);
        }
    }
}

//simple function that computes color from a min-max range
//using linear Interpolation
QColor4ub Heatmap::createHeatMapLinearColor(const int value,
                                            const int min,
                                            const int max)
{
    const qreal halfmax = (min + max) / 2;
    const int blue = std::max(0.0, 255 * (1 - (value / halfmax)));
    const int red = std::max(0.0, 255 * ((value / halfmax) - 1));
    const int green = 255 - blue - red;
    return QColor4ub::fromRgb(red, green, blue);
}

//simple function that computes color from a value
//using the human wave lenght spectra
QColor4ub Heatmap::createHeatMapWaveLenghtColor(const qreal value)
{
    static const qreal gamma = 0.8;

    // assert value is in range (380-780)
    const qreal cwavelength = STMath::clamp(value, 380.0, 780.0);

    // define colors according to wave lenght spectra
    qreal red = 0.0;
    qreal green = 0.0;
    qreal blue = 0.0;

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
    qreal factor = 0.3;
    if (380.0 <= cwavelength && cwavelength < 420.0) {
        factor = 0.3 + 0.7 * (cwavelength - 380.0) / (420.0 - 380.0);
    } else if (420.0 <= cwavelength && cwavelength < 700.0) {
        factor = 1.0;
    } else if (700.0 <= cwavelength && cwavelength <= 780.0) {
        factor = 0.3 + 0.7 * (780.0 - cwavelength) / (780.0 - 700.0);
    }

    // Gamma adjustments (clamp to [0.0, 1.0])
    red = STMath::clamp(qPow(red * factor, gamma), 0.0, 1.0);
    green = STMath::clamp(qPow(green * factor, gamma), 0.0, 1.0);
    blue = STMath::clamp(qPow(blue * factor, gamma), 0.0, 1.0);

    // return color
    return QColor4ub::fromRgbF(red, green, blue, 1.0);
}

//normalizes a value to wave lenghts range using different modes (to be used
//with the function above)
qreal Heatmap::normalizeValueSpectrumFunction(const qreal value, const SpectrumMode mode)
{
    qreal transformedValue = value;

    switch (mode) {
    case Heatmap::SpectrumLog:
        //value = qLn(value + 1.0) * 1.442695;
        transformedValue = std::log(value + 1.0);
        break;
    case Heatmap::SpectrumExp:
        transformedValue = qSqrt(value);
        break;
    case Heatmap::SpectrumLinear:
    default:
        // do nothing
        break;
    }

    return transformedValue;
}

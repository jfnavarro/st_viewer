/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HeatMap.h"

#include <QImage>
#include <QColor>
#include <QColor4ub>
#include "utils/Utils.h"

void Heatmap::createHeatMapImage(QImage &image, const SpectrumMode mode,
                                 int lowerbound, int upperbound)
{
    const int h = image.height();
    const int w = image.width();
    for (int y = 0; y < h; ++y) {
        //I want to get the color of each line of the image as the heatmap
        //color normalized to the lower and upper bound
        const qreal nh = STMath::norm<int,qreal>(h - y - 1, lowerbound, upperbound);
        const qreal nw = Heatmap::generateHeatMapWavelength(nh, mode);
        const QColor4ub color = Heatmap::createHeatMapColor(nw);
        const QRgb rgb_color = color.toColor().rgb();
        for(int x = 0; x < w; ++x) {
            image.setPixel(x, y, rgb_color);
        }
    }
}

QColor4ub Heatmap::createHeatMapColor(const qreal wavelength)
{
    static const qreal gamma = 0.8f;

    // clamp input value
    const qreal cwavelength = STMath::clamp(wavelength, 380.0, 780.0);

    // define colors according to wave lenght spectra
    qreal red;
    qreal green;
    qreal blue;

    if (380.0 <= cwavelength && cwavelength < 440.0) {
        red = -(cwavelength - 440.0) / (440.0f - 380.0);
        green = 0.0;
        blue = 1.0;
    } else if (440.0 <= cwavelength && cwavelength < 490.0) {
        red = 0.0;
        green = (cwavelength - 440.0) / (490.0 - 440.0);
        blue = 1.0;
    } else if (490.0f <= cwavelength && cwavelength < 510.0f) {
        red = 0.0f;
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
    } else {
        red = 0.0;
        green = 0.0;
        blue = 0.0;
    }

    // Let the intensity fall off near the vision limits
    qreal factor;
    if (380.0 <= cwavelength && cwavelength < 420.0) {
        factor = 0.3 + 0.7 * (cwavelength - 380.0) / (420.0 - 380.0);
    } else if (420.0 <= cwavelength && cwavelength < 700.0) {
        factor = 1.0;
    } else if (700.0 <= cwavelength && cwavelength <= 780.0) {
        factor = 0.3 + 0.7 * (780.0 - cwavelength) / (780.0 - 700.0);
    } else {
        factor = 0.3f;
    }

    // Gamma adjustments (clamp to [0.0, 1.0])
    red = STMath::clamp(qPow(red * factor, gamma), 0.0, 1.0);
    green = STMath::clamp(qPow(green * factor, gamma), 0.0, 1.0);
    blue = STMath::clamp(qPow(blue * factor, gamma), 0.0, 1.0);

    // return color
    return QColor4ub::fromRgbF(red, green, blue, 1.0);
}

qreal Heatmap::generateHeatMapWavelength(const qreal t, const SpectrumMode mode)
{
    // assert normalized value
    qreal nt = STMath::clamp(t, 0.0, 1.0);

    switch (mode) {
    case Heatmap::SpectrumLog:
        nt = qLn(nt + 1.0) * 1.442695; //NOTE [0,1] -> [0,1]
        break;
    case Heatmap::SpectrumExp:
        nt = qSqrt(nt);
        break;
    case Heatmap::SpectrumLinear:
    default:
        // do nothing
        break;
    }
    return STMath::denorm(nt, 380.0, 780.0);
}

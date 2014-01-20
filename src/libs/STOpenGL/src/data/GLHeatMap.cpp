/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLHeatMap.h"
#include "utils/MathExtended.h"

#include <QImage>
#include <QColor>
#include <QColor4ub>

namespace GL
{

void GLheatmap::createHeatMapImage(QImage &image, const SpectrumMode mode,
        int lowerbound, int upperbound)
{
    int h = image.height();
    int w = image.width();
    for (int i = 0; i < h; ++i) {
        //I want to get the color of each line of the image as the heatmap
        //color normalized to the lower and upper bound
        const qreal nh = GL::norm<int,qreal>(h - i - 1, lowerbound, upperbound);
        const qreal nw = GLheatmap::generateHeatMapWavelength(nh, mode);
        const QColor4ub color = GLheatmap::createHeatMapColor(nw);
        for(int j = 0; j < w; ++j) {
            image.setPixel(i, j, color.toColor().rgb());
        }
    }
}

QColor4ub GLheatmap::createHeatMapColor(const qreal wavelength)
{
    static const qreal gamma = 0.8f;

    // clamp input value
    const qreal cwavelength = GL::clamp(wavelength, 380.0, 780.0);

    // define colors according to wave lenght spectra
    qreal red;
    qreal green;
    qreal blue;

    if (380.0f <= cwavelength && cwavelength < 440.0f) {
        red = -(cwavelength - 440.0f) / (440.0f - 380.0f);
        green = 0.0f;
        blue = 1.0f;
    } else if (440.0f <= cwavelength && cwavelength < 490.0f) {
        red = 0.0f;
        green = (cwavelength - 440.0f) / (490.0f - 440.0f);
        blue = 1.0f;
    } else if (490.0f <= cwavelength && cwavelength < 510.0f) {
        red = 0.0f;
        green = 1.0f;
        blue = -(cwavelength - 510.0f) / (510.0f - 490.0f);
    } else if (510.0f <= cwavelength && cwavelength < 580.0f) {
        red = (cwavelength - 510.0f) / (580.0f - 510.0f);
        green = 1.0f;
        blue = 0.0f;
    } else if (580.0f <= cwavelength && cwavelength < 645.0f) {
        red = 1.0f;
        green = -(cwavelength - 645.0f) / (645.0f - 580.0f);
        blue = 0.0f;
    } else if (645.0f <= cwavelength && cwavelength <= 780.0f) {
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    } else {
        red = 0.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    // Let the intensity fall off near the vision limits
    qreal factor;
    if (380.0f <= cwavelength && cwavelength < 420.0f) {
        factor = 0.3f + 0.7f * (cwavelength - 380.0f) / (420.0f - 380.0f);
    } else if (420.0f <= cwavelength && cwavelength < 700.0f) {
        factor = 1.0f;
    } else if (700.0f <= cwavelength && cwavelength <= 780.0f) {
        factor = 0.3f + 0.7f * (780.0f - cwavelength) / (780.0f - 700.0f);
    } else {
        factor = 0.3f;
    }

    // Gamma adjustments (clamp to [0.0, 1.0])
    red = GL::clamp(qPow(red * factor, gamma), 0.0, 1.0);
    green = GL::clamp(qPow(green * factor, gamma), 0.0, 1.0);
    blue = GL::clamp(qPow(blue * factor, gamma), 0.0, 1.0);

    // return color
    return QColor4ub(red, green, blue);
}

qreal GLheatmap::generateHeatMapWavelength(const qreal t, const SpectrumMode mode)
{
    // assert normalized value
    qreal nt = GL::clamp(t, 0.0, 1.0);

    switch (mode) {
    case GLheatmap::SpectrumLog:
        nt = qLn(nt + 1.0) * 1.442695f; //NOTE [0,1] -> [0,1]
        break;
    case GLheatmap::SpectrumExp:
        nt = qSqrt(nt);
        break;
    case GLheatmap::SpectrumLinear:
    default:
        // do nothing
        break;
    }
    return GL::denorm(nt, 380.0, 780.0);
}

} // namespace GL //

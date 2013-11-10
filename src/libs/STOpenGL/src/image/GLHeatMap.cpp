/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <math/GLMath.h>
#include <image/GLImageWriter.h>

#include "GLHeatMap.h"

namespace GL
{

const bool GLheatmap::createHeatMapImage(GLimage &image, const SpectrumMode mode,
                                         int lowerbound, int upperbound)
{
    bool ret;
    GLimagewriter writer(image);
    GLsizei h = image.height();
    for (GLsizei i = 0; i<h; ++i)
    {
        //const GLfloat nh = norm<GLsizei,GLfloat>(h-i-1, 0, h-1);
        //I want to get the color of each line of the image as the heatmap
        //color normalized to the lower and upper bound
        const GLfloat nh = norm<GLsizei,GLfloat>(h-i-1,lowerbound,upperbound);
        const GLfloat nw = GLheatmap::generateHeatMapWavelength(nh, mode);
        const GLcolor color = GLheatmap::createHeatMapColor(nw);
        if (!(ret = writer.writeLine(color)))
        {
            break;
        }
    }

    return ret;
}

const GLcolor GLheatmap::createHeatMapColor(const GLfloat wavelength)
{
    const GLfloat gamma = 0.8f;

    // clamp input value
    const GLfloat cwavelength = clamp(wavelength, 380.0f, 780.0f);

    // define colors according to wave lenght spectra
    GLfloat red, green, blue;
    if (380.0f <= cwavelength && cwavelength < 440.0f)
    {
        red = -(cwavelength - 440.0f) / (440.0f - 380.0f);
        green = 0.0f;
        blue = 1.0f;
    }
    else if (440.0f <= cwavelength && cwavelength < 490.0f)
    {
        red = 0.0f;
        green = (cwavelength - 440.0f) / (490.0f - 440.0f);
        blue = 1.0f;
    }
    else if (490.0f <= cwavelength && cwavelength < 510.0f)
    {
        red = 0.0f;
        green = 1.0f;
        blue = -(cwavelength - 510.0f) / (510.0f - 490.0f);
    }
    else if (510.0f <= cwavelength && cwavelength < 580.0f)
    {
        red = (cwavelength - 510.0f) / (580.0f - 510.0f);
        green = 1.0f;
        blue = 0.0f;
    }
    else if (580.0f <= cwavelength && cwavelength < 645.0f)
    {
        red = 1.0f;
        green = -(cwavelength - 645.0f) / (645.0f - 580.0f);
        blue = 0.0f;
    }
    else if (645.0f <= cwavelength && cwavelength <= 780.0f)
    {
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    }
    else
    {
        red = 0.0f;
        green = 0.0f;
        blue = 0.0f;
    }

    // Let the intensity fall off near the vision limits
    GLfloat factor;
    if (380.0f <= cwavelength && cwavelength < 420.0f)
    {
        factor = 0.3f + 0.7f * (cwavelength - 380.0f) / (420.0f - 380.0f);
    }
    else if (420.0f <= cwavelength && cwavelength < 700.0f)
    {
        factor = 1.0f;
    }
    else if (700.0f <= cwavelength && cwavelength <= 780.0f)
    {
        factor = 0.3f + 0.7f * (780.0f - cwavelength) / (780.0f - 700.0f);
    }
    else
    {
        factor = 0.3f;
    }

    // Gamma adjustments (clamp to [0.0, 1.0])
    red = clamp((GLfloat) qPow(red * factor, gamma), 0.0f, 1.0f);
    green = clamp((GLfloat) qPow(green * factor, gamma), 0.0f, 1.0f);
    blue = clamp((GLfloat) qPow(blue * factor, gamma), 0.0f, 1.0f);

    // return color
    return GLcolor(red, green, blue);
}

const GLfloat GLheatmap::generateHeatMapWavelength(const GLfloat t, const SpectrumMode mode)
{
    // assert normalized value
    GLfloat nt = GL::clamp(t, GLfloat(0.0), GLfloat(1.0));

    switch (mode)
    {
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

    return GL::denorm(nt, GLfloat(380.0), GLfloat(780.0));
}

} // namespace GL //

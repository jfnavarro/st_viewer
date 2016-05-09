#ifndef HEATMAP_H
#define HEATMAP_H

#include "math/Common.h"
#include "SettingsVisual.h"

class QImage;

// Heatmap is a convenience namespace containing functions to generate
// heatmap related images and data.
namespace Color
{

enum SpectrumMode { SpectrumLinear, SpectrumLog, SpectrumExp };

enum InterpolationColorMode { SpectrumRaibow, SpectrumLinearInterpolation };

// Convenience function to generate a heatmap spectrum image given specific
// mapping function
// using the wave lenght spectra or a linear interpolation spectra between two
// colors
// the input image will be transformed with the new colors
void createHeatMapImage(QImage &image,
                        const float lowerbound,
                        const float upperbound,
                        const Visual::GeneColorMode &colorMode);

// Convenience function to generate a QColor color from a real value
QColor createHeatMapWaveLenghtColor(const float value);

// Convenience function to generate a QColor color from a real value given a
// range
QColor createHeatMapLinearColor(const float value, const float min, const float max);

// Convenience function to adjust the input value using a Linear - Exponential
// or Logaritmic
// function
float normalizeValueSpectrumFunction(const float value, const Visual::GeneColorMode &colorMode);
}

#endif // HEATMAP_H //

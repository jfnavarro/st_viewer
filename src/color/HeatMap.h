/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HEATMAP_H
#define HEATMAP_H

#include "math/Common.h"

class QColor4ub;
class QImage;

// Heatmap is a convenience class containing functions to generate
// heatmap related data.
class Heatmap
{

public:

    enum SpectrumMode {
        SpectrumLinear,
        SpectrumLog,
        SpectrumExp
    };

    enum ColorMode {
        SpectrumRaibow,
        SpectrumLinearInterpolation
    };

    // convenience function to generate a heatmap spectrum image given specific mapping function
    // using the wave lenght spectra or a linear interpolation spectra between two colors
    // the input image will be transformed with the new colors 
    static void createHeatMapImage(QImage &image,
                                   const int lowerbound,
                                   const int upperbound,
                                   const ColorMode colorMode = SpectrumRaibow,
                                   const SpectrumMode mode = SpectrumLinear);

    // convenience function to generate a QColor4ub color from a real value
    static QColor4ub createHeatMapWaveLenghtColor(const qreal value);

    // convenience function to generate a QColor4ub color from a real value given a range
    static QColor4ub createHeatMapLinearColor(const int value,
                                              const int min,
                                              const int max);

    // convenience function to adjust the input value using a Liner - Exponential or Logaritmic
    // function
    static qreal normalizeValueSpectrumFunction(const qreal value,
                                                const SpectrumMode mode = SpectrumLinear);
};

#endif // HEATMAP_H //

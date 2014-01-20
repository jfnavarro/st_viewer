/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLHEATMAP_H
#define GLHEATMAP_H

#include "GLCommon.h"

class QColor4ub;
class QImage;

namespace GL
{
// GLheatmap is a convenience class containing functions to generate
// heatmap related data.
class GLheatmap
{

public:

    enum SpectrumMode {
        SpectrumLinear,
        SpectrumLog,
        SpectrumExp
    };

    // convenience function to generate a heatmap spectrum image given specific mapping function
    static void createHeatMapImage(QImage &image, const SpectrumMode mode = SpectrumLinear,
                                         int lowerbound = 0, int upperbound = 100);

    static QColor4ub createHeatMapColor(const qreal wavelength);

    // convenience function to generate a heat map wavelength
    // based on a normalized value [0,1] and a distribution function.
    static qreal generateHeatMapWavelength(const qreal t, const SpectrumMode mode = SpectrumLinear);
};

} // namespace GL //

#endif // GLHEATMAP_H //
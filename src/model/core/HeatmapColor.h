/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef HEATMAPCOLOR_H
#define HEATMAPCOLOR_H

#include "model/core/ColorScheme.h"

// render feature with color from normalized hit count mapped to HSV color space
class HeatMapColor : public ColorScheme
{
public:
    explicit HeatMapColor(int minHits, int maxHits);
    virtual QColor getColor(DataProxy::FeatureRef feature) const;
};

#endif // HEATMAPCOLOR_H

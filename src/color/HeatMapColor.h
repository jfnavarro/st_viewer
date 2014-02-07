/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef HEATMAPCOLOR_H
#define HEATMAPCOLOR_H

#include "color/ColorScheme.h"

// render feature with color from normalized hit count mapped to HSV color space
class HeatMapColor : public ColorScheme
{
public:
    
    HeatMapColor();
    virtual ~HeatMapColor();
    
    QColor getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const;
};

#endif // HEATMAPCOLOR_H

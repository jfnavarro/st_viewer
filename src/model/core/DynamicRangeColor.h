/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef DYNAMICRANGECOLOR_H
#define DYNAMICRANGECOLOR_H

#include "model/core/ColorScheme.h"

// render feature with feature color but set alpha to normalized hit count
class DynamicRangeColor : public ColorScheme
{
public:
    DynamicRangeColor(int minHits, int maxHits);
    virtual QColor getColor(DataProxy::FeatureRef feature) const;
};

#endif // DYNAMICRANGECOLOR_H

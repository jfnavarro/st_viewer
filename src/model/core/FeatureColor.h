/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef FEATURECOLOR_H
#define FEATURECOLOR_H

#include "model/core/ColorScheme.h"

// render feature according to feature color
class FeatureColor : public ColorScheme
{
public:
    FeatureColor(int minHits, int maxHits);
    virtual QColor getColor(DataProxy::FeatureRef feature) const;
};

#endif // FEATURECOLOR_H

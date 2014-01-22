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

    FeatureColor();
    virtual ~FeatureColor();

    virtual QColor getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const;
};

#endif // FEATURECOLOR_H

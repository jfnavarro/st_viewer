/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef DYNAMICRANGECOLOR_H
#define DYNAMICRANGECOLOR_H

#include "color/ColorScheme.h"

// render feature with feature color but set alpha to normalized hit count
class DynamicRangeColor : public ColorScheme
{
public:

    DynamicRangeColor();
    virtual ~DynamicRangeColor();

    QColor getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const;
};

#endif // DYNAMICRANGECOLOR_H

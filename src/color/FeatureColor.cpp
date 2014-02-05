/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "color/FeatureColor.h"

FeatureColor::FeatureColor() : ColorScheme()
{

}

FeatureColor::~FeatureColor()
{

}

QColor FeatureColor::getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const
{
    Q_UNUSED(m_min);
    Q_UNUSED(m_max);
    //qreal alpha = (feature->hits() > max || feature->hits() < min) ? 0.0 : 1.0;
    //QColor adjusted = feature->color();
    //adjusted.setAlphaF(alpha);
    //return adjusted;
    return feature->color();
}

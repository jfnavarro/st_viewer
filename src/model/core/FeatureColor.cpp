/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "FeatureColor.h"

FeatureColor::FeatureColor(int minHits, int maxHits) : ColorScheme(minHits,maxHits)
{

}

QColor FeatureColor::getColor(DataProxy::FeaturePtr feature) const
{
    qreal alpha = 1.0;
    if(feature->hits() > m_maxHits || feature->hits() < m_minHits )
    {
        alpha = 0.0;
    }
    QColor adjusted = feature->color();
    adjusted.setAlphaF(alpha);
    return adjusted;
}

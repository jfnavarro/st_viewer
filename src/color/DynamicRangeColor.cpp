/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "color/DynamicRangeColor.h"

#include "math/Common.h"
#include "HeatMap.h"

DynamicRangeColor::DynamicRangeColor() : ColorScheme()
{

}

DynamicRangeColor::~DynamicRangeColor()
{

}

QColor DynamicRangeColor::getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const
{
    const qreal v = qreal(feature->hits());
    const qreal min = qreal(m_min);
    const qreal max = qreal(m_max);
    const qreal nv = qSqrt(STMath::norm<qreal, qreal>(v, min, max));
    QColor color = feature->color();
    color.setAlphaF(nv);
    return color;
}

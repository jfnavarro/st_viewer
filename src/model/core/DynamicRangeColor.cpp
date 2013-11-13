/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "DynamicRangeColor.h"
#include "GLCommon.h"
#include "math/GLFloat.h"
#include "image/GLHeatMap.h"
#include "GLColor.h"
#include "GLQt.h"

DynamicRangeColor::DynamicRangeColor(int minHits, int maxHits) : ColorScheme(minHits,maxHits)
{

}

QColor DynamicRangeColor::getColor(DataProxy::FeaturePtr feature) const
{
    const qreal v = qreal(feature->hits());
    const qreal min = qreal(m_minHits);
    const qreal max = qreal(m_maxHits);
    const qreal nv = qSqrt(GL::norm<qreal,qreal>(v, min, max));

    QColor color = feature->color();
    color.setAlphaF(nv);
    return color;
}

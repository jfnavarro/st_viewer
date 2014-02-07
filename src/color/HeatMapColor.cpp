/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "color/HeatMapColor.h"

#include "HeatMap.h"
#include "math/Common.h"

#include <QColor4ub>

HeatMapColor::HeatMapColor() : ColorScheme()
{
    
}

HeatMapColor::~HeatMapColor()
{
    
}

QColor HeatMapColor::getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const
{
    const qreal nv = STMath::norm<int,qreal>(feature->hits(), m_min, m_max);
    const qreal waveLength = Heatmap::generateHeatMapWavelength(nv, Heatmap::SpectrumExp);
    return QColor4ub(Heatmap::createHeatMapColor(waveLength)).toColor();
}

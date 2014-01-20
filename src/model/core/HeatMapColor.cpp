/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "HeatMapColor.h"

#include "data/GLHeatMap.h"
#include "utils/MathExtended.h"

#include <QColor4ub>

HeatMapColor::HeatMapColor() : ColorScheme()
{
    
}

HeatMapColor::~HeatMapColor()
{
    
}

QColor HeatMapColor::getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const
{
    const qreal nv = GL::norm<int,qreal>(feature->hits(), m_min, m_max);
    const qreal waveLength = GL::GLheatmap::generateHeatMapWavelength(nv, GL::GLheatmap::SpectrumExp);
    return QColor4ub(GL::GLheatmap::createHeatMapColor(waveLength)).toColor();
}

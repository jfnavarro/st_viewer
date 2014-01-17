/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "HeatMapColor.h"

#include "data/GLHeatMap.h"
#include "GLColor.h"

#include "utils/MathExtended.h"

HeatMapColor::HeatMapColor() : ColorScheme()
{
    
}

HeatMapColor::~HeatMapColor()
{
    
}

QColor HeatMapColor::getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const
{
    const GLfloat nv = GL::norm<GLfloat,int>(feature->hits(), m_min, m_max);
    const GLfloat waveLength = GL::GLheatmap::generateHeatMapWavelength(nv, GL::GLheatmap::SpectrumExp);
    return GL::toQColor(GL::GLheatmap::createHeatMapColor(waveLength));
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "HeatMapColor.h"

#include "GLCommon.h"
#include "math/GLFloat.h"
#include "image/GLHeatMap.h"
#include "GLColor.h"
#include "GLQt.h"

HeatMapColor::HeatMapColor() : ColorScheme()
{
    
}

HeatMapColor::~HeatMapColor()
{
    
}

QColor HeatMapColor::getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const
{
    const GLfloat v = GLfloat(feature->hits());
    const GLfloat min = GLfloat(m_min);
    const GLfloat max = GLfloat(m_max);
    const GLfloat nv = GL::norm<GLfloat,GLfloat>(v, min, max);
    const GLfloat waveLength = GL::GLheatmap::generateHeatMapWavelength(nv, GL::GLheatmap::SpectrumExp);
    return GL::toQColor(GL::GLheatmap::createHeatMapColor(waveLength));
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "HeatmapColor.h"
#include "GLCommon.h"
#include "math/GLFloat.h"
#include "image/GLHeatMap.h"
#include "GLColor.h"
#include "GLQt.h"

HeatMapColor::HeatMapColor(int minHits, int maxHits) : ColorScheme(minHits,maxHits)
{

}

QColor HeatMapColor::getColor(const DataProxy::FeaturePtr feature) const
{
    const GLfloat v = GLfloat(feature->hits());
    const GLfloat min = GLfloat(m_minHits);
    const GLfloat max = GLfloat(m_maxHits);
    const GLfloat nv = GL::norm<GLfloat,GLfloat>(v, min, max);
    const GLfloat waveLength = GL::GLheatmap::generateHeatMapWavelength(nv, GL::GLheatmap::SpectrumExp);
    return GL::toQColor(GL::GLheatmap::createHeatMapColor(waveLength));
}

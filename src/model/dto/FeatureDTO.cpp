/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "FeatureDTO.h"

FeatureDTO::FeatureDTO(QObject* parent) : QObject(parent), m_feature()
{
    
}

FeatureDTO::FeatureDTO(const Feature& feature, QObject* parent) : QObject(parent), m_feature(feature)
{
    
}

FeatureDTO::~FeatureDTO() { }

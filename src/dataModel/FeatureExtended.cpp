/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "FeatureExtended.h"

#include "utils/Utils.h"

FeatureExtended::FeatureExtended()
    : Feature(),
      m_color(Globals::DEFAULT_COLOR_GENE),
      m_selected(false)
{

}

FeatureExtended::FeatureExtended(const Feature& other)
    : Feature(other),
      m_color(Globals::DEFAULT_COLOR_GENE),
      m_selected(false)
{

}

FeatureExtended::FeatureExtended(const FeatureExtended& other)
    : Feature(other),
      m_color(other.m_color),
      m_selected(other.m_selected)

{

}

FeatureExtended::~FeatureExtended()
{

}

FeatureExtended& FeatureExtended::operator=(const Feature& other)
{
    Feature::operator=(other);
    m_color = QColor(Globals::DEFAULT_COLOR_GENE);
    m_selected = false;
    return (*this);
}
FeatureExtended& FeatureExtended::operator=(const FeatureExtended& other)
{
    Feature::operator=(other);
    m_color = other.m_color;
    m_selected = other.m_selected;
    return (*this);
}

bool FeatureExtended::operator==(const FeatureExtended& other) const
{
    return (Feature::operator==(other) && (m_color == other.m_color)
            && m_selected == other.m_selected);
}

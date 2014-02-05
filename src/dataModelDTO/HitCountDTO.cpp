/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HitCountDTO.h"

HitCountDTO::HitCountDTO(QObject* parent) : QObject(parent), m_hitCount()
{

}

HitCountDTO::HitCountDTO(const HitCount& hitCount, QObject* parent) : QObject(parent), m_hitCount(hitCount)
{

}

HitCountDTO::~HitCountDTO()
{

}

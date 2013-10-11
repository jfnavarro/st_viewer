/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ChipDTO.h"

ChipDTO::ChipDTO(QObject* parent) : QObject(parent), m_chip()
{
    
}
ChipDTO::ChipDTO(const Chip& chip, QObject* parent) : QObject(parent), m_chip(chip)
{
    
}

ChipDTO::~ChipDTO()
{
    
}

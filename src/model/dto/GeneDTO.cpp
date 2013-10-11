/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneDTO.h"

GeneDTO::GeneDTO(QObject* parent) : QObject(parent), m_gene()
{
    
}

GeneDTO::GeneDTO(const Gene& gene, QObject* parent) : QObject(parent), m_gene(gene)
{
    
}

GeneDTO::~GeneDTO()
{
    
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "model/Feature.h"

Feature::Feature()
    : m_id(""), m_barcode(""), m_gene(""),
      m_hits(0), m_x(0), m_y(0)

{

}

Feature::Feature(const Feature& other)
{
    m_id = other.m_id;
    m_barcode = other.m_barcode;
    m_gene = other.m_gene;
    m_hits = other.m_hits;
    m_x = other.m_x;
    m_y = other.m_y;
}

Feature::~Feature()
{

}

Feature& Feature::operator=(const Feature& other)
{
    m_id = other.m_id;
    m_barcode = other.m_barcode;
    m_gene = other.m_gene;
    m_hits = other.m_hits;
    m_x = other.m_x;
    m_y = other.m_y;
    return (*this);
}

bool Feature::operator==(const Feature& other) const
{
    if (m_barcode == other.m_barcode &&
            m_gene == other.m_gene &&
            m_hits == other.m_hits &&
            m_x == other.m_x &&
            m_y == other.m_y )
        return true;
    else
        return false;
}




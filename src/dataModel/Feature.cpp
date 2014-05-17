/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "dataModel/Feature.h"

#include "utils/Utils.h"

Feature::Feature()
    : m_id(),
      m_barcode(),
      m_gene(),
      m_annotation(),
      m_hits(0),
      m_x(0),
      m_y(0),
      m_color(Globals::DEFAULT_COLOR_GENE),
      m_selected(false)

{

}

Feature::Feature(const Feature& other)
{
    m_id = other.m_id;
    m_barcode = other.m_barcode;
    m_gene = other.m_gene;
    m_annotation = other.m_annotation;
    m_hits = other.m_hits;
    m_x = other.m_x;
    m_y = other.m_y;
    m_color = other.m_color;
    m_selected = other.m_selected;
}

Feature::~Feature()
{

}

Feature& Feature::operator=(const Feature& other)
{
    m_id = other.m_id;
    m_barcode = other.m_barcode;
    m_gene = other.m_gene;
    m_annotation = other.m_annotation;
    m_hits = other.m_hits;
    m_x = other.m_x;
    m_y = other.m_y;
    m_color = other.m_color;
    m_selected = other.m_selected;
    return (*this);
}

bool Feature::operator==(const Feature& other) const
{
    return (m_id == other.m_id &&
            m_barcode == other.m_barcode &&
            m_gene == other.m_gene &&
            m_annotation == other.m_annotation &&
            m_hits == other.m_hits &&
            m_x == other.m_x &&
            m_y == other.m_y &&
            m_color == other.m_color &&
            m_selected == other.m_selected
            );
}




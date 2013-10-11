/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Gene.h"

Gene::Gene()
    : m_id(""), m_name(""), m_color(Globals::color_gene), m_selected(false)
{

}

Gene::Gene(const Gene& other)
    : m_id(other.m_id), m_name(other.m_name),
      m_color(other.m_color), m_selected(other.m_selected)
{

}

Gene::Gene(const QString& id, const QString& name, bool hidden, QColor color)
    : m_id(id), m_name(name), m_color(color), m_selected(hidden)
{

}
Gene::~Gene() 
{

}

Gene& Gene::operator=(const Gene& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_selected = other.m_selected;
    m_color = other.m_color;
    return (*this);
}

bool Gene::operator==(const Gene& other) const
{
    if (
        m_selected == other.m_selected &&
        m_name == other.m_name &&
        m_color == other.m_color
    )
        return true;
    else
        return false;
}

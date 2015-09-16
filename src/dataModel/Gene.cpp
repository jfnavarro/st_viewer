/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Gene.h"

Gene::Gene()
    : m_name()
    , m_color(Globals::DEFAULT_COLOR_GENE)
    , m_selected(false)
{
}

Gene::Gene(const Gene& other)
    : m_name(other.m_name)
    , m_color(other.m_color)
    , m_selected(other.m_selected)
{
}

Gene::Gene(const QString& name, bool selected, QColor color)
    : m_name(name)
    , m_color(color)
    , m_selected(selected)
{
}
Gene::~Gene()
{
}

Gene& Gene::operator=(const Gene& other)
{
    m_name = other.m_name;
    m_selected = other.m_selected;
    m_color = other.m_color;
    return (*this);
}

bool Gene::operator==(const Gene& other) const
{
    return (m_selected == other.m_selected && m_name == other.m_name && m_color == other.m_color);
}

const QString Gene::name() const
{
    return m_name;
}

bool Gene::selected() const
{
    return m_selected;
}

const QColor Gene::color() const
{
    return m_color;
}

void Gene::name(const QString& name)
{
    m_name = name;
}

void Gene::selected(bool selected)
{
    m_selected = selected;
}

void Gene::color(const QColor& color)
{
    m_color = color;
}

bool Gene::isAmbiguous() const
{
    return m_name.startsWith("ambiguous", Qt::CaseSensitive);
}

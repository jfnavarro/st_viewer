/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "dataModel/Feature.h"

#include "utils/Utils.h"

Feature::Feature()
    : m_barcode(),
      m_gene(),
      m_annotation(),
      m_hits(0),
      m_x(0),
      m_y(0),
      m_color(Globals::DEFAULT_COLOR_GENE)

{

}

Feature::Feature(const Feature& other)
{
    m_barcode = other.m_barcode;
    m_gene = other.m_gene;
    m_annotation = other.m_annotation;
    m_hits = other.m_hits;
    m_x = other.m_x;
    m_y = other.m_y;
    m_color = other.m_color;
}

Feature::~Feature()
{

}

Feature& Feature::operator=(const Feature& other)
{
    m_barcode = other.m_barcode;
    m_gene = other.m_gene;
    m_annotation = other.m_annotation;
    m_hits = other.m_hits;
    m_x = other.m_x;
    m_y = other.m_y;
    m_color = other.m_color;
    return (*this);
}

bool Feature::operator==(const Feature& other) const
{
    return (
            m_barcode == other.m_barcode &&
            m_gene == other.m_gene &&
            m_annotation == other.m_annotation &&
            m_hits == other.m_hits &&
            m_x == other.m_x &&
            m_y == other.m_y &&
            m_color == other.m_color
            );
}

const QString Feature::barcode() const
{
    return m_barcode;
}

const QString Feature::gene() const
{
    return m_gene;
}

const QString Feature::annotation() const
{
    return m_annotation;
}

int Feature::hits() const
{
    return m_hits;
}

double Feature::x() const
{
    return m_x;
}

double Feature::y() const
{
    return m_y;
}

void Feature::barcode(const QString& barcode)
{
    m_barcode = barcode;
}

void Feature::gene(const QString& gene)
{
    m_gene = gene;
}

void Feature::annotation(const QString& annotation)
{
    m_annotation = annotation;
}

void Feature::hits(int hits)
{
    m_hits = hits;
}

void Feature::x(double x)
{
    m_x = x;
}

void Feature::y(double y)
{
    m_y = y;
}

const QColor Feature::color() const
{
    return m_color;
}

void Feature::color(const QColor& color)
{
    m_color = color;
}

Feature::GenePtr Feature::geneObject() const
{
    Q_ASSERT(!m_geneObject.isNull());
    return m_geneObject;
}

void Feature::geneObject(GenePtr gene)
{
    Q_ASSERT(!gene.isNull());
    m_geneObject = gene;
}

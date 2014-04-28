#include "GeneSelection.h"

GeneSelection::GeneSelection()
    : m_name(),
      m_reads(0.0),
      m_normalizedReads(0.0)
{

}

GeneSelection::GeneSelection(const QString name, qreal reads, qreal normalizedReads)
    : m_name(name),
      m_reads(reads),
      m_normalizedReads(normalizedReads)
{

}

GeneSelection::GeneSelection(const GeneSelection& other)
    : m_name(other.m_name),
      m_reads(other.m_reads),
      m_normalizedReads(other.m_normalizedReads)
{

}

GeneSelection::~GeneSelection()
{

}

GeneSelection& GeneSelection::operator=(const GeneSelection& other)
{
    m_name = other.m_name;
    m_reads = other.m_reads;
    m_normalizedReads = other.m_normalizedReads;
    return (*this);
}

bool GeneSelection::operator==(const GeneSelection& other) const
{
    return( m_reads == other.m_reads &&
            m_name == other.m_name &&
            m_normalizedReads == other.m_normalizedReads
        );
}


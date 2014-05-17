/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Dataset.h"
#include <QStringList>
#include <QVariant>

Dataset::Dataset()
    : m_id(),
      m_name(),
      m_alignmentId(),
      m_statBarcodes(0),
      m_statGenes(0),
      m_statUniqueBarcodes(0),
      m_statUniqueGenes(0),
      m_statTissue(),
      m_statSpecie(),
      m_statComments()
{

}

Dataset::Dataset(const Dataset& other)
{
    m_name = other.m_name;
    m_id = other.m_id;
    m_alignmentId = other.m_alignmentId;
    m_statBarcodes = other.m_statBarcodes;
    m_statGenes = other.m_statGenes;
    m_statUniqueBarcodes = other.m_statUniqueBarcodes;
    m_statUniqueGenes = other.m_statUniqueGenes;
    m_statTissue = other.m_statTissue;
    m_statSpecie = other.m_statSpecie;
    m_statComments = other.m_statComments;
    m_oboFroundryTerms = other.m_oboFroundryTerms;
    m_genePooledHitsQuartiles = other.m_genePooledHitsQuartiles;
}

Dataset::~Dataset()
{

}

Dataset& Dataset::operator=(const Dataset& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_alignmentId = other.m_alignmentId;
    m_statBarcodes = other.m_statBarcodes;
    m_statGenes = other.m_statGenes;
    m_statUniqueBarcodes = other.m_statUniqueBarcodes;
    m_statUniqueGenes = other.m_statUniqueGenes;
    m_statTissue = other.m_statTissue;
    m_statSpecie = other.m_statSpecie;
    m_statComments = other.m_statComments;
    m_oboFroundryTerms = other.m_oboFroundryTerms;
    m_genePooledHitsQuartiles = other.m_genePooledHitsQuartiles;
    return (*this);
}

bool Dataset::operator==(const Dataset& other) const
{
    return (
               m_id == other.m_id &&
               m_name == other.m_name &&
               m_alignmentId == other.m_alignmentId &&
               m_statBarcodes == other.m_statBarcodes &&
               m_statGenes == other.m_statGenes &&
               m_statUniqueBarcodes == other.m_statUniqueBarcodes &&
               m_statUniqueGenes == other.m_statUniqueGenes &&
               m_statTissue == other.m_statTissue &&
               m_statSpecie == other.m_statSpecie &&
               m_statComments == other.m_statComments &&
               m_oboFroundryTerms == other.m_oboFroundryTerms &&
               m_genePooledHitsQuartiles == other.m_genePooledHitsQuartiles
           );
}

qreal Dataset::min() const
{
    Q_ASSERT(m_genePooledHitsQuartiles.size() == 5);
    // max ( q1 - 1.5 * (q3-q1), q0 )
    const qreal q0 = m_genePooledHitsQuartiles.at(0);
    const qreal q1 = m_genePooledHitsQuartiles.at(1);
    const qreal q3 = m_genePooledHitsQuartiles.at(3);
    return std::max(q1 - 1.5 * (q3 - q1), q0);
}

qreal Dataset::max() const
{
    Q_ASSERT(m_genePooledHitsQuartiles.size() == 5);
    // min ( q3 + 1.5 * (q3-q1), q4 )
    const qreal q4 = m_genePooledHitsQuartiles.at(4);
    const qreal q1 = m_genePooledHitsQuartiles.at(1);
    const qreal q3 = m_genePooledHitsQuartiles.at(3);
    return std::min(q3 + 1.5 * (q3 - q1), q4);
}

qreal Dataset::pooledMin() const
{
    Q_ASSERT(m_genePooledHitsQuartiles.size() == 5);
    // max ( q1 - 1.5 * (q3-q1), q0 )
    const qreal q0 = m_genePooledHitsQuartiles.at(0);
    const qreal q1 = m_genePooledHitsQuartiles.at(1);
    const qreal q3 = m_genePooledHitsQuartiles.at(3);
    return std::max(q1 - 1.5 * (q3 - q1), q0);
}

qreal Dataset::pooledMax() const
{
    Q_ASSERT(m_genePooledHitsQuartiles.size() == 5);
    // min ( q3 + 1.5 * (q3-q1), q4 )
    const qreal q4 = m_genePooledHitsQuartiles.at(4);
    const qreal q1 = m_genePooledHitsQuartiles.at(1);
    const qreal q3 = m_genePooledHitsQuartiles.at(3);
    return std::min(q3 + 1.5 * (q3 - q1), q4);
}

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
      m_chipId(),
      m_alignment(),
      m_figureBlue(),
      m_figureRed(),
      m_figureStatus(0),
      m_statBarcodes(0),
      m_statGenes(0),
      m_statUniqueBarcodes(0),
      m_statUniqueGenes(0),
      m_statTissue(),
      m_statSpecie(),
      m_statCreated(),
      m_statComments()
{

}

Dataset::Dataset(const Dataset& other)
{
    m_name = other.m_name;
    m_chipId = other.m_chipId;
    m_id = other.m_id;
    m_alignment = other.m_alignment;
    m_figureBlue = other.m_figureBlue;
    m_figureRed = other.m_figureRed;
    m_figureStatus = other.m_figureStatus;
    m_statBarcodes = other.m_statBarcodes;
    m_statGenes = other.m_statGenes;
    m_statUniqueBarcodes = other.m_statUniqueBarcodes;
    m_statUniqueGenes = other.m_statUniqueGenes;
    m_statTissue = other.m_statTissue;
    m_statSpecie = other.m_statSpecie;
    m_statCreated = other.m_statCreated;
    m_statComments = other.m_statComments;
}

Dataset::~Dataset()
{

}

Dataset& Dataset::operator=(const Dataset& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_chipId = other.m_chipId;
    m_alignment = other.m_alignment;
    m_figureBlue = other.m_figureBlue;
    m_figureRed = other.m_figureRed;
    m_figureStatus = other.m_figureStatus;
    m_statBarcodes = other.m_statBarcodes;
    m_statGenes = other.m_statGenes;
    m_statUniqueBarcodes = other.m_statUniqueBarcodes;
    m_statUniqueGenes = other.m_statUniqueGenes;
    m_statTissue = other.m_statTissue;
    m_statSpecie = other.m_statSpecie;
    m_statCreated = other.m_statCreated;
    m_statComments = other.m_statComments;
    return (*this);
}

bool Dataset::operator==(const Dataset& other) const
{
    return (
               m_id == other.m_id &&
               m_name == other.m_name &&
               m_chipId == other.m_chipId &&
               m_alignment == other.m_alignment &&
               m_figureBlue == other.m_figureBlue &&
               m_figureRed == other.m_figureRed &&
               m_figureStatus == other.m_figureStatus &&
               m_statBarcodes == other.m_statBarcodes &&
               m_statGenes == other.m_statGenes &&
               m_statUniqueBarcodes == other.m_statUniqueBarcodes &&
               m_statUniqueGenes == other.m_statUniqueGenes &&
               m_statTissue == other.m_statTissue &&
               m_statSpecie == other.m_statSpecie &&
               m_statCreated == other.m_statCreated &&
               m_statComments == other.m_statComments
           );
}

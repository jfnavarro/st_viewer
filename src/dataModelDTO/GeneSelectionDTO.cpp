/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DatasetStatisticsDTO.h"

DatasetStatisticsDTO::DatasetStatisticsDTO(QObject* parent) :
    QObject(parent),
    m_statistics()
{

}

DatasetStatisticsDTO::DatasetStatisticsDTO(const DatasetStatistics& statistics, QObject* parent) :
    QObject(parent),
    m_statistics(statistics)
{

}

DatasetStatisticsDTO::~DatasetStatisticsDTO()
{

}



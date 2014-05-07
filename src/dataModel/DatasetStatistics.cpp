/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <climits>

#include "DatasetStatistics.h"

DatasetStatistics::DatasetStatistics()
{

}

DatasetStatistics::DatasetStatistics(const DatasetStatistics& other)
{
    m_datasetId = other.m_datasetId;
    m_hits = other.m_hits;
    m_pooledHits = other.m_pooledHits;
    m_quartilesHits = other.m_quartilesHits;
    m_pooledQuartilesHits = other.m_pooledQuartilesHits;
    m_sum = other.m_sum;
}

DatasetStatistics::~DatasetStatistics()
{

}

DatasetStatistics& DatasetStatistics::operator=(const DatasetStatistics& other)
{
    m_datasetId = other.m_datasetId;
    m_hits = other.m_hits;
    m_pooledHits = other.m_pooledHits;
    m_quartilesHits = other.m_quartilesHits;
    m_pooledQuartilesHits = other.m_pooledQuartilesHits;
    return (*this);
}

bool DatasetStatistics::operator==(const DatasetStatistics& other) const
{
    return (m_datasetId == other.m_datasetId &&
            m_hits == other.m_hits &&
            m_pooledHits == other.m_pooledHits &&
            m_quartilesHits == other.m_quartilesHits &&
            m_pooledQuartilesHits == other.m_pooledQuartilesHits &&
            m_sum == other.m_sum
        );
}

qreal DatasetStatistics::minValue()
{
    Q_ASSERT(m_quartilesHits.size() == 5);
    // max ( q1 - 1.5 * (q3-q1), q0 )
    const qreal q0 = m_quartilesHits.at(0);
    const qreal q1 = m_quartilesHits.at(1);
    const qreal q3 = m_quartilesHits.at(3);
    return std::max(q1 - 1.5 * (q3 - q1), q0);
}

qreal DatasetStatistics::maxValue()
{
    Q_ASSERT(m_quartilesHits.size() == 5);
    // min ( q3 + 1.5 * (q3-q1), q4 )
    const qreal q4 = m_quartilesHits.at(4);
    const qreal q1 = m_quartilesHits.at(1);
    const qreal q3 = m_quartilesHits.at(3);
    return std::min(q3 + 1.5 * (q3 - q1), q4);
}

qreal DatasetStatistics::pooledMin()
{
    Q_ASSERT(m_pooledQuartilesHits.size() == 5);
    // max ( q1 - 1.5 * (q3-q1), q0 )
    const qreal q0 = m_pooledQuartilesHits.at(0);
    const qreal q1 = m_pooledQuartilesHits.at(1);
    const qreal q3 = m_pooledQuartilesHits.at(3);
    return std::max(q1 - 1.5 * (q3 - q1), q0);
}

qreal DatasetStatistics::pooledMax()
{
    Q_ASSERT(m_pooledQuartilesHits.size() == 5);
    // min ( q3 + 1.5 * (q3-q1), q4 )
    const qreal q4 = m_pooledQuartilesHits.at(4);
    const qreal q1 = m_pooledQuartilesHits.at(1);
    const qreal q3 = m_pooledQuartilesHits.at(3);
    return std::min(q3 + 1.5 * (q3 - q1), q4);
}

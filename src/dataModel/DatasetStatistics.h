/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETSTATISTICS_H
#define DATASETSTATISTICS_H

#include "utils/Utils.h"

// Data model class to store hit count data.
class DatasetStatistics
{

public:

    typedef QVector<int> hitsType;
    typedef QVector<qreal> quartilesType;

    DatasetStatistics();
    explicit DatasetStatistics(const DatasetStatistics& other);
    virtual ~DatasetStatistics();

    DatasetStatistics& operator= (const DatasetStatistics& other);
    bool operator== (const DatasetStatistics& other) const;

    const QString datasetId() { return m_datasetId; }
    const DatasetStatistics::hitsType hits() { return m_hits; }
    const DatasetStatistics::hitsType pooledHits() { return m_pooledHits; }
    const DatasetStatistics::quartilesType hitsQuartiles() { return m_quartilesHits; }
    const DatasetStatistics::quartilesType pooledQuartilesHits() { return m_pooledQuartilesHits; }

    int hitsSum() { return m_sum; }

    // The functions minValue() and maxValue() 
    // were previously called min() and max() but that led to compile problems
    // in Visual Studio. (error message: not enough actual parameters for macro 'min')
    
    qreal minValue();
    qreal maxValue();
    qreal pooledMin();
    qreal pooledMax();

    void datasetId(const QString& datasetId) { m_datasetId = datasetId; }
    void hits(const DatasetStatistics::hitsType& hits) { m_hits = hits; }
    void pooledHits(const DatasetStatistics::hitsType& pooledHits) { m_pooledHits = pooledHits; }
    void hitsQuartiles(const DatasetStatistics::quartilesType& quartilesHits) { m_quartilesHits = quartilesHits; }

    void pooledQuartilesHits(const DatasetStatistics::quartilesType& pooledQuartilesHits)
    { m_pooledQuartilesHits = pooledQuartilesHits; }

    void hitsSum(int sum) { m_sum = sum; }

private:

    QString m_datasetId;
    hitsType m_hits;
    hitsType m_pooledHits;
    quartilesType m_quartilesHits;
    quartilesType m_pooledQuartilesHits;
    int m_sum;

};

#endif // DATASETSTATISTICS_H //

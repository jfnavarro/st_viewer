/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETSTATISTICSDTO_H
#define DATASETSTATISTICSDTO_H

#include <QObject>

#include "dataModel/DatasetStatistics.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// HitCount defines the parsing object for the underlying HitCount data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class DatasetStatisticsDTO : public QObject
{

public:

    Q_OBJECT

    Q_PROPERTY(QString datasetId READ datasetId WRITE datasetId)
    Q_PROPERTY(QVariantList hits READ hits WRITE hits)
    Q_PROPERTY(QVariantList pooledHits READ pooledHits WRITE pooledHits)
    Q_PROPERTY(QVariantList hitsQuartiles READ hitsQuartiles WRITE hitsQuartiles)
    Q_PROPERTY(QVariantList pooledHitsQuartiles READ pooledQuartilesHits WRITE pooledQuartilesHits)
    Q_PROPERTY(int hitsSum READ hitsSum WRITE hitsSum)

public:

    explicit DatasetStatisticsDTO(QObject* parent = 0);
    explicit DatasetStatisticsDTO(const DatasetStatistics& statistics, QObject* parent = 0);
    virtual ~DatasetStatisticsDTO();

    // binding
    const QString datasetId() { return m_statistics.datasetId(); }
    const QVariantList hits() { return serializeVector(m_statistics.hits()); }
    const QVariantList pooledHits() { return serializeVector(m_statistics.pooledHits()); }
    const QVariantList hitsQuartiles() { return serializeVector(m_statistics.hitsQuartiles()); }
    const QVariantList pooledQuartilesHits() { return serializeVector(m_statistics.pooledQuartilesHits()); }
    int hitsSum() { return m_statistics.hitsSum(); }

    void datasetId(const QString& datasetId) { m_statistics.datasetId(datasetId); }
    void hits(const QVariantList& hits) { m_statistics.hits(unserializeVector<int>(hits)); }

    void pooledHits(const QVariantList& pooledHits)
    { m_statistics.pooledHits(unserializeVector<int>(pooledHits)); }

    void hitsQuartiles(const QVariantList& hitsQuartiles)
    { m_statistics.hitsQuartiles(unserializeVector<qreal>(hitsQuartiles)); }

    void pooledQuartilesHits(const QVariantList& pooledQuartilesHits)
    { m_statistics.pooledQuartilesHits(unserializeVector<qreal>(pooledQuartilesHits)); }

    void hitsSum(int hitsSum) { m_statistics.hitsSum(hitsSum); }

    // get parsed data model
    const DatasetStatistics& datasetStatistics() const { return m_statistics; }
    DatasetStatistics& datasetStatistics() { return m_statistics; }

private:

    template<typename N>
    const QVariantList serializeVector(const QVector<N>& unserializedVector) const
    {
        QVariantList newList;
        foreach( const N &item, unserializedVector.toList() ) {
            newList << item;
        }
        return newList;
    }

    template<typename N>
    const QVector<N> unserializeVector(const QVariantList& serializedVector) const
    {
        // unserialize data
        QList<N> values;
        QVariantList::const_iterator it, end = serializedVector.end();
        for (it = serializedVector.begin(); it != end; ++it) {
            values << it->value<N>();
        }
        return QVector<N>::fromList(values);
    }

    DatasetStatistics m_statistics;
};

#endif // DATASETSTATISTICSDTO_H //

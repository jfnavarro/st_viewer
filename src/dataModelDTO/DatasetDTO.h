/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETDTO_H
#define DATASETDTO_H

#include <QObject>
#include <QString>
#include <QVariantList>

#include "dataModel/Dataset.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

class DatasetDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id WRITE id)
    Q_PROPERTY(QString name WRITE name)
    Q_PROPERTY(QString image_alignment_id WRITE imageAlignmentId)
    Q_PROPERTY(QString tissue WRITE statTissue)
    Q_PROPERTY(QString species WRITE statSpecie)
    Q_PROPERTY(int overall_barcodes WRITE statBarcodes)
    Q_PROPERTY(int overall_genes WRITE statGenes)
    Q_PROPERTY(int overall_unique_barcodes WRITE statUniqueBarcodes)
    Q_PROPERTY(int overall_unique_genes WRITE statUniqueGenes)
    Q_PROPERTY(QVariantList overall_hit_quartiles WRITE hitsQuartiles)
    Q_PROPERTY(QString comment WRITE statComments)

public:

    explicit DatasetDTO(QObject* parent = 0) : QObject(parent) { }
    ~DatasetDTO() { }

    // binding
    void id(const QString& id) { m_dataset.id(id); }
    void name(const QString& name) { m_dataset.name(name); }
    void imageAlignmentId(const QString& alignmentId) { m_dataset.imageAlignmentId(alignmentId); }
    void statTissue(const QString& tissue) { m_dataset.statTissue(tissue); }
    void statSpecie(const QString& specie) { m_dataset.statSpecie(specie); }
    void statBarcodes(int barcodes) { m_dataset.statBarcodes(barcodes); }
    void statGenes(int genes) { m_dataset.statGenes(genes); }
    void statUniqueBarcodes(int unique_barcodes) { m_dataset.statUniqueBarcodes(unique_barcodes); }
    void statUniqueGenes(int unique_genes) { m_dataset.statUniqueGenes(unique_genes); }
    void hitsQuartiles(QVariantList hitQuartiles) { m_dataset.hitsQuartiles(unserializeVector<qreal>(hitQuartiles)); }
    void statComments(const QString& comments) { m_dataset.statComments(comments); }

    // get parsed data model
    const Dataset& dataset() const { return m_dataset; }
    Dataset& dataset() { return m_dataset; }

private:

    //TODO duplicated in other DTOs move to Utils class
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


    Dataset m_dataset;
};

#endif // DATASETDTO_H //

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

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString image_alignment_id READ imageAlignmentId WRITE imageAlignmentId)
    Q_PROPERTY(QString tissue READ statTissue WRITE statTissue)
    Q_PROPERTY(QString species READ statSpecie WRITE statSpecie)
    Q_PROPERTY(int overall_feature_count READ statBarcodes WRITE statBarcodes)
    Q_PROPERTY(int overall_hit_count READ statGenes WRITE statGenes)
    Q_PROPERTY(int unique_barcode_count READ statUniqueBarcodes WRITE statUniqueBarcodes)
    Q_PROPERTY(int unique_gene_count READ statUniqueGenes WRITE statUniqueGenes)
    Q_PROPERTY(QVariantList overall_hit_quartiles READ hitsQuartiles WRITE hitsQuartiles)
    Q_PROPERTY(QString comment READ statComments WRITE statComments)
    Q_PROPERTY(bool enabled READ enabled WRITE enabled)
    Q_PROPERTY(QVariantList obo_foundry_terms READ oboFoundryTerms WRITE oboFoundryTerms)

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
    void enabled(const bool enabled) { m_dataset.enabled(enabled); }
    void oboFoundryTerms(const QVariantList& oboFoundryTerms)
      { m_dataset.oboFoundryTerms(unserializeVector<QString>(oboFoundryTerms)); }

    // read
    const QString id() { return m_dataset.id(); }
    const QString name() { return m_dataset.name(); }
    const QString imageAlignmentId() { return m_dataset.imageAlignmentId(); }
    const QString statTissue() { return m_dataset.statTissue(); }
    const QString statSpecie() { return m_dataset.statSpecie(); }
    int statBarcodes() { return m_dataset.statBarcodes(); }
    int statGenes() { return m_dataset.statGenes(); }
    int statUniqueBarcodes() { return m_dataset.statUniqueBarcodes(); }
    int statUniqueGenes() { return m_dataset.statUniqueGenes(); }
    const QVariantList hitsQuartiles() { return serializeVector<qreal>(m_dataset.hitsQuartiles()); }
    const QString statComments() { return m_dataset.statComments(); }
    bool enabled() { return m_dataset.enabled(); }
    const QVariantList oboFoundryTerms()
      { return serializeVector<QString>(m_dataset.oboFoundryTerms()); }

    // get parsed data model
    const Dataset& dataset() const { return m_dataset; }
    Dataset& dataset() { return m_dataset; }

private:

    //TODO duplicated in other DTOs move to Utils class
    template<typename N>
    const QVariantList serializeVector(const QVector<N>& unserializedVector) const
    {
        QVariantList newList;
        foreach(const N &item, unserializedVector.toList()) {
            newList << QVariant::fromValue(item);
        }
        return newList;
    }

    template<typename N>
    const QVector<N> unserializeVector(const QVariantList& serializedVector) const
    {
        // unserialize data
        QVector<N> values;
        QVariantList::const_iterator it;
        QVariantList::const_iterator end = serializedVector.end();
        for (it = serializedVector.begin(); it != end; ++it) {
            values.push_back(it->value<N>());
        }
        return values;
    }


    Dataset m_dataset;
};

#endif // DATASETDTO_H //

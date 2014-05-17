/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENESELECTIONDTO_H
#define GENESELECTIONDTO_H

#include <QObject>
#include <QString>
#include "dataModel/GeneSelection.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

class GeneSelectionDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id WRITE id)
    Q_PROPERTY(QString name WRITE name)
    Q_PROPERTY(QString account_id WRITE userId)
    Q_PROPERTY(QString dataset_id WRITE datasetId)
    Q_PROPERTY(QVariantList feature_ids WRITE featureIds)
    Q_PROPERTY(QString type WRITE type)
    Q_PROPERTY(QString status WRITE status)
    Q_PROPERTY(QVariantList obo_foundry_terms WRITE oboFoundryTerms)
    Q_PROPERTY(QString comment WRITE comment)

public:

    explicit GeneSelectionDTO(QObject* parent = 0) : QObject(parent) {};
    ~GeneSelectionDTO() {};

    // binding
    void id(const QString& id) { m_geneSelection.id(id); }
    void name(const QString& name) { m_geneSelection.name(name); }
    void userId(const QString& userId) { m_geneSelection.userId(userId); }
    void datasetId(const QString& datasetId) { m_geneSelection.datasetId(datasetId); }
    void featureIds(const QVarianList& featureIds)
      { m_geneSelection.featureIds(unserializeVector<QString>(featuresIds)); }
    void type(const QString& type) { m_geneSelection.type(type); }
    void status(const QString& status) { m_geneSelection.status(status); }
    void oboFoundryTerms(const QVariantList& oboFoundryTerms)
      { m_geneSelection.oboFoundryTerms(unserializeVector<QString>(oboFoundryTerms)); }
    void comment(const QString& comment) { m_geneSelection.comment(comment); }

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

    GeneSelection m_geneSelection;
};

#endif // GENESELECTIONDTO_H //

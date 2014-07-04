/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENESELECTIONDTO_H
#define GENESELECTIONDTO_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include "dataModel/GeneSelection.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

Q_DECLARE_METATYPE(SelectionType)

class GeneSelectionDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString account_id READ userId WRITE userId)
    Q_PROPERTY(QString dataset_id READ datasetId WRITE datasetId)
    Q_PROPERTY(QVariantList gene_hits READ selectedItems WRITE selectedItems)
    Q_PROPERTY(QString type READ type WRITE type)
    Q_PROPERTY(QString status READ status WRITE status)
    Q_PROPERTY(QVariantList obo_foundry_terms READ oboFoundryTerms WRITE oboFoundryTerms)
    Q_PROPERTY(QString comment READ comment WRITE comment)
    Q_PROPERTY(QString resul_file READ resultFile WRITE resultFile)
    Q_PROPERTY(bool enabled READ enabled WRITE enabled)

public:

    explicit GeneSelectionDTO(QObject* parent = 0) : QObject(parent) {}
    GeneSelectionDTO(const GeneSelection& selection, QObject* parent = 0) :
        QObject(parent), m_geneSelection(selection) {}
    ~GeneSelectionDTO() {}

    //getters
    const QString id() { return m_geneSelection.id(); }
    const QString name() { return m_geneSelection.name(); }
    const QString userId() { return m_geneSelection.userId(); }
    const QString datasetId() { return m_geneSelection.datasetId(); }
    const QVariantList selectedItems()
      { return serializeVector<SelectionType>(m_geneSelection.selectedItems()); }
    const QString type() { return m_geneSelection.type(); }
    const QString status() { return m_geneSelection.status(); }
    const QVariantList oboFoundryTerms()
      { return serializeVector<QString>(m_geneSelection.oboFoundryTerms()); }
    const QString comment() { return m_geneSelection.comment(); }
    const QString resultFile() { return m_geneSelection.resultFile(); }
    void enabled(const bool enabled) { m_geneSelection.enabled(enabled); }

    // binding
    void id(const QString& id) { m_geneSelection.id(id); }
    void name(const QString& name) { m_geneSelection.name(name); }
    void userId(const QString& userId) { m_geneSelection.userId(userId); }
    void datasetId(const QString& datasetId) { m_geneSelection.datasetId(datasetId); }
    void selectedItems(const QVariantList& selectedItems)
      { m_geneSelection.selectedItems(unserializeVector<SelectionType>(selectedItems)); }
    void type(const QString& type) { m_geneSelection.type(type); }
    void status(const QString& status) { m_geneSelection.status(status); }
    void oboFoundryTerms(const QVariantList& oboFoundryTerms)
      { m_geneSelection.oboFoundryTerms(unserializeVector<QString>(oboFoundryTerms)); }
    void comment(const QString& comment) { m_geneSelection.comment(comment); }
    void resultFile(const QString& file) { m_geneSelection.resultFile(file); }
    bool enabled() { return m_geneSelection.enabled(); }

    // get parsed data model
    const GeneSelection& geneSelection() const { return m_geneSelection; }
    GeneSelection& geneSelection() { return m_geneSelection; }

private:

    //TODO duplicated in other DTOs move to Utils class
    template<typename N>
    const QVariantList serializeVector(const QVector<N> &unserializedVector) const
    {
        QVariantList newList;
        foreach(const N &item, unserializedVector.toList()) {
            newList << QVariant::fromValue(item);
        }
        return newList;
    }

    template<typename N>
    const QVector<N> unserializeVector(const QVariantList &serializedVector) const
    {
        // unserialize data
        QList<N> values;
        QVariantList::const_iterator it;
        QVariantList::const_iterator end = serializedVector.end();
        for (it = serializedVector.begin(); it != end; ++it) {
            values << it->value<N>();
        }
        return QVector<N>::fromList(values);
    }

    GeneSelection m_geneSelection;
};

#endif // GENESELECTIONDTO_H //

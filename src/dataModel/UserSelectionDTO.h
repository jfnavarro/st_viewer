/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef USERSELECTIONDTO_H
#define USERSELECTIONDTO_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

#include "dataModel/UserSelection.h"
#include "utils/SerializationFunctions.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

Q_DECLARE_METATYPE(AggregatedGene)

// TODO move definitions to CPP and/or consider removing DTOs
// TODO add support to send the features data (as binary file)
// and the spots data (as JSON part of the object)
class UserSelectionDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString account_id READ userId WRITE userId)
    Q_PROPERTY(QString dataset_id READ datasetId WRITE datasetId)
    Q_PROPERTY(QVariantList gene_hits READ selectedGenes WRITE selectedGenes)
    Q_PROPERTY(QString type READ type WRITE type)
    Q_PROPERTY(QString status READ status WRITE status)
    Q_PROPERTY(QVariantList obo_foundry_terms READ oboFoundryTerms WRITE oboFoundryTerms)
    Q_PROPERTY(QString comment READ comment WRITE comment)
    Q_PROPERTY(bool enabled READ enabled WRITE enabled)
    Q_PROPERTY(QString created_at READ created WRITE created)
    Q_PROPERTY(QString last_modified READ lastModified WRITE lastModified)
    Q_PROPERTY(QByteArray tissue_snapshot READ tissueSnapShot WRITE tissueSnapShot)

public:
    explicit UserSelectionDTO(QObject* parent = 0)
        : QObject(parent)
    {
    }
    UserSelectionDTO(const UserSelection& selection, QObject* parent = 0)
        : QObject(parent)
        , m_userSelection(selection)
    {
    }
    ~UserSelectionDTO() {}

    // getters
    const QString id() const { return m_userSelection.id(); }
    const QString name() const { return m_userSelection.name(); }
    const QString userId() const { return m_userSelection.userId(); }
    const QString datasetId() const { return m_userSelection.datasetId(); }
    const QVariantList selectedGenes() const
    {
        return serializeSelectionVector(m_userSelection.selectedGenes());
    }
    const QString type() const { return UserSelection::typeToQString(m_userSelection.type()); }
    const QString status() const { return m_userSelection.status(); }
    const QVariantList oboFoundryTerms() const
    {
        return serializeVector<QString>(m_userSelection.oboFoundryTerms());
    }
    const QString comment() const { return m_userSelection.comment(); }
    bool enabled() const { return m_userSelection.enabled(); }
    const QString created() const { return m_userSelection.created(); }
    const QString lastModified() const { return m_userSelection.lastModified(); }
    const QByteArray tissueSnapShot() const { return m_userSelection.tissueSnapShot(); }

    // binding
    void id(const QString& id) { m_userSelection.id(id); }
    void name(const QString& name) { m_userSelection.name(name); }
    void userId(const QString& userId) { m_userSelection.userId(userId); }
    void datasetId(const QString& datasetId) { m_userSelection.datasetId(datasetId); }
    void selectedGenes(const QVariantList& selectedGenes)
    {
        m_userSelection.selectedGenes(unserializeSelectionVector(selectedGenes));
    }
    void type(const QString& type) { m_userSelection.type(UserSelection::QStringToType(type)); }
    void status(const QString& status) { m_userSelection.status(status); }
    void oboFoundryTerms(const QVariantList& oboFoundryTerms)
    {
        m_userSelection.oboFoundryTerms(unserializeVector<QString>(oboFoundryTerms));
    }
    void comment(const QString& comment) { m_userSelection.comment(comment); }
    void enabled(const bool enabled) { m_userSelection.enabled(enabled); }
    void created(const QString& created) { m_userSelection.created(created); }
    void lastModified(const QString& lastModified) { m_userSelection.lastModified(lastModified); }
    void tissueSnapShot(const QByteArray& tissueSnapshot)
    {
        m_userSelection.tissueSnapShot(tissueSnapshot);
    }
    // get parsed data model
    const UserSelection& geneSelection() const { return m_userSelection; }
    UserSelection& geneSelection() { return m_userSelection; }

    // toJson is needed to send PUT/POST requests as the JSON content of the object
    // is appended to the request
    // TODO transform this to obtain fields and their types dynamically using the meta_properties
    QByteArray toJson() const
    {
        QJsonObject jsonObj;
        jsonObj["id"] = !id().isNull() ? QJsonValue(id()) : QJsonValue::Null;
        jsonObj["name"] = name();
        jsonObj["account_id"] = userId();
        jsonObj["dataset_id"] = datasetId();
        QJsonArray geneHits;
        foreach (const AggregatedGene& item, m_userSelection.selectedGenes()) {
            QJsonArray geneHit;
            geneHit.append(item.name);
            // TODO temp hack coz they are wronly defined as strings in the server
            geneHit.append(QString::number(item.reads));
            geneHit.append(QString::number(item.count));
            geneHit.append(QString::number(item.normalizedReads));
            geneHits.append(geneHit);
        }
        jsonObj["gene_hits"] = geneHits;
        jsonObj["type"] = !type().isNull() ? QJsonValue(type()) : QJsonValue::Null;
        jsonObj["status"] = !status().isNull() ? QJsonValue(status()) : QJsonValue::Null;
        QJsonArray oboTerms;
        foreach (const QString& item, m_userSelection.oboFoundryTerms()) {
            oboTerms.append(item);
        }
        jsonObj["obo_foundry_terms"] = oboTerms;
        jsonObj["comment"] = !comment().isNull() ? QJsonValue(comment()) : QJsonValue::Null;
        jsonObj["enabled"] = enabled();
        jsonObj["created_at"] = QJsonValue::Null; // leave this empty the API will take care of it
        jsonObj["last_modified"] = QJsonValue::Null; // leave this empty the API will take care of
                                                     // it
        QByteArray tissue_base64 = tissueSnapShot().toBase64();
        jsonObj["tissue_snapshot"] = QString::fromUtf8(tissue_base64);

        QJsonDocument doc(jsonObj);
        QByteArray serializedDoc = doc.toJson(QJsonDocument::Compact);
        return serializedDoc;
    }

private:
    // Transforms a list of AggregatedGenes to a QVariantList
    // TODO this could be done automatically in serializeVector() we just need to register
    // the selection metatype conversion
    const QVariantList serializeSelectionVector(
        const UserSelection::selectedGenesList& unserializedVector) const
    {
        QVariantList newList;
        foreach (const AggregatedGene& item, unserializedVector) {
            QVariantList itemList;
            itemList << item.name << QString::number(item.reads) << QString::number(item.count)
                     << QString::number(item.normalizedReads);
            newList << QVariant::fromValue(itemList);
        }

        return newList;
    }

    // Transforms a QVariantList of AggregatedGenes to a list of AggregatedGenes
    // TODO this could be done automatically in unserializeVector() we just need to register
    // the selection metatype conversion
    const UserSelection::selectedGenesList unserializeSelectionVector(
        const QVariantList& serializedVector) const
    {
        // unserialize data
        UserSelection::selectedGenesList values;
        QVariantList::const_iterator it;
        QVariantList::const_iterator end = serializedVector.end();
        for (it = serializedVector.begin(); it != end; ++it) {
            QVariantList elementList = it->toList();
            // TODO there seems to be some buggy selection items in the DB with 5 elements
            Q_ASSERT(elementList.size() >= 4);
            const QString name = elementList.at(0).toString();
            const int reads = elementList.at(1).toInt();
            const int count = elementList.at(2).toInt();
            const int normalizedReads = elementList.at(3).toInt();
            AggregatedGene genes_selection(name, reads, normalizedReads, count);
            values.push_back(genes_selection);
        }

        return values;
    }

    UserSelection m_userSelection;
};

#endif // USERSELECTIONDTO_H //

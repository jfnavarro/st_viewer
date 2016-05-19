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

// TODO move definitions to CPP and/or consider removing DTOs
// TODO add support to send the features data (as binary file)
class UserSelectionDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString account_id READ userId WRITE userId)
    Q_PROPERTY(QString dataset_id READ datasetId WRITE datasetId)
    Q_PROPERTY(QVariantList gene_hits READ selectedFeatures WRITE selectedFeatures)
    Q_PROPERTY(QString type READ type WRITE type)
    Q_PROPERTY(QString status READ status WRITE status)
    Q_PROPERTY(QString comment READ comment WRITE comment)
    Q_PROPERTY(bool enabled READ enabled WRITE enabled)
    Q_PROPERTY(QString created_at READ created WRITE created)
    Q_PROPERTY(QString last_modified READ lastModified WRITE lastModified)
    Q_PROPERTY(QByteArray tissue_snapshot READ tissueSnapShot WRITE tissueSnapShot)

public:
    explicit UserSelectionDTO(QObject *parent = 0)
        : QObject(parent)
    {
    }
    UserSelectionDTO(const UserSelection &selection, QObject *parent = 0)
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
    const QVariantList selectedFeatures() const
    {
        return serializeSelectionVector(m_userSelection.selectedFeatures());
    }
    const QString type() const { return UserSelection::typeToQString(m_userSelection.type()); }
    const QString status() const { return m_userSelection.status(); }
    const QString comment() const { return m_userSelection.comment(); }
    bool enabled() const { return m_userSelection.enabled(); }
    const QString created() const { return m_userSelection.created(); }
    const QString lastModified() const { return m_userSelection.lastModified(); }
    const QByteArray tissueSnapShot() const { return m_userSelection.tissueSnapShot(); }

    // binding
    void id(const QString &id) { m_userSelection.id(id); }
    void name(const QString &name) { m_userSelection.name(name); }
    void userId(const QString &userId) { m_userSelection.userId(userId); }
    void datasetId(const QString &datasetId) { m_userSelection.datasetId(datasetId); }
    void selectedFeatures(const QVariantList &selectedFeatures)
    {
        m_userSelection.loadFeatures(unserializeSelectionVector(selectedFeatures));
    }
    void type(const QString &type) { m_userSelection.type(UserSelection::QStringToType(type)); }
    void status(const QString &status) { m_userSelection.status(status); }
    void comment(const QString &comment) { m_userSelection.comment(comment); }
    void enabled(const bool enabled) { m_userSelection.enabled(enabled); }
    void created(const QString &created) { m_userSelection.created(created); }
    void lastModified(const QString &lastModified) { m_userSelection.lastModified(lastModified); }
    void tissueSnapShot(const QByteArray &tissueSnapshot)
    {
        m_userSelection.tissueSnapShot(tissueSnapshot);
    }

    // get parsed data model
    const UserSelection &userSelection() const { return m_userSelection; }
    UserSelection &userSelection() { return m_userSelection; }

    // toJson is needed to send PUT/POST requests as the JSON content of the
    // object
    // is appended to the request
    // TODO transform this to obtain fields and their types dynamically using the
    // meta_properties
    QByteArray toJson() const
    {
        QJsonObject jsonObj;
        jsonObj["id"] = !id().isNull() ? QJsonValue(id()) : QJsonValue::Null;
        jsonObj["name"] = name();
        jsonObj["account_id"] = userId();
        jsonObj["dataset_id"] = datasetId();
        QJsonArray geneHits;
        for (const auto &feature : m_userSelection.selectedFeatures()) {
            QJsonArray geneHit;
            geneHit.append(feature->gene());
            // TODO temp hack coz they are wronly defined as strings in the server
            geneHit.append(QString::number(feature->x()));
            geneHit.append(QString::number(feature->y()));
            geneHit.append(QString::number(feature->count()));
            geneHits.append(geneHit);
        }
        jsonObj["gene_hits"] = geneHits;
        jsonObj["type"] = !type().isNull() ? QJsonValue(type()) : QJsonValue::Null;
        jsonObj["status"] = !status().isNull() ? QJsonValue(status()) : QJsonValue::Null;
        jsonObj["comment"] = !comment().isNull() ? QJsonValue(comment()) : QJsonValue::Null;
        jsonObj["enabled"] = enabled();
        jsonObj["created_at"] = QJsonValue::Null;    // leave this empty the API will update it
        jsonObj["last_modified"] = QJsonValue::Null; // leave this empty the API will update it
        jsonObj["tissue_snapshot"] = QString::fromUtf8(tissueSnapShot());

        QJsonDocument doc(jsonObj);
        QByteArray serializedDoc = doc.toJson(QJsonDocument::Compact);
        return serializedDoc;
    }

private:
    // Transforms a list of selected features to a QVariantList
    // TODO this could be done automatically in serializeVector() we just need to
    // register
    // the selection metatype conversion in the Feature Object
    const QVariantList serializeSelectionVector(
        const DataProxy::FeatureList &unserializedVector) const
    {
        QVariantList newList;
        for (const auto &feature : unserializedVector) {
            QVariantList itemList;
            itemList << feature->gene() << QString::number(feature->x())
                     << QString::number(feature->y()) << QString::number(feature->count());
            newList << QVariant::fromValue(itemList);
        }

        return newList;
    }

    // Transforms a QVariantList of features to a list of features
    // TODO this could be done automatically in serializeVector() we just need to
    // register
    // the selection metatype conversion in the Feature Object
    const DataProxy::FeatureList unserializeSelectionVector(
        const QVariantList &serializedVector) const
    {
        // unserialize data
        DataProxy::FeatureList features;
        QVariantList::const_iterator it;
        QVariantList::const_iterator end = serializedVector.end();
        for (it = serializedVector.begin(); it != end; ++it) {
            QVariantList elementList = it->toList();
            // TODO there seems to be some buggy selection items in the DB with 5
            // elements
            Q_ASSERT(elementList.size() >= 4);
            const QString gene = elementList.at(0).toString();
            const int x = elementList.at(1).toInt();
            const int y = elementList.at(2).toInt();
            const int count = elementList.at(3).toInt();
            features.push_back(std::make_shared<Feature>(Feature(gene, x, y, count)));
        }
        return features;
    }

    UserSelection m_userSelection;
};

#endif // USERSELECTIONDTO_H //

#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QString>
#include <QVector>
#include <QSet>
#include <QColor>

#include "dataModel/Feature.h"
#include "data/DataProxy.h"

// Gene selection represents a selection of spots made by the user trough the
// UI.
// Users selects barcodes/spots by manual selection of targe gene names.
// Selections can be stored in the database
class UserSelection
{
    Q_ENUMS(Type)
    Q_FLAGS(Types)

public:
    typedef QPair<QString, int> geneCount;
    typedef QVector<geneCount> geneTotalCountsVector;

    enum Type { Rubberband = 1, Lazo = 2, Segmented = 3, Console = 4, Cluster = 5, Other = 6 };
    Q_DECLARE_FLAGS(Types, Type)

    UserSelection();
    explicit UserSelection(const UserSelection &other);
    ~UserSelection();

    UserSelection &operator=(const UserSelection &other);
    bool operator==(const UserSelection &other) const;

    // Id represents databaset ID for objects
    const QString id() const;
    const QString name() const;
    // Id represents databaset ID for objects
    const QString userId() const;
    // Id represents databaset ID for objects
    const QString datasetId() const;
    // the list of features present in the selection
    const DataProxy::FeatureList selectedFeatures() const;
    const QString status() const;
    const QString comment() const;
    // whether the selection is valid or not
    bool enabled() const;
    const QString created() const;
    const QString lastModified() const;
    const QString datasetName() const;
    // the type of the selection
    Type type() const;
    // a image snapshot of the tissue taken when the selection was made
    const QByteArray tissueSnapShot() const;
    // returns true when the selection has been fetched from the DB
    bool saved() const;
    // returns the total sum of reads in the selection
    int totalReads() const;
    // returns the total number of features (gene-spot) in the selection
    int totalFeatures() const;
    // returns the total number of genes in the selection
    int totalGenes() const;
    // returns the total number of spots in the selection
    int totalSpots() const;

    void id(const QString &id);
    void name(const QString &name);
    void userId(const QString &userId);
    void datasetId(const QString &datasetId);
    void selectedFeatures(const DataProxy::FeatureList &features);
    void status(const QString &status);
    void comment(const QString &comment);
    void enabled(const bool enabled);
    void created(const QString &created);
    void lastModified(const QString &lastModified);
    void datasetName(const QString &datasetName);
    void type(const Type &type);
    void tissueSnapShot(const QByteArray &tissueSnapShot);
    void saved(const bool saved);
    // load features is meant to be used when creating the selection
    // this method will call selectedFeatures(features) to assign the features
    // but will also compute the selectedGenes and selectedSpots
    // so they can be stored in the selection
    void loadFeatures(const DataProxy::FeatureList &features);

    // convenience type to convert enum types to qstring and viceversea
    static QString typeToQString(const Type &type);
    static Type QStringToType(const QString &type);

    // helper functions to get a map of
    //    gene names -> total count in selection
    //    spot -> total count in selection
    geneTotalCountsVector getGeneCounts() const;
    Feature::spotTotalCounts getTotalCounts() const;

private:
    QString m_id;
    QString m_name;
    QString m_userId;
    QString m_datasetId;
    DataProxy::FeatureList m_selectedFeatures;
    Type m_type;
    QString m_status;
    QString m_comment;
    bool m_enabled;
    QString m_created;
    QString m_lastMofidied;
    QString m_datasetName;
    QByteArray m_tissueSnapShot;
    // for caching purposes to not have to recompute them
    int m_totalReads;
    int m_totalFeatures;
    int m_totalGenes;
    int m_totalSpots;
    // to differiante local selections from downloaded ones
    bool m_saved;
};

#endif // USERSELECTION_H

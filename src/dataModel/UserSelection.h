#ifndef USERSELECTION_H
#define USERSELECTION_H

#include <QString>
#include <QVector>
#include <QSet>
#include <QColor>
#include "data/DataProxy.h"

// AggregatedGene represents a gene in the selection
// a user selection is a set of features and one gene can
// be present in several features. AggregatedGene represents
// a gene with the aggregated count over all features
// TODO move to a separate file
class AggregatedGene
{

public:
    AggregatedGene();
    AggregatedGene(const AggregatedGene& other);
    AggregatedGene(QString name, unsigned reads, unsigned normalizedReads = 0, unsigned count = 0);

    AggregatedGene& operator=(const AggregatedGene& other);
    bool operator<(const AggregatedGene& other) const;
    bool operator==(const AggregatedGene& other) const;

    // the name of the gene
    QString name;
    // aggregated reads
    unsigned reads;
    unsigned normalizedReads;
    // number of features this gene is present in the selection
    unsigned count;
};

// Gene selection represents a selection of genes
// made by the user trough the UI, user selects barcodes/spots and then the information
// is aggregated by genes.
// Gene selections are meant to be stored and shown
// in the experiments view
// TODO right now we cache (total_reads, total_genes, total_spots and total_spots) for
// convenience. They get update when the UserSelection is created but we might want to
// store them in the database.
// TODO In reality we do not really need to store the aggregated genes and unique features
// they can be obtained when creating the selection or downloading it from the features

class UserSelection
{
    Q_ENUMS(Type)
    Q_FLAGS(Types)

public:
    typedef QList<AggregatedGene> selectedGenesList;
    typedef QSet<QString> selectedSpotsList;
    typedef DataProxy::FeatureList selectedFeaturesList;

    enum Type {
        Rubberband = 1,
        Lazo = 2,
        Segmented = 3,
        Console = 4,
        Cluster = 5,
        Other = 6
    };
    Q_DECLARE_FLAGS(Types, Type)

    UserSelection();
    explicit UserSelection(const UserSelection& other);
    ~UserSelection();

    UserSelection& operator=(const UserSelection& other);
    bool operator==(const UserSelection& other) const;

    // Id represents databaset ID for objects
    const QString id() const;
    const QString name() const;
    const QString userId() const;
    const QString datasetId() const;
    const selectedGenesList selectedGenes() const;
    const selectedFeaturesList selectedFeatures() const;
    const selectedSpotsList selectedSpots() const;
    const QString status() const;
    const QVector<QString> oboFoundryTerms() const;
    const QString comment() const;
    bool enabled() const;
    const QString created() const;
    const QString lastModified() const;
    const QString datasetName() const;
    Type type() const;
    const QByteArray tissueSnapShot() const;
    // returns true when the selection has been fetched from the DB
    bool saved() const;
    // returns the total sum of reads in the Selection items
    unsigned totalReads() const;
    // returns the total number of features (gene-spot) in this selection
    unsigned totalFeatures() const;
    // returns the total number of genes in this selection
    unsigned totalGenes() const;
    // returns the total number of spots in this selection
    unsigned totalSpots() const;

    void id(const QString& id);
    void name(const QString& name);
    void userId(const QString& userId);
    void datasetId(const QString& datasetId);
    void selectedGenes(const selectedGenesList& selectedItems);
    void selectedFeatures(const selectedFeaturesList& features);
    void selectedSpots(const selectedSpotsList& spots);
    void status(const QString& status);
    void oboFoundryTerms(const QVector<QString>& oboFoundryTerms);
    void comment(const QString& comment);
    void enabled(const bool enabled);
    void created(const QString& created);
    void lastModified(const QString& lastModified);
    void datasetName(const QString& datasetName);
    void type(const Type& type);
    void tissueSnapShot(const QByteArray& tissueSnapShot);
    void saved(const bool saved);
    // load features is meant to be used when creating the selection
    // this method will call selectedFeatures(features) to assign the features
    // but will also compute the selectedGenes and selectedSpots
    // so they can be stored in the selection
    void loadFeatures(const selectedFeaturesList& features);

    // convenience type to convert enum types to qstring and viceversea
    static QString typeToQString(const Type& type);
    static Type QStringToType(const QString& type);

private:
    QString m_id;
    QString m_name;
    QString m_userId;
    QString m_datasetId;
    selectedGenesList m_selectedGenes;
    selectedFeaturesList m_selectedFeatures;
    selectedSpotsList m_selectedSpots;
    Type m_type;
    QString m_status;
    QVector<QString> m_oboFroundryTerms;
    QString m_comment;
    bool m_enabled;
    QString m_created;
    QString m_lastMofidied;
    QString m_datasetName;
    QByteArray m_tissueSnapShot;
    // for caching purposes
    unsigned m_totalReads;
    unsigned m_totalFeatures;
    unsigned m_totalGenes;
    unsigned m_totalSpots;
    // to differiante local selections from downloaded ones
    bool m_saved;
};

#endif // USERSELECTION_H

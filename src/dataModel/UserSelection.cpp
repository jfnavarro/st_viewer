#include "UserSelection.h"

#include <QMap>
#include <QDate>
#include <QDebug>
#include "math/Common.h"

#include <numeric>
#include <unordered_set>

UserSelection::UserSelection()
    : m_name()
    , m_dataset()
    , m_selectedSpots()
    , m_selectedGenes()
    , m_type()
    , m_status()
    , m_comment()
    , m_tissueSnapShot()
    , m_totalReads(0)
{
}

UserSelection::UserSelection(const UserSelection &other)
    : m_name(other.m_name)
    , m_dataset(other.m_datasetId)
    , m_selectedSpots(other.m_selectedSpots)
    , m_selectedGenes(other.m_selectedGenes)
    , m_type(other.m_type)
    , m_status(other.m_status)
    , m_comment(other.m_comment)
    , m_enabled(other.m_enabled)
    , m_created(other.m_created)
    , m_lastMofidied(other.m_lastMofidied)
    , m_datasetName(other.m_datasetName)
    , m_tissueSnapShot(other.m_tissueSnapShot)
    , m_totalReads(other.m_totalReads)
    , m_totalFeatures(other.m_totalFeatures)
    , m_totalGenes(other.m_totalGenes)
    , m_totalSpots(other.m_totalSpots)
    , m_saved(other.m_saved)
{
}

UserSelection::~UserSelection()
{
}

UserSelection &UserSelection::operator=(const UserSelection &other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_userId = other.m_userId;
    m_datasetId = other.m_datasetId;
    m_selectedFeatures = other.m_selectedFeatures;
    m_type = other.m_type;
    m_status = other.m_status;
    m_comment = other.m_comment;
    m_enabled = other.m_enabled;
    m_created = other.m_created;
    m_lastMofidied = other.m_lastMofidied;
    m_datasetName = other.m_datasetName;
    m_totalReads = other.m_totalReads;
    m_totalFeatures = other.m_totalFeatures;
    m_totalGenes = other.m_totalGenes;
    m_totalSpots = other.m_totalSpots;
    m_tissueSnapShot = other.m_tissueSnapShot;
    m_saved = other.m_saved;
    return (*this);
}

bool UserSelection::operator==(const UserSelection &other) const
{
    return (m_id == other.m_id && m_name == other.m_name && m_userId == other.m_userId
            && m_datasetId == other.m_datasetId && m_selectedFeatures == other.m_selectedFeatures
            && m_type == other.m_type && m_status == other.m_status && m_comment == other.m_comment
            && m_enabled == other.m_enabled && m_created == other.m_created
            && m_lastMofidied == other.m_lastMofidied && m_datasetName == other.m_datasetName
            && m_tissueSnapShot == other.m_tissueSnapShot && m_totalReads == other.m_totalReads
            && m_totalFeatures == other.m_totalFeatures && m_totalGenes == other.m_totalGenes
            && m_totalSpots == other.m_totalSpots && m_saved == other.m_saved);
}

const QString UserSelection::id() const
{
    return m_id;
}

const QString UserSelection::name() const
{
    return m_name;
}

const QString UserSelection::userId() const
{
    return m_userId;
}

const QString UserSelection::datasetId() const
{
    return m_datasetId;
}

const QList<Spot> UserSelection::selectedFeatures() const
{
    return m_selectedFeatures;
}

const QString UserSelection::status() const
{
    return m_status;
}

const QString UserSelection::comment() const
{
    return m_comment;
}

bool UserSelection::enabled() const
{
    return m_enabled;
}

const QString UserSelection::created() const
{
    return m_created;
}

const QString UserSelection::lastModified() const
{
    return m_lastMofidied;
}

const QString UserSelection::datasetName() const
{
    return m_datasetName;
}

UserSelection::Type UserSelection::type() const
{
    return m_type;
}

const QByteArray UserSelection::tissueSnapShot() const
{
    return m_tissueSnapShot;
}

bool UserSelection::saved() const
{
    return m_saved;
}

int UserSelection::totalReads() const
{
    return m_totalReads;
}

int UserSelection::totalFeatures() const
{
    return m_totalFeatures;
}

int UserSelection::totalGenes() const
{
    return m_totalGenes;
}

int UserSelection::totalSpots() const
{
    return m_totalSpots;
}

void UserSelection::id(const QString &id)
{
    m_id = id;
}

void UserSelection::name(const QString &name)
{
    m_name = name;
}

void UserSelection::userId(const QString &userId)
{
    m_userId = userId;
}

void UserSelection::datasetId(const QString &datasetId)
{
    m_datasetId = datasetId;
}

void UserSelection::status(const QString &status)
{
    m_status = status;
}

void UserSelection::comment(const QString &comment)
{
    m_comment = comment;
}

void UserSelection::enabled(const bool enabled)
{
    m_enabled = enabled;
}

void UserSelection::created(const QString &created)
{
    m_created = created;
}

void UserSelection::lastModified(const QString &lastModified)
{
    m_lastMofidied = lastModified;
}

void UserSelection::datasetName(const QString &datasetName)
{
    m_datasetName = datasetName;
}

void UserSelection::type(const Type &type)
{
    m_type = type;
}

void UserSelection::tissueSnapShot(const QByteArray &tissueSnapShot)
{
    m_tissueSnapShot = tissueSnapShot;
}

void UserSelection::saved(const bool saved)
{
    m_saved = saved;
}

void UserSelection::loadFeatures(const DataProxy::FeatureList &features)
{
    m_selectedFeatures = features;
    // clear variables
    m_totalReads = 0;
    m_totalGenes = 0;
    m_totalFeatures = 0;
    m_totalSpots = 0;

    // populate the selected genes and spots by iterating the features
    QSet<QString> unique_genes;
    Feature::UniqueSpotsType unique_spots;
    for (const auto &feature : features) {
        unique_genes.insert(feature->gene());
        unique_spots.insert(feature->spot());
        m_totalReads += feature->count();
    }

    m_totalSpots = unique_spots.size();
    m_totalFeatures = m_selectedFeatures.size();
    m_totalGenes = unique_genes.size();
}

QString UserSelection::typeToQString(const UserSelection::Type &type)
{
    switch (type) {
    case Rubberband:
        return QString("Rubberband");
    case Lazo:
        return QString("Lazo");
    case Segmented:
        return QString("Segmented");
    case Console:
        return QString("Console");
    case Cluster:
        return QString("Cluster");
    case Other:
        return QString("Other");
    default:
        Q_ASSERT_X(true, "UserSelection", "Invalid selection type!");
    }
    Q_ASSERT(false); // Should never arrive here
    return QString();
}

UserSelection::Type UserSelection::QStringToType(const QString &type)
{
    const QString lower_type = type.toLower();
    if (lower_type == "rubberband") {
        return Rubberband;
    } else if (lower_type == "lazo") {
        return Lazo;
    } else if (lower_type == "segmented") {
        return Segmented;
    } else if (lower_type == "console") {
        return Console;
    } else if (lower_type == "cluster") {
        return Cluster;
    } else if (lower_type == "other") {
        return Other;
    }
    // Should never arrive here
    Q_ASSERT_X(true, "UserSelection", "Invalid selection type!");
    return Other;
}

UserSelection::geneTotalCountsVector UserSelection::getGeneCounts() const
{
    Feature::geneTotalCounts gene_countsMap;
    geneTotalCountsVector gene_countsVector;
    // aggreate counts by gene
    for (const auto &feature : m_selectedFeatures) {
        Q_ASSERT(feature);
        gene_countsMap[feature->gene()] += feature->count();
    }
    // create a vector of gene-count pairs
    Feature::geneTotalCounts::const_iterator it = gene_countsMap.constBegin();
    while (it != gene_countsMap.constEnd()) {
        gene_countsVector.push_back(geneCount(it.key(), it.value()));
        ++it;
    }
    // return the vector
    return gene_countsVector;
}

Feature::spotTotalCounts UserSelection::getTotalCounts() const
{
    Feature::spotTotalCounts read_counts;
    for (const auto &feature : m_selectedFeatures) {
        read_counts[feature->spot()] += feature->count();
    }
    return read_counts;
}

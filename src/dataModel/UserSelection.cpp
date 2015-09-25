/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "UserSelection.h"

#include <QMap>
#include <QDate>
#include "dataModel/Feature.h"
#include "math/Common.h"
#include <numeric>

AggregatedGene::AggregatedGene()
    : name()
    , reads(0)
    , normalizedReads(0)
    , count(0)
{
}

AggregatedGene::AggregatedGene(const AggregatedGene& other)
    : name(other.name)
    , reads(other.reads)
    , normalizedReads(other.normalizedReads)
    , count(other.count)
{
}

AggregatedGene::AggregatedGene(QString name,
                               unsigned reads,
                               unsigned normalizedReads,
                               unsigned count)
    : name(name)
    , reads(reads)
    , normalizedReads(normalizedReads)
    , count(count)
{
}

AggregatedGene& AggregatedGene::operator=(const AggregatedGene& other)
{
    name = other.name;
    reads = other.reads;
    normalizedReads = other.normalizedReads;
    count = other.count;
    return (*this);
}

// To enable sorting by name
bool AggregatedGene::operator<(const AggregatedGene& other) const
{
    return name < other.name;
}

bool AggregatedGene::operator==(const AggregatedGene& other) const
{
    return (name == other.name && reads == other.reads && normalizedReads == other.normalizedReads
            && count == other.count);
}

UserSelection::UserSelection()
    : m_id()
    , m_name()
    , m_userId()
    , m_datasetId()
    , m_selectedGenes()
    , m_selectedFeatures()
    , m_selectedSpots()
    , m_type()
    , m_status()
    , m_oboFroundryTerms()
    , m_comment()
    , m_enabled(false)
    , m_created(QDate::currentDate().toString())
    , m_lastMofidied(QDate::currentDate().toString())
    , m_datasetName()
    , m_tissueSnapShot()
    , m_color(Qt::red)
    , m_totalReads(0)
    , m_totalFeatures(0)
    , m_totalGenes(0)
    , m_totalSpots(0)
    , m_saved(false)
{
}

UserSelection::UserSelection(const UserSelection& other)
    : m_id(other.m_id)
    , m_name(other.m_name)
    , m_userId(other.m_userId)
    , m_datasetId(other.m_datasetId)
    , m_selectedGenes(other.m_selectedGenes)
    , m_selectedFeatures(other.m_selectedFeatures)
    , m_selectedSpots(other.m_selectedSpots)
    , m_type(other.m_type)
    , m_status(other.m_status)
    , m_oboFroundryTerms(other.m_oboFroundryTerms)
    , m_comment(other.m_comment)
    , m_enabled(other.m_enabled)
    , m_created(other.m_created)
    , m_lastMofidied(other.m_lastMofidied)
    , m_datasetName(other.m_datasetName)
    , m_tissueSnapShot(other.m_tissueSnapShot)
    , m_color(other.m_color)
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

UserSelection& UserSelection::operator=(const UserSelection& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_userId = other.m_userId;
    m_datasetId = other.m_datasetId;
    m_selectedGenes = other.m_selectedGenes;
    m_selectedFeatures = other.m_selectedFeatures;
    m_selectedSpots = other.m_selectedSpots;
    m_type = other.m_type;
    m_status = other.m_status;
    m_oboFroundryTerms = other.m_oboFroundryTerms;
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

bool UserSelection::operator==(const UserSelection& other) const
{
    return (m_id == other.m_id && m_name == other.m_name && m_userId == other.m_userId
            && m_datasetId == other.m_datasetId
            && m_selectedGenes == other.m_selectedGenes
            && m_selectedFeatures == other.m_selectedFeatures
            && m_selectedSpots == other.m_selectedSpots
            && m_type == other.m_type
            && m_status == other.m_status
            && m_oboFroundryTerms == other.m_oboFroundryTerms
            && m_comment == other.m_comment
            && m_enabled == other.m_enabled
            && m_created == other.m_created
            && m_lastMofidied == other.m_lastMofidied
            && m_datasetName == other.m_datasetName
            && m_tissueSnapShot == other.m_tissueSnapShot
            && m_color == other.m_color
            && m_totalReads == other.m_totalReads
            && m_totalFeatures == other.m_totalFeatures
            && m_totalGenes == other.m_totalGenes
            && m_totalSpots == other.m_totalSpots
            && m_saved == other.m_saved);
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

const UserSelection::selectedGenesList UserSelection::selectedGenes() const
{
    return m_selectedGenes;
}

const UserSelection::selectedFeaturesList UserSelection::selectedFeatures() const
{
    return m_selectedFeatures;
}

const UserSelection::selectedSpotsList UserSelection::selectedSpots() const
{
    return m_selectedSpots;
}

const QString UserSelection::status() const
{
    return m_status;
}

const QVector<QString> UserSelection::oboFoundryTerms() const
{
    return m_oboFroundryTerms;
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

const QColor UserSelection::color() const
{
    return m_color;
}

bool UserSelection::saved() const
{
    return m_saved;
}

unsigned UserSelection::totalReads() const
{
    Q_ASSERT(m_totalReads > 0);
    return m_totalReads;
}

unsigned UserSelection::totalFeatures() const
{
    Q_ASSERT(m_totalFeatures == static_cast<unsigned>(m_selectedFeatures.size()));
    return m_totalFeatures;
}

unsigned UserSelection::totalGenes() const
{
    Q_ASSERT(m_totalGenes == static_cast<unsigned>(m_selectedGenes.size()));
    return m_totalGenes;
}

unsigned UserSelection::totalSpots() const
{
    Q_ASSERT(m_totalGenes == static_cast<unsigned>(m_selectedSpots.size()));
    return m_totalGenes;
}

void UserSelection::id(const QString& id)
{
    m_id = id;
}

void UserSelection::name(const QString& name)
{
    m_name = name;
}

void UserSelection::userId(const QString& userId)
{
    m_userId = userId;
}

void UserSelection::datasetId(const QString& datasetId)
{
    m_datasetId = datasetId;
}

void UserSelection::selectedGenes(const selectedGenesList& selectedGenes)
{
    m_selectedGenes = selectedGenes;
    m_totalGenes = m_selectedGenes.size();
    m_totalReads = std::accumulate(m_selectedGenes.begin(),
                                   m_selectedGenes.end(),
                                   0,
                                   [](unsigned total, const AggregatedGene& item) {
                                       return total + item.reads;
                                   });
}

void UserSelection::selectedFeatures(const selectedFeaturesList& features)
{
    m_selectedFeatures = features;
    m_totalFeatures = m_selectedFeatures.size();
}

void UserSelection::selectedSpots(const selectedSpotsList& spots)
{
    m_selectedSpots = spots;
    m_totalSpots = m_selectedSpots.size();
}

void UserSelection::status(const QString& status)
{
    m_status = status;
}

void UserSelection::oboFoundryTerms(const QVector<QString>& oboFoundryTerms)
{
    m_oboFroundryTerms = oboFoundryTerms;
}

void UserSelection::comment(const QString& comment)
{
    m_comment = comment;
}

void UserSelection::enabled(const bool enabled)
{
    m_enabled = enabled;
}

void UserSelection::created(const QString& created)
{
    m_created = created;
}

void UserSelection::lastModified(const QString& lastModified)
{
    m_lastMofidied = lastModified;
}

void UserSelection::datasetName(const QString& datasetName)
{
    m_datasetName = datasetName;
}

void UserSelection::type(const Type& type)
{
    m_type = type;
}

void UserSelection::tissueSnapShot(const QByteArray& tissueSnapShot)
{
    m_tissueSnapShot = tissueSnapShot;
}

void UserSelection::color(const QColor& color)
{
    m_color = color;
}

void UserSelection::saved(const bool saved)
{
    m_saved = saved;
}

void UserSelection::loadFeatures(const selectedFeaturesList& features)
{
    m_selectedFeatures = features;
    m_selectedGenes.clear();
    m_selectedSpots.clear();
    m_totalReads = 0;
    m_totalGenes = 0;
    m_totalFeatures = 0;
    m_totalSpots = 0;

    // populate the selected genes by aggregation and the spots
    QMap<QString, AggregatedGene> temp_map;
    for (auto feature : features) {
        m_selectedSpots.insert(feature->barcode());
        const QString gene_name = feature->gene();
        const unsigned reads = feature->hits();
        m_totalReads += reads;
        if (temp_map.contains(gene_name)) {
            ++temp_map[gene_name].count;
            temp_map[gene_name].reads += reads;
        } else {
            temp_map[gene_name] = AggregatedGene(gene_name, reads);
        }
    }
    m_selectedGenes = temp_map.values();

    m_totalSpots = m_selectedSpots.size();
    m_totalFeatures = m_selectedFeatures.size();
    m_totalGenes = m_selectedGenes.size();

    // compute TPM values
    selectedGenesList::iterator it;
    for (it = m_selectedGenes.begin(); it != m_selectedGenes.end(); ++it) {
        it->normalizedReads = STMath::tpmNormalization<int>(it->reads, m_totalReads);
    }
}

QString UserSelection::typeToQString(const UserSelection::Type& type)
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

UserSelection::Type UserSelection::QStringToType(const QString& type)
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

    Q_ASSERT_X(true, "UserSelection", "Invalid selection type!");
    // TODO find a cleaner way for this
    return Other;
}

#include "UserSelection.h"

UserSelection::UserSelection()
    : m_name()
    , m_dataset()
    , m_selectedSpots()
    , m_selectedGenes()
    , m_type()
    , m_comment()
    , m_tissueSnapShot()
    , m_totalReads(0)
{
}

UserSelection::UserSelection(const UserSelection &other)
    : m_name(other.m_name)
    , m_dataset(other.m_dataset)
    , m_selectedSpots(other.m_selectedSpots)
    , m_selectedGenes(other.m_selectedGenes)
    , m_type(other.m_type)
    , m_comment(other.m_comment)
    , m_tissueSnapShot(other.m_tissueSnapShot)
    , m_totalReads(other.m_totalReads)
{
}

UserSelection::~UserSelection()
{
}

UserSelection &UserSelection::operator=(const UserSelection &other)
{
    m_name = other.m_name;
    m_dataset = other.m_dataset;
    m_selectedSpots = other.m_selectedSpots;
    m_selectedGenes = other.m_selectedGenes;
    m_type = other.m_type;
    m_comment = other.m_comment;
    m_tissueSnapShot = other.m_tissueSnapShot;
    m_totalReads = other.m_totalReads;
    return (*this);
}

bool UserSelection::operator==(const UserSelection &other) const
{
    return (m_name == other.m_name
            && m_dataset == other.m_dataset
            && m_selectedSpots == other.m_selectedSpots
            && m_selectedGenes == m_selectedGenes
            && m_type == other.m_type
            && m_comment == other.m_comment
            && m_tissueSnapShot == other.m_tissueSnapShot
            && m_totalReads == other.m_totalReads);
}

const QString UserSelection::name() const
{
    return m_name;
}

const QString UserSelection::dataset() const
{
    return m_dataset;
}

const STData::spot_list UserSelection::selectedSpots() const
{
    return m_selectedSpots;
}

const STData::gene_list UserSelection::selectedGenes() const
{
    return m_selectedGenes;
}

const QString UserSelection::comment() const
{
    return m_comment;
}

UserSelection::Type UserSelection::type() const
{
    return m_type;
}

const QByteArray UserSelection::tissueSnapShot() const
{
    return m_tissueSnapShot;
}

int UserSelection::totalReads() const
{
    return m_totalReads;
}


void UserSelection::name(const QString &name)
{
    m_name = name;
}

void UserSelection::dataset(const QString &dataset)
{
    m_dataset = dataset;
}

void UserSelection::selectedSpots(const STData::spot_list &selectedSpots)
{
    m_selectedSpots = selectedSpots;
}

void UserSelection::selectedGenes(const STData::gene_list &selectedGenes)
{
    m_selectedGenes = selectedGenes;
}

void UserSelection::type(const Type &type)
{
    m_type = type;
}

void UserSelection::comment(const QString &comment)
{
    m_comment = comment;
}


void UserSelection::tissueSnapShot(const QByteArray &tissueSnapShot)
{
    m_tissueSnapShot = tissueSnapShot;
}

void UserSelection::totalReads(const int totalReads)
{
    m_totalReads = totalReads;
}

const QString UserSelection::typeToQString(const UserSelection::Type &type)
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

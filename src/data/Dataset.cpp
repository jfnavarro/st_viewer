#include "Dataset.h"
#include "STData.h"

Dataset::Dataset()
    : m_name()
    , m_alignment()
    , m_statTissue()
    , m_statSpecies()
    , m_statComments()
    , m_data_file()
    , m_data(nullptr)
{
}

Dataset::Dataset(const Dataset &other)
{
    m_name = other.m_name;
    m_alignment = other.m_alignment;
    m_statTissue = other.m_statTissue;
    m_statSpecies = other.m_statSpecies;
    m_statComments = other.m_statComments;
    m_data_file = other.m_data_file;
    m_data = other.m_data;
}

Dataset::~Dataset()
{
}

Dataset &Dataset::operator=(const Dataset &other)
{
    m_name = other.m_name;
    m_alignment = other.m_alignment;
    m_statTissue = other.m_statTissue;
    m_statSpecies = other.m_statSpecies;
    m_statComments = other.m_statComments;
    m_data_file = other.m_data_file;
    m_data = other.m_data;
    return (*this);
}

bool Dataset::operator==(const Dataset &other) const
{
    return (m_name == other.m_name
            && m_alignment == other.m_alignment
            && m_statTissue == other.m_statTissue
            && m_statSpecies == other.m_statSpecies
            && m_statComments == other.m_statComments
            && m_data_file == other.m_data_file
            && m_data == other.m_data);
}

const QString Dataset::name() const
{
    return m_name;
}

const std::shared_ptr<STData> Dataset::data() const
{
    return m_data;
}

const QTransform Dataset::imageAlignment() const
{
    return m_alignment;
}

const QString Dataset::statTissue() const
{
    return m_statTissue;
}

const QString Dataset::statSpecies() const
{
    return m_statSpecies;
}

const QString Dataset::statComments() const
{
    return m_statComments;
}

void Dataset::name(const QString &name)
{
    m_name = name;
}

void Dataset::dataFile(const QByteArray &datafile)
{
    m_data_file = datafile;
}

void Dataset::imageAlignment(const QTransform &alignment)
{
    m_alignment = alignment;
}

void Dataset::statTissue(const QString &statTissue)
{
    m_statTissue = statTissue;
}

void Dataset::statSpecies(const QString &statSpecies)
{
    m_statSpecies = statSpecies;
}

void Dataset::statComments(const QString &statComments)
{
    m_statComments = statComments;
}

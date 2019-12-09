#include "UserSelection.h"

UserSelection::UserSelection()
    : m_name()
    , m_dataset()
    , m_data()
    , m_comment()
{
}

UserSelection::UserSelection(const STData::STDataFrame &data)
    : m_name()
    , m_dataset()
    , m_data(data)
    , m_comment()
{
}

UserSelection::~UserSelection()
{
}

UserSelection::UserSelection(const UserSelection &other)
{
    m_name = other.m_name;
    m_dataset = other.m_dataset;
    m_data = other.m_data;
    m_comment = other.m_comment;
}

UserSelection &UserSelection::operator=(const UserSelection &other)
{
    m_name = other.m_name;
    m_dataset = other.m_dataset;
    m_data = other.m_data;
    m_comment = other.m_comment;
    return (*this);
}

bool UserSelection::operator==(const UserSelection &other) const
{
    return (m_name == other.m_name
            && m_dataset == other.m_dataset
            //&& m_data == other.m_data
            && m_comment == other.m_comment);
}

const QString UserSelection::name() const
{
    return m_name;
}

const QString UserSelection::dataset() const
{
    return m_dataset;
}

const STData::STDataFrame &UserSelection::data() const
{
    return m_data;
}

const QString UserSelection::comment() const
{
    return m_comment;
}

int UserSelection::totalGenes() const
{
    return m_data.counts.n_cols;
}

int UserSelection::totalSpots() const
{
    return m_data.counts.n_rows;
}

void UserSelection::name(const QString &name)
{
    m_name = name;
}

void UserSelection::dataset(const QString &dataset)
{
    m_dataset = dataset;
}

void UserSelection::comment(const QString &comment)
{
    m_comment = comment;
}

void UserSelection::data(const STData::STDataFrame &data)
{
    m_data = data;
}


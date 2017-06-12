#include "UserSelection.h"

UserSelection::UserSelection()
    : m_name()
    , m_dataset()
    , m_data()
    , m_comment()
{
}

UserSelection::UserSelection(QSharedPointer<STData> data)
    : m_name()
    , m_dataset()
    , m_data()
    , m_comment()
{
    init(data);
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
            //TODO gotta fix the == for the Matrix type
            //&& m_data.counts == other.m_data.counts
            && m_data.genes == other.m_data.genes
            && m_data.spots == other.m_data.spots
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
    return m_data.genes.size();
}

int UserSelection::totalSpots() const
{
    return m_data.spots.size();
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

void UserSelection::init(QSharedPointer<STData> data)
{
    m_data = data->data();
    const auto selected = data->renderingSelected();

    // Keep only selected spots
    std::vector<uword> to_keep_rows;
    QList<Spot::SpotType> selected_spots;
    for (uword i = 0; i < m_data.counts.n_rows; ++i) {
        if (selected.at(i)) {
            to_keep_rows.push_back(i);
            selected_spots.append(m_data.spots.at(i));
        }
    }
    m_data.spots = selected_spots;
    m_data.counts = m_data.counts.rows(uvec(to_keep_rows));

    // Remove non present genes
    std::vector<uword> to_keep_genes;
    QList<QString> selected_genes;
    for (uword j = 0; j < m_data.counts.n_cols; ++j) {
        if (sum(m_data.counts.col(j)) > 0) {
            to_keep_genes.push_back(j);
            selected_genes.append(m_data.genes.at(j));
        }
    }
    m_data.genes = selected_genes;
    m_data.counts = m_data.counts.cols(uvec(to_keep_genes));
}


#include "UserSelection.h"

UserSelection::UserSelection(QSharedPointer<STData> data)
    : m_name()
    , m_dataset()
    , m_counts()
    , m_genes()
    , m_spots()
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
    m_counts = other.m_counts;
    m_genes = other.m_genes;
    m_spots = other.m_spots;
    m_comment = other.m_comment;
}

UserSelection &UserSelection::operator=(const UserSelection &other)
{
    m_name = other.m_name;
    m_dataset = other.m_dataset;
    m_counts = other.m_counts;
    m_genes = other.m_genes;
    m_spots = other.m_spots;
    m_comment = other.m_comment;
    return (*this);
}

bool UserSelection::operator==(const UserSelection &other) const
{
    return (m_name == other.m_name
            && m_dataset == other.m_dataset
            //TODO gotta fix the == for the Matrix type
            //&& m_counts == other.m_counts
            && m_genes == other.m_genes
            && m_spots == other.m_spots
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

const STData::Matrix &UserSelection::data() const
{
    return m_counts;
}

const UserSelection::GeneListType &UserSelection::genes() const
{
    return m_genes;
}

const UserSelection::SpotListType &UserSelection::spots() const
{
    return m_spots;
}

const QString UserSelection::comment() const
{
    return m_comment;
}

int UserSelection::totalGenes() const
{
    return m_genes.size();
}

int UserSelection::totalSpots() const
{
    return m_spots.size();
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

void UserSelection::init(QSharedPointer<STData> data)
{
    m_counts = data->matrix_counts();
    const auto genes = data->genes();
    const auto spots = data->spots();
    const auto selected = data->renderingSelected();

    std::vector<uword> to_keep_rows;
    for (uword i = 0; i < m_counts.n_rows; ++i) {
        if (selected.at(i)) {
            to_keep_rows.push_back(i);
            m_spots.append(spots.at(i)->coordinates());
        }
    }
    m_counts = m_counts.rows(uvec(to_keep_rows));

    std::vector<uword> to_keep_genes;
    for (uword j = 0; j < m_counts.n_cols; ++j) {
        if (sum(m_counts.col(j)) > 0) {
            to_keep_genes.push_back(j);
            m_genes.append(genes.at(j)->name());
        }
    }
    m_counts = m_counts.cols(uvec(to_keep_genes));
}

void UserSelection::save(const QString filename) const
{
    Q_UNUSED(filename)
}

void UserSelection::load(const QString filename)
{
    Q_UNUSED(filename)
}

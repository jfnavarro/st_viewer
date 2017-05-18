#include "STData.h"
#include <QDebug>

STData::STData():
   m_counts_matrix(),
   m_counts_norm_matrix(),
   m_normalization(Normalization::RAW),
   m_spots(),
   m_genes(),
   m_matrix_genes(),
   m_matrix_spots(),
   m_spot_count_threshold(0),
   m_gene_count_threshold(0),
   m_spot_gene_count_threshold(0)
{
    Q_UNUSED(m_normalization)
    Q_UNUSED(m_spot_count_threshold)
    Q_UNUSED(m_gene_count_threshold)
    Q_UNUSED(m_spot_gene_count_threshold)
}

STData::STData(const STData &other)
{
    Q_UNUSED(other);
    //TODO implement
}

STData::~STData()
{
}

STData &STData::operator=(const STData &other)
{
    Q_UNUSED(other);
    //TODO implement
    return (*this);
}

bool STData::operator==(const STData &other) const
{
    Q_UNUSED(other);
    //TODO implement
    return true;
}

void STData::read(const QString &filename) {
    // TODO the values type can be templated
    m_matrix_genes.clear();
    m_matrix_spots.clear();
    m_genes.clear();
    m_spots.clear();
    std::vector<std::vector<float>> values;

    // Open file
    std::ifstream f(filename.toStdString());
    qDebug() << "Opening file " << filename;

    // Process the rest of the lines (row names and counts)
    int row_number = 0;
    int col_number = 0;
    char sep = '\t';
    for (std::string line; std::getline(f, line);) {
        std::istringstream iss(line);
        std::string token;
        std::vector<float> values_row;
        col_number = 0;
        while(std::getline(iss, token, sep)) {
            if (row_number == 0) {
                const GeneType gene = QString::fromStdString(token);
                m_matrix_genes.push_back(gene);
                m_genes.append(GeneObjectType(new Gene(gene)));
            } else if (col_number == 0) {
                const QString spot_tmp = QString::fromStdString(token);
                const QStringList items = spot_tmp.split("x");
                Q_ASSERT(items.size() == 2);
                float x = items.at(0).toFloat();
                float y = items.at(1).toFloat();
                m_matrix_spots.push_back(SpotType(x,y));
                m_spots.append(SpotObjectType(new Spot(x,y)));
            } else {
                values_row.push_back(std::stod(token));
            }
            ++col_number;
        }
        if (row_number > 0) {
            values.push_back(values_row);
        }
        ++row_number;
    }
    // Close file
    f.close();

    // Remove the first column
    if (m_matrix_genes.size() == col_number) {
        m_matrix_genes.removeAt(0);
        m_genes.removeAt(0);
    }

    if (m_matrix_spots.empty() || m_matrix_genes.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    // Create an armadillo matrix
    Mat<float> counts_matrix(row_number - 1, col_number - 1);
    for (int i = 0; i < row_number - 1; ++i) {
        for (int j = 0; j < col_number - 1; ++j) {
            counts_matrix(i, j) = values[i][j];
        }
    }
    m_counts_matrix = counts_matrix;
}

void STData::save(const QString &filename) const
{
    Q_UNUSED(filename)
}

void STData::normalize(Normalization normalization)
{
    Q_UNUSED(normalization)
}

std::vector<float> STData::count(const GeneType &gene) const
{
    Q_UNUSED(gene)
    return std::vector<float>();
}

std::vector<float> STData::count(const SpotType &spot) const
{
    Q_UNUSED(spot)
    return std::vector<float>();
}

float STData::count(const GeneType &gene, const SpotType &spot) const
{
    const uword i = m_matrix_genes.indexOf(gene);
    const uword j = m_matrix_spots.indexOf(spot);
    return m_counts_matrix.at(i,j);
}

std::vector<float> STData::count(const GeneObjectType &gene) const
{
    Q_UNUSED(gene)
    return std::vector<float>();
}

std::vector<float> STData::count(const SpotObjectType &spot) const
{
    Q_UNUSED(spot)
    return std::vector<float>();
}

float STData::count(const GeneObjectType &gene, const SpotObjectType &spot) const
{
    const uword i = m_genes.indexOf(gene);
    const uword j = m_spots.indexOf(spot);
    return m_counts_matrix.at(i,j);
}

Mat<float> STData::slice_matrix_counts() const
{
    return m_counts_matrix;
}

Mat<float> STData::matrix_counts() const
{
    return m_counts_matrix;
}

std::vector<float> STData::spots_counts()
{
    return std::vector<float>();
}

std::vector<float> STData::genes_counts()
{
    return std::vector<float>();
}

QVector<QColor> STData::spots_colors() const
{
    return QVector<QColor>();
}

STData::GeneType STData::gene_at(size_t index) const
{
    return m_matrix_genes.at(index);
}

STData::SpotType STData::spot_at(size_t index) const
{
    return m_matrix_spots.at(index);
}

STData::GeneObjectType STData::gene_object_at(size_t index) const
{
    return m_genes.at(index);
}

STData::SpotObjectType STData::spot_object_at(size_t index) const
{
    return m_spots.at(index);
}

size_t STData::number_spots() const
{
    return m_spots.size();
}

size_t STData::number_genes() const
{
    return m_genes.size();
}

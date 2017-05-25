#include "STData.h"
#include <QDebug>

static const int ROW = 1;
static const int COLUMN = 0;

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

STData::GeneListType STData::genes()
{
    return m_genes;
}

STData::SpotListType STData::spots()
{
    return m_spots;
}

void STData::compuateGenesCutoff()
{
    /*
    const int minseglen = 2;
    for (auto gene : m_dataProxy->getGeneList()) {
        Q_ASSERT(gene);
        // get all the counts of the spots that contain that gene
        auto counts = m_geneInfoByGeneFeatures.value(gene);
        const size_t num_features = counts.size();
        // if too little counts or if all the counts are the same cut off is the min count present
        if (num_features < minseglen + 1
            || std::equal(counts.begin() + 1, counts.end(), counts.begin())) {
            const int cutoff = *std::min_element(counts.begin(), counts.end());
            gene->cut_off(cutoff);
            continue;
        }
        // sort the counts and compute a list of their squared sum
        std::sort(counts.begin(), counts.end());
        std::vector<int> squared_summed_counts(counts);
        std::transform(squared_summed_counts.begin(),
                       squared_summed_counts.end(),
                       squared_summed_counts.begin(),
                       squared_summed_counts.begin(),
                       std::multiplies<int>());
        std::partial_sum(squared_summed_counts.begin(),
                         squared_summed_counts.end(),
                         squared_summed_counts.begin());
        squared_summed_counts.insert(squared_summed_counts.begin(), 0);
        // generate a vector taustar with indexes of the counts
        std::vector<int> taustar;
        int n = minseglen;
        std::generate_n(std::back_inserter(taustar),
                        num_features + minseglen - 2 - 2,
                        [n]() mutable { return n++; });
        std::vector<float> tmp1;
        std::vector<float> tmp2;
        std::vector<float> tmp3;
        const float last_count = static_cast<float>(squared_summed_counts.back());
        // perform tmp3 = (squared_summed_counts / last_count) - (taustar / num_counts)
        std::transform(squared_summed_counts.begin() + 2,
                       squared_summed_counts.end() - 1,
                       std::back_inserter(tmp1),
                       [=](int count) { return count / last_count; });
        std::transform(taustar.begin(),
                       taustar.end(),
                       std::back_inserter(tmp2),
                       [=](int tau_value)
        { return tau_value / static_cast<float>(num_features); });
        std::transform(tmp1.begin(),
                       tmp1.end(),
                       tmp2.begin(),
                       std::back_inserter(tmp3),
                       [](float a, float b) { return std::fabs(a - b); });
        // tau is the distance to the max element in tmp3 which is an index
        const auto tau = std::distance(tmp3.begin(), std::max_element(tmp3.begin(), tmp3.end()));
        // get the read count of the tau index and that is the gene cut off
        const int est_readcount
            = *std::upper_bound(counts.begin(), counts.end(), counts.at(tau));
        gene->cut_off(est_readcount);
    }
    */
}

float STData::min_genes_spot() const
{
    return cumsum(m_counts_matrix, COLUMN).min();
}

float STData::max_genes_spot() const
{
    return cumsum(m_counts_matrix, COLUMN).max();
}

float STData::min_reads_spot() const
{
    return cumsum(m_counts_matrix, ROW).min();
}

float STData::max_reads_spot() const
{
    return cumsum(m_counts_matrix, ROW).max();
}

float STData::max_reads() const
{
    return m_counts_matrix.max();
}

float STData::min_reads() const
{
    return m_counts_matrix.min();
}

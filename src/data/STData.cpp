#include "STData.h"
#include <QDebug>
#include "math/Common.h"

static const int ROW = 1;
static const int COLUMN = 0;
//static const int QUAD_SIZE = 4;
static const QVector2D ta(0.0, 0.0);
static const QVector2D tb(0.0, 1.0);
static const QVector2D tc(1.0, 1.0);
static const QVector2D td(1.0, 0.0);

using namespace Math;

namespace
{

QVector4D fromQtColor(const QColor color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}
}

STData::STData()
   : m_counts_matrix()
   , m_counts_norm_matrix()
   , m_spots()
   , m_genes()
   , m_matrix_genes()
   , m_matrix_spots()
   , m_rendering_settings(nullptr)
   , m_normalization(SettingsWidget::NormalizationMode::RAW)
   , m_vertices()
   , m_textures()
   , m_colors()
   , m_indexes()

{

}

STData::~STData()
{

}

void STData::read(const QString &filename) {
    m_matrix_genes.clear();
    m_matrix_spots.clear();
    m_genes.clear();
    m_spots.clear();
    std::vector<std::vector<float>> values;

    // Open file
    std::ifstream f(filename.toStdString());
    qDebug() << "Opening ST Data file " << filename;

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

STData::Matrix STData::slice_matrix_counts() const
{
    return m_counts_matrix;
}

STData::Matrix STData::matrix_counts() const
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

void STData::computeRenderingData()
{
    //TODO normalize the counts
    //TODO parallelize this
    // Iterate all the spots
    for (int i = 0; i < m_spots.size(); ++i) {
        const auto spot = m_spots.at(i);
        if (!spot->visible() || accu(cumsum(m_counts_matrix, ROW))
                < m_rendering_settings->reads_threshold) {
            continue;
        }
        // Iterage the genes in the spot to compute the sum of values and color
        QColor merged_color;
        float merged_value = 0;
        int num_genes = 0;
        for (int j = 0; j < m_genes.size(); ++j) {
            const auto gene = m_genes.at(i);
            const float value = m_counts_matrix.at(i,j);
            if (!gene->selected() || value < m_rendering_settings->ind_reads_threshold) {
                continue;
            }
            ++num_genes;
            merged_value += value;
            merged_color = lerp(1.0 / num_genes, merged_color, gene->color());
        }
        // Update the color of the spot
        if (num_genes > m_rendering_settings->genes_threshold) {
            //TODO adjust color for type and mode
            //TODO adjust color for intensity
            m_colors[i] = fromQtColor(merged_color);
        } else {
            // not visible
            m_colors[i] = QVector4D(0.0, 0.0, 0.0, 0.0);
        }
    }
}

const QVector<unsigned> &STData::renderingIndexes() const
{
    return m_indexes;
}

const QVector<QVector3D> &STData::renderingVertices() const
{
    return m_vertices;
}

const QVector<QVector2D> &STData::renderingTextures() const
{
    return m_textures;
}

const QVector<QVector4D> &STData::renderingColors() const
{
    return m_colors;
}

void STData::updateSize(const float size)
{
    Q_UNUSED(size)
}

void STData::initRenderingData()
{
    m_vertices.clear();
    m_textures.clear();
    m_colors.clear();
    m_indexes.clear();
    const QVector4D opengl_color = fromQtColor(Qt::white);
    const float size = 1.0;
    for (const SpotType &spot : m_matrix_spots) {
        const int index_count = static_cast<int>(m_vertices.size());
        const float x = spot.first;
        const float y = spot.second;
        m_vertices.append(QVector3D(x - size / 2.0, y - size / 2.0, 0.0));
        m_vertices.append(QVector3D(x + size / 2.0, y - size / 2.0, 0.0));
        m_vertices.append(QVector3D(x + size / 2.0, y + size / 2.0, 0.0));
        m_vertices.append(QVector3D(x - size / 2.0, y + size / 2.0, 0.0));
        m_textures.append(ta);
        m_textures.append(tb);
        m_textures.append(tc);
        m_textures.append(td);
        m_colors.append(opengl_color);
        m_colors.append(opengl_color);
        m_colors.append(opengl_color);
        m_colors.append(opengl_color);
        m_indexes.append(index_count);
        m_indexes.append(index_count + 1);
        m_indexes.append(index_count + 2);
        m_indexes.append(index_count);
        m_indexes.append(index_count + 2);
        m_indexes.append(index_count + 3);
    }
}

void STData::computeGenesCutoff()
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

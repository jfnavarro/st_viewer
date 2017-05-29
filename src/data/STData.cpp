#include "STData.h"
#include <QDebug>
#include "math/Common.h"
#include "color/HeatMap.h"
#include "R.h"
#include "Rcpp.h"
#include "RInside.h"

static const int ROW = 1;
static const int COLUMN = 0;
static const int QUAD_SIZE = 4;
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
    , m_deseq_size_factors()
    , m_scran_size_factors()
    , m_spots()
    , m_genes()
    , m_matrix_genes()
    , m_matrix_spots()
    , m_rendering_settings(nullptr)
    //, m_normalization(SettingsWidget::NormalizationMode::RAW)
    , m_vertices()
    , m_textures()
    , m_colors()
    , m_indexes()

{
    R = new RInside();
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
    Matrix counts_matrix(row_number - 1, col_number - 1);
    for (int i = 0; i < row_number - 1; ++i) {
        for (int j = 0; j < col_number - 1; ++j) {
            counts_matrix.at(i, j) = values[i][j];
        }
    }
    m_counts_matrix = counts_matrix;

    computeDESeqFactors();
    computeScranFactors();
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
    return sum(m_counts_matrix, COLUMN).min();
}

float STData::max_genes_spot() const
{

    return sum(m_counts_matrix, COLUMN).max();
}

float STData::min_reads_spot() const
{
    return sum(m_counts_matrix, ROW).min();
}

float STData::max_reads_spot() const
{
    return sum(m_counts_matrix, ROW).max();
}

float STData::max_reads() const
{
    return m_counts_matrix.max();
}

float STData::min_reads() const
{
    return m_counts_matrix.min();
}

void STData::setRenderingSettings(const SettingsWidget::Rendering *rendering_settings)
{
    m_rendering_settings = rendering_settings;
}

void STData::computeRenderingData()
{
    Q_ASSERT(m_rendering_settings != nullptr);
    Q_ASSERT(!m_colors.empty());

    //TODO normalize the counts and/or log the counts
    //TODO parallelize this

    // Get the list of selected genes
    GeneListType selected_genes;
    std::copy_if(m_genes.begin(), m_genes.end(),
                 std::back_inserter(selected_genes), [&](const auto gene) {
        return gene->visible();
    });

    // Empty color for non visible spots
    const QColor empty(0.0, 0.0, 0.0, 0.0);

    const Col<float> &col_sums = sum(m_counts_matrix, ROW);

    float min_genes = 10e6;
    float max_genes = -1.0;
    float min_reads = 10e6;
    float max_reads = -1.0;
    for (int i = 0; i < m_spots.size(); ++i) {
        const auto spot = m_spots.at(i);
        if (!spot->visible() || col_sums.at(i) < m_rendering_settings->reads_threshold) {
            updateColor(i, empty);
            continue;
        }
        // Iterage the genes in the spot to compute the sum of values and color
        QColor merged_color;
        float merged_value = 0;
        float num_genes = 0;
        for (int j = 0; j < selected_genes.size(); ++j) {;
            const auto gene = selected_genes[j];
            const float value = m_counts_matrix.at(i,j);
            if (m_rendering_settings->gene_cutoff && gene->cut_off() > value) {
                continue;
            }
            ++num_genes;
            merged_value += value;
            merged_color = lerp(1.0 / num_genes, merged_color, gene->color());
            min_reads = std::min(value, min_reads);
            max_reads = std::max(value, max_reads);
        }
        min_genes = std::min(num_genes, min_genes);
        max_genes = std::max(num_genes, max_genes);

        // Adjust spot's value according to type of visualization
        switch (m_rendering_settings->visual_type_mode) {
        case (SettingsWidget::VisualTypeMode::Reads): {
        } break;
        case (SettingsWidget::VisualTypeMode::ReadsLog): {
            merged_value = std::log(merged_value);
            min_reads = std::log(min_reads);
            max_reads = std::log(max_reads);
        } break;
        case (SettingsWidget::VisualTypeMode::Genes): {
            merged_value = num_genes;
            min_reads = min_genes;
            max_reads = max_genes;
        } break;
        case (SettingsWidget::VisualTypeMode::GenesLog): {
            merged_value = std::log(num_genes);
            min_reads = std::log(min_genes);
            max_reads = std::log(max_genes);
        }
        }

        // Update the color of the spot
        QColor color = spot->color();
        if (num_genes > m_rendering_settings->genes_threshold && color == Qt::white) {
            switch (m_rendering_settings->visual_mode) {
            case (SettingsWidget::VisualMode::Normal): {
                color = merged_color;
            } break;
            case (SettingsWidget::VisualMode::DynamicRange): {
                color = Color::createDynamicRangeColor(merged_value, min_reads,
                                                       max_reads, merged_color);
            } break;
            case (SettingsWidget::VisualMode::HeatMap): {
                color = Color::createCMapColor(merged_value, min_reads,
                                               max_reads, Color::ColorGradients::gpSpectrum);
            } break;
            case (SettingsWidget::VisualMode::ColorRange): {
                color = Color::createCMapColor(merged_value, min_reads,
                                               max_reads, Color::ColorGradients::gpHot);
            }
            }
        }
        color.setAlphaF(m_rendering_settings->intensity);
        updateColor(i, color);
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
    for (int index = 0; index < m_spots.size(); ++index) {
        const auto spot = m_matrix_spots.at(index);
        const float x = spot.first;
        const float y = spot.second;
        m_vertices[(QUAD_SIZE * index)] =
                QVector3D(x - size / 2.0, y - size / 2.0, 0.0);
        m_vertices[(QUAD_SIZE * index) + 1] =
                QVector3D(x + size / 2.0, y - size / 2.0, 0.0);
        m_vertices[(QUAD_SIZE * index) + 2] =
                QVector3D(x + size / 2.0, y + size / 2.0, 0.0);
        m_vertices[(QUAD_SIZE * index) + 3] =
                QVector3D(x - size / 2.0, y + size / 2.0, 0.0);
    }
}

void STData::initRenderingData()
{
    m_vertices.clear();
    m_textures.clear();
    m_colors.clear();
    m_indexes.clear();
    const QVector4D opengl_color = fromQtColor(Qt::white);
    const float size = 0.5;
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

void STData::updateColor(const int index, const QColor &color)
{
    const QVector4D opengl_color = fromQtColor(color);
    for (int z = 0; z < QUAD_SIZE; ++z) {
        m_colors[(QUAD_SIZE * index) + z] = opengl_color;
    }
}

void STData::computeDESeqFactors()
{

}

void STData::computeScranFactors()
{

}

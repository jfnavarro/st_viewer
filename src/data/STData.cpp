#include "STData.h"
#include <QDebug>
#include <QMessageBox>
#include "math/Common.h"
#include "color/HeatMap.h"
#include "math/RInterface.h"

static const int ROW = 1;
//static const int COLUMN = 0;
static const int QUAD_SIZE = 4;
static const QVector2D ta(0.0, 0.0);
static const QVector2D tb(0.0, 1.0);
static const QVector2D tc(1.0, 1.0);
static const QVector2D td(1.0, 0.0);
static const QColor cempty(0.0, 0.0, 0.0, 0.0);

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
    , m_selected()
    , m_vertices()
    , m_textures()
    , m_colors()
    , m_indexes()
    , m_quadTree()
    , m_size(0.5)
{

}

STData::~STData()
{

}

void STData::read(const QString &filename) {
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
                const QString gene = QString::fromStdString(token).trimmed();
                m_genes.append(GeneObjectType(new Gene(gene)));
            } else if (col_number == 0) {
                const QString spot_tmp = QString::fromStdString(token).trimmed();
                const QStringList items = spot_tmp.split("x");
                Q_ASSERT(items.size() == 2);
                const float x = items.at(0).toFloat();
                const float y = items.at(1).toFloat();
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

    // Remove the first empty column name (gene)
    if (m_genes.size() == col_number) {
        m_genes.removeAt(0);
    }

    if (m_spots.empty() || m_genes.empty()) {
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

    // Initialize quad tree
    m_quadTree.clear();
    const QRectF border = getBorder();
    m_quadTree = SpotsQuadTree(QuadTreeAABB(border));

    // Filter out almost non-present spots (total count < 5)
    for (uword i = 0; i < m_counts_matrix.n_rows; ++i) {
        if (accu(m_counts_matrix.row(i)) < 5) {
            m_counts_matrix.shed_row(i);
            m_spots.removeAt(i);
        } else {
            // add the coordinates to the quad tree
            const auto spot = m_spots[i]->coordinates();
            m_quadTree.insert(QPointF(spot.first, spot.second), i);
        }
    }

    // Compute these only when the dataset is created
    computeGenesCutoff();
    m_deseq_size_factors = RInterface::computeDESeqFactors(m_counts_matrix);
    m_scran_size_factors = RInterface::computeScranFactors(m_counts_matrix);
}

void STData::save(const QString &filename) const
{
    //TODO implement
    Q_UNUSED(filename)
}

STData::Matrix STData::matrix_counts() const
{
    return m_counts_matrix;
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

void STData::computeRenderingData(SettingsWidget::Rendering &rendering_settings)
{
    Q_ASSERT(m_counts_matrix.size() > 0);
    Q_ASSERT(!m_colors.empty());
    Q_ASSERT(!m_selected.empty());
    Q_ASSERT(m_selected.size() == m_colors.size());

    const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;

    // Normalize the counts
    Matrix counts = normalizeCounts(m_counts_matrix,
                                    rendering_settings.normalization_mode,
                                    m_deseq_size_factors,
                                    m_scran_size_factors);

    // Update spots size
    if (m_size != rendering_settings.size) {
        m_size = rendering_settings.size;
        updateSize(m_size);
    }

    // Get the list of selected genes
    rowvec colsum_nonzero = computeNonZeroColumns(counts);
    QVector<uword> selected_genes_indexes;
    float max_value_gene = -1;
    float min_value_gene = 10e6;
    for (uword i = 0; i < counts.n_cols; ++i) {
        const auto gene = m_genes[i];
        const float gene_count = colsum_nonzero(i);
        if (gene->visible() && gene_count > rendering_settings.genes_threshold) {
            selected_genes_indexes.push_back(i);
            max_value_gene = std::max(max_value_gene, gene_count);
            min_value_gene = std::min(min_value_gene, gene_count);
        }
    }

    // Get the list of selected spots
    colvec rowsum = sum(counts, ROW);
    QVector<uword> selected_spots_indexes;
    float max_value_reads = -1;
    float min_value_reads = 10e6;
    for (uword i = 0; i < counts.n_rows; ++i) {
        updateSelected(i, false);
        updateColor(i, cempty);
        const float reads_count = rowsum(i);
        if (reads_count > rendering_settings.reads_threshold) {
            selected_spots_indexes.push_back(i);
            max_value_reads = std::max(max_value_reads, reads_count);
            min_value_reads = std::min(min_value_reads, reads_count);
        }
    }

    // early out
    if (selected_spots_indexes.empty() && selected_genes_indexes.empty()) {
        return;
    }

    // Compute color adjusment constants
    float min_value = use_genes ? min_value_gene : min_value_reads;
    float max_value = use_genes ? max_value_gene : max_value_reads;
    min_value = use_log ? std::log(min_value + std::numeric_limits<float>::epsilon()) : min_value;
    max_value = use_log ? std::log(max_value + std::numeric_limits<float>::epsilon()) : max_value;
    rendering_settings.legend_min = min_value;
    rendering_settings.legend_max = max_value;

    // Iterate the spots and genes in the matrix to compute the rendering colors
    //TODO make this paralell
    for (const uword i : selected_spots_indexes) {
        const auto spot = m_spots[i];
        bool visible = false;
        float merged_value = 0;
        float num_genes = 0;
        bool any_gene_selected = false;
        QColor merged_color;
        if (!spot->visible()) {
            merged_value = 0;
            num_genes = 0;
            // Iterage the genes in the spot to compute the sum of values and color
            for (const uword j : selected_genes_indexes) {;
                const auto gene = m_genes[j];
                const float value = counts.at(i,j);
                if (rendering_settings.gene_cutoff && gene->cut_off() > value) {
                    continue;
                }
                ++num_genes;
                merged_value += value;
                merged_color = lerp(1.0 / num_genes, merged_color, gene->color());
                any_gene_selected |= gene->selected();
            }
            // Use number of genes or total reads in the spot depending on settings
            merged_value = use_genes ? num_genes : merged_value;

            // Update the color of the spot
            if (merged_value > 0) {
                // Update the counts with the visual type (log or not)
                merged_value = use_log ? std::log(merged_value) : merged_value;
                merged_color = adjustVisualMode(merged_color,
                                                merged_value,
                                                min_value,
                                                max_value,
                                                rendering_settings.visual_mode);
                visible = true;
                spot->selected(spot->selected() || any_gene_selected);
            }
        } else {
            visible = true;
            merged_color = spot->color();
        }

        updateSelected(i, spot->selected() && visible);
        if (visible) {
            merged_color.setAlphaF(rendering_settings.intensity);
            updateColor(i, merged_color);
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

const QVector<float> &STData::renderingSelected() const
{
    return m_selected;
}

void STData::updateSize(const float size)
{
    for (int index = 0; index < m_spots.size(); ++index) {
        const auto spot = m_spots[index]->coordinates();
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
    m_selected.clear();
    m_vertices.clear();
    m_textures.clear();
    m_colors.clear();
    m_indexes.clear();
    m_size = 0.5;
    const QVector4D default_color = fromQtColor(cempty);
    const bool default_sected = false;
    for (const auto spot : m_spots) {
        const int index_count = static_cast<int>(m_vertices.size());
        const auto spot_cor = spot->coordinates();
        const float x = spot_cor.first;
        const float y = spot_cor.second;
        m_vertices.append(QVector3D(x - m_size / 2.0, y - m_size / 2.0, 0.0));
        m_vertices.append(QVector3D(x + m_size / 2.0, y - m_size / 2.0, 0.0));
        m_vertices.append(QVector3D(x + m_size / 2.0, y + m_size / 2.0, 0.0));
        m_vertices.append(QVector3D(x - m_size / 2.0, y + m_size / 2.0, 0.0));
        m_textures.append(ta);
        m_textures.append(tb);
        m_textures.append(tc);
        m_textures.append(td);
        m_colors.append(default_color);
        m_colors.append(default_color);
        m_colors.append(default_color);
        m_colors.append(default_color);
        m_indexes.append(index_count);
        m_indexes.append(index_count + 1);
        m_indexes.append(index_count + 2);
        m_indexes.append(index_count);
        m_indexes.append(index_count + 2);
        m_indexes.append(index_count + 3);
        m_selected.append(default_sected);
        m_selected.append(default_sected);
        m_selected.append(default_sected);
        m_selected.append(default_sected);
    }
}

bool STData::parseSpotsMap(const QString &spots_file)
{
    qDebug() << "Parsing spots file " << spots_file;
    QMap<Spot::SpotType, Spot::SpotType> spotMap;
    QFile file(spots_file);
    // Parse the spots map = old_spot -> new_spot
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        bool parsed = true;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() != 4 && fields.length() != 6) {
                parsed = false;
                break;
            }
            const float orig_x = fields.at(0).toFloat();
            const float orig_y = fields.at(1).toFloat();
            const float new_x = fields.at(2).toFloat();
            const float new_y = fields.at(3).toFloat();
            spotMap.insert(Spot::SpotType(orig_x, orig_y), Spot::SpotType(new_x, new_y));
        }

        if (spotMap.empty() || !parsed) {
            QMessageBox::warning(nullptr,
                                 QObject::tr("Spot coordinates"),
                                 QObject::tr("No valid spots were found in the given file"));
            file.close();
            return false;
        }

    } else {
        qDebug() << "Could not open spots file";
        file.close();
        return false;
    }
    file.close();

    // Update matrix and containers
    QVector<uword> remove_indexes;
    for (uword i = 0; i < m_counts_matrix.n_rows; ++i) {
        auto oldspot = m_spots[i]->coordinates();
        if (spotMap.contains(oldspot)) {
            m_spots[i]->coordinates(spotMap[oldspot]);
        } else {
            remove_indexes.push_back(i);
        }
    }

    if (remove_indexes.size() == m_spots.size()) {
        QMessageBox::warning(nullptr,
                             QObject::tr("Spot coordinates"),
                             QObject::tr("No spots were matched in the spots file"));
        return true;
    }

    if (remove_indexes.size() < m_spots.size() && !remove_indexes.empty()) {
        const int answer = QMessageBox::warning(nullptr,
                                                QObject::tr("Spots coordinates"),
                                                QObject::tr("Some spots in the data matrix were not found"
                                                            "in the file\n. Do you want to keep them?"),
                                                QMessageBox::Yes,
                                                QMessageBox::No | QMessageBox::Escape);

        if (answer == QMessageBox::No) {
            //Remove spots from the matrix and the containers
            for (const uword index : remove_indexes) {
                m_spots.removeAt(index);
                m_counts_matrix.shed_row(index);
            }
        }
    }

    return true;
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

void STData::updateSelected(const int index, const bool &selected)
{
    for (int z = 0; z < QUAD_SIZE; ++z) {
        m_selected[(QUAD_SIZE * index) + z] = static_cast<float>(selected);
    }
}



QColor STData::adjustVisualMode(const QColor merged_color,
                                const float &merged_value,
                                const float &min_reads,
                                const float &max_reads,
                                const SettingsWidget::VisualMode mode)
{
    QColor color = merged_color;
    switch (mode) {
    case (SettingsWidget::VisualMode::Normal): {
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
    return color;
}

STData::Matrix STData::normalizeCounts(const Matrix &counts,
                                       SettingsWidget::NormalizationMode mode,
                                       const rowvec &deseq_factors,
                                       const rowvec &scran_factors)
{
    Matrix norm_counts = counts;
    switch (mode) {
    case (SettingsWidget::NormalizationMode::RAW): {
    } break;
    case (SettingsWidget::NormalizationMode::REL): {
        norm_counts.each_col() /= sum(norm_counts, ROW);
    } break;
    case (SettingsWidget::NormalizationMode::TPM): {
        norm_counts.each_col() /= (sum(norm_counts, ROW) * 1e6);
    } break;
    case (SettingsWidget::NormalizationMode::DESEQ): {
        norm_counts.each_col() /= deseq_factors.t();
    } break;
    case (SettingsWidget::NormalizationMode::SCRAN): {
        norm_counts.each_col() /= scran_factors.t();
    }
    }
    return counts;
}

STData::rowvec STData::computeNonZeroColumns(const STData::Matrix &matrix)
{
    rowvec non_zeros(matrix.n_cols);
    for (uword i = 0; i < matrix.n_cols; ++i) {
        int non_zero = 0;
        for (uword j = 0; j < matrix.n_rows; ++j) {
            if (matrix.at(j,i) > 0.0) {
                ++non_zero;
            }
        }
        non_zeros[i] = non_zero;
    }
    return non_zeros;
}

STData::colvec computeNonZeroRows(const STData::Matrix &matrix)
{
    STData::colvec non_zeros(matrix.n_rows);
    for (uword i = 0; i < matrix.n_rows; ++i) {
        int non_zero = 0;
        for (uword j = 0; j < matrix.n_cols; ++j) {
            if (matrix.at(i,j) > 0.0) {
                ++non_zero;
            }
        }
        non_zeros[i] = non_zero;
    }
    return non_zeros;
}

void STData::clearSelection()
{
    for (int i = 0; i < m_spots.size(); ++i) {
        m_spots[i]->selected(false);
        updateSelected(i, false);
    }

    for (auto gene : m_genes) {
        gene->selected(false);
    }
}

void STData::selectSpots(const SelectionEvent &event)
{
    const QRectF rect = event.path();
    const auto mode = event.mode();

    if (mode == SelectionEvent::SelectionMode::NewSelection) {
        clearSelection();
    }

    // get selection area
    const QuadTreeAABB aabb(rect);

    // get selected points from selection shape
    SpotsQuadTree::PointItemList pointList;
    m_quadTree.select(aabb, pointList);

    // update selection
    const bool remove = (mode == SelectionEvent::SelectionMode::ExcludeSelection);
    for (const auto point : pointList) {
        const int index = point.second;
        m_spots[index]->selected(!remove);
    }
}

void STData::selectGenes(const QRegExp &regexp, const bool force)
{
    for (auto gene : m_genes) {
        gene->selected(regexp.exactMatch(gene->name()));
        gene->visible(gene->visible() || force);
    }
}

const QRectF STData::getBorder() const
{
    /*const auto mm_x = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().first < rhs->coordinates().first;});

    const auto mm_y = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().second < rhs->coordinates().second;});

    const auto min_x = (*mm_x.first)->coordinates().first;
    const auto min_y = (*mm_y.first)->coordinates().second;
    const auto max_x = (*mm_x.first)->coordinates().second;
    const auto max_y = (*mm_y.first)->coordinates().first;*/
    return QRectF(QPointF(1, 1), QPointF(33, 35));
}

#include "STData.h"
#include <QDebug>
#include <QMessageBox>
#include "math/Common.h"
#include "color/HeatMap.h"
#include "math/RInterface.h"

static const int ROW = 1;
//static const int COLUMN = 0;
using namespace Math;

STData::STData()
    : m_data()
    , m_deseq_size_factors()
    , m_scran_size_factors()
    , m_spots()
    , m_genes()
{

}

STData::~STData()
{

}

STData::STDataFrame STData::read(const QString &filename)
{
    STDataFrame data;
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
                data.genes.append(gene);
            } else if (col_number == 0) {
                const QString spot_tmp = QString::fromStdString(token).trimmed();
                const QStringList items = spot_tmp.split("x");
                Q_ASSERT(items.size() == 2);
                const float x = items.at(0).toFloat();
                const float y = items.at(1).toFloat();
                data.spots.append(Spot::SpotType(x,y));
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
    if (data.genes.size() == col_number) {
        data.genes.removeAt(0);
    }

    if (data.spots.empty() || data.genes.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    // Create an armadillo matrix
    mat counts_matrix(row_number - 1, col_number - 1);
    for (int i = 0; i < row_number - 1; ++i) {
        for (int j = 0; j < col_number - 1; ++j) {
            counts_matrix.at(i, j) = values[i][j];
        }
    }
    data.counts = counts_matrix;

    qDebug() << "Parsed data file with " << data.genes.size()
             << " genes and " << data.spots.size() << " spots";
    // returns the data frame
    return data;
}

void STData::init(const QString &filename) {
    m_genes.clear();
    m_spots.clear();
    m_data = read(filename);

    // Filter out almost non-present spots (total count < 5)
    std::vector<uword> to_keep_spots;
    QList<Spot::SpotType> filtered_spots;
    for (uword i = 0; i < m_data.counts.n_rows; ++i) {
        if (accu(m_data.counts.row(i)) >= 5) {
            to_keep_spots.push_back(i);
            const auto spot = m_data.spots.at(i);
            filtered_spots.append(spot);
            m_spots.append(SpotObjectType(new Spot(spot)));
        }
    }
    m_data.spots = filtered_spots;
    m_data.counts = m_data.counts.rows(uvec(to_keep_spots));

    // Filter out almost non-present genes (total count < 5)
    std::vector<uword> to_keep_genes;
    QList<QString> filtered_genes;
    for (uword j = 0; j < m_data.counts.n_cols; ++j) {
        const uvec t = find(m_data.counts.col(j) > 0);
        if (t.n_elem >= 5) {
            to_keep_genes.push_back(j);
            const QString gene = m_data.genes.at(j);
            filtered_genes.append(gene);
            m_genes.append(GeneObjectType(new Gene(gene)));
        }
    }
    m_data.genes = filtered_genes;
    m_data.counts = m_data.counts.cols(uvec(to_keep_genes));

    // Compute these only when the dataset is created
    m_deseq_size_factors = RInterface::computeDESeqFactors(m_data.counts);
    m_scran_size_factors = RInterface::computeScranFactors(m_data.counts);
}

void STData::save(const QString &filename, const STData::STDataFrame &data)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        // write genes (1st row)
        for (const auto gene : data.genes) {
            stream << "\t" << gene;
        }
        stream << endl;
        // write spots (1st column and the rest of the rows (counts))
        for (uword i = 0; i < data.counts.n_rows; ++i) {
            const auto spot = data.spots[i];
            stream << QString::number(spot.first) + "x" + QString::number(spot.second);
            for (uword j = 0; j < data.counts.n_cols; ++j) {
                stream << "\t" << data.counts(i,j);
            }
            stream << endl;
        }

    }
}

STData::STDataFrame STData::data() const
{
    return m_data;
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
    Q_ASSERT(m_data.counts.size() > 0);
    m_rendering_colors.clear();
    m_rendering_selected.clear();
    m_rendering_spots.clear();

    const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;

    // Normalize the counts
    mat counts = normalizeCounts(m_data.counts,
                                    rendering_settings.normalization_mode,
                                    m_deseq_size_factors,
                                    m_scran_size_factors);

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
    for (uword i = 0; i < counts.n_rows; ++i) {;
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
            for (const uword j : selected_genes_indexes) {
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

        if (visible) {
            merged_color.setAlphaF(rendering_settings.intensity);
            m_rendering_colors.append(merged_color);
            m_rendering_selected.append(spot->selected());
            m_rendering_spots.append(spot->coordinates());
        }
    }
}

const QVector<Spot::SpotType> &STData::renderingSpots() const
{
    return m_rendering_spots;
}

const QVector<QColor> &STData::renderingColors() const
{
    return m_rendering_colors;
}

const QVector<bool> &STData::renderingSelected() const
{
    return m_rendering_selected;
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
    for (uword i = 0; i < m_data.counts.n_rows; ++i) {
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
                m_data.counts.shed_row(index);
            }
        }
    }

    return true;
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

mat STData::normalizeCounts(const mat &counts,
                                       SettingsWidget::NormalizationMode mode,
                                       const rowvec &deseq_factors,
                                       const rowvec &scran_factors)
{
    mat norm_counts = counts;
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

rowvec STData::computeNonZeroColumns(const mat &matrix)
{
    rowvec non_zeros(matrix.n_cols);
    for (uword i = 0; i < matrix.n_cols; ++i) {
        const uvec t = find(matrix.col(i) > 0);
        non_zeros[i] = t.n_elem;
    }
    return non_zeros;
}

colvec STData::computeNonZeroRows(const mat &matrix)
{
    colvec non_zeros(matrix.n_rows);
    for (uword i = 0; i < matrix.n_rows; ++i) {
        const uvec t = find(matrix.row(i) > 0);
        non_zeros[i] = t.n_elem;
    }
    return non_zeros;
}

void STData::clearSelection()
{
    for (auto spot : m_spots) {
        spot->selected(false);
    }

    for (auto gene : m_genes) {
        gene->selected(false);
    }
}

void STData::selectSpots(const SelectionEvent &event)
{
    const QPainterPath path = event.path();
    const auto mode = event.mode();

    if (mode == SelectionEvent::SelectionMode::NewSelection) {
        clearSelection();
    }

    // update selection
    const bool remove = (mode == SelectionEvent::SelectionMode::ExcludeSelection);
    for (auto spot : m_spots) {
        auto coord = spot->coordinates();
        spot->selected(!remove && path.contains(QPointF(coord.first, coord.second)));
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
    const auto mm_x = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().first < rhs->coordinates().first;});

    const auto mm_y = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().second < rhs->coordinates().second;});

    const auto min_x = (*mm_x.first)->coordinates().first;
    const auto min_y = (*mm_y.first)->coordinates().second;
    const auto max_x = (*mm_x.first)->coordinates().second;
    const auto max_y = (*mm_y.first)->coordinates().first;
    return QRectF(QPointF(min_x, min_y), QPointF(max_x, max_y));
}

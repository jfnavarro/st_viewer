#include "STData.h"
#include <QDebug>
#include <QMessageBox>
#include <QtConcurrent>
#include "math/Common.h"
#include "color/HeatMap.h"

#include <future>
#include <thread>
#include <variant>
#include <fstream>
#include <sstream>

constexpr int ROW = 1;
constexpr int COLUMN = 0;

namespace  {
inline QVector4D fromQtColor(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

inline QColor fromOpenGLColor(const QVector4D &opengl_color)
{
    return QColor::fromRgbF(opengl_color.x(), opengl_color.y(),
                            opengl_color.z(), opengl_color.w());
}
}


STData::STData()
    : m_is3D(false)
{

}

STData::~STData()
{

}

STData::STDataFrame STData::read(const QString &filename)
{
    // Open file
    std::ifstream f(filename.toStdString(), std::ios::in);
    qDebug() << "Opening ST Data file " << filename;

    // Parse the file
    STDataFrame data;
    std::vector<double> values;
    unsigned row_number = 0;
    unsigned col_number = 0;
    char sep = '\t';
    bool parsed = true;
    for (std::string line; std::getline(f, line);) {
        std::istringstream iss(line);
        std::string token;
        col_number = 0;
        while (std::getline(iss, token, sep)) {
            if (row_number == 0) {
                const QString gene = QString::fromStdString(token).trimmed();
                if (!gene.isNull() && !gene.isEmpty()) {
                    data.genes.append(gene);
                }
            } else if (col_number == 0) {
                const QString spot = QString::fromStdString(token).trimmed();
                if (!spot.isNull() && !spot.isEmpty()) {
                    data.spots.append(spot);
                }
            } else {
                values.push_back(std::stod(token));
            }
            ++col_number;
        }
        ++row_number;
    }
    f.close();

    if (!parsed || data.spots.empty() || data.genes.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    qDebug() << "Parsed data file with " << data.genes.size()
             << " genes and " << data.spots.size() << " spots";

    // Create data frame
    data.counts = mat(values.data(), data.spots.size(), data.genes.size());

    return data;
}

void STData::init(const QString &filename, const QString &spots_coordinates) {

    // First parse the matrix with counts
    try {
        m_data = read(filename);
    } catch (const std::exception &e) {
        throw;
    }

    // parse the spot coordinates file (if any)
    QMap<QString, QString> spots_dict;
    if (!spots_coordinates.isNull() && !spots_coordinates.isEmpty()) {
        try {
            spots_dict = parseSpotsMap(spots_coordinates);
        } catch (const std::exception &e) {
            throw;
        }
    }

    // The containers for the gene/spot objects
    m_genes.clear();
    m_spots.clear();

    // Create the spot object (if spot coordinates have been given only the spots
    // there will be added), compute the total sum of the spot to add it to the spot objects
    // and if the total sum == 0 the spot is discarded
    const colvec row_sum = sum(m_data.counts, ROW);
    std::vector<uword> to_keep_spots;
    QList<QString> spots;
    m_spot_index.clear();
    for (uword i = 0; i < m_data.counts.n_rows; ++i) {
        const auto &spot = m_data.spots.at(i);
        auto adj_spot = spot;
        if (!spots_dict.empty() && spots_dict.contains(spot)) {
            adj_spot = spots_dict[spot];
        } else if (!spots_dict.empty()) {
            continue;
        }
        const double row_sum_value = row_sum.at(i);
        if (row_sum_value > 0) {
            to_keep_spots.push_back(i);
            auto spot_obj = SpotObjectType(new Spot(spot));
            spot_obj->adj_coordinates(Spot::getCoordinates(adj_spot));
            spot_obj->totalCount(row_sum_value);
            m_spots.push_back(spot_obj);
            spots.push_back(spot);
            m_spot_index.insert(spot, m_spots.size() - 1);

            // update the rendering vectors
            m_rendering_coords.append(spot_obj->adj_coordinates());
            m_rendering_colors.append(QVector4D(1.0, 1.0, 1.0, 1.0));
            m_rendering_visible.append(0);
            m_rendering_selected.append(0);
        }
    }
    m_data.spots = spots;
    m_data.counts = m_data.counts.rows(uvec(to_keep_spots));

    if (m_spots.empty()) {
        qDebug() << "No valid spots could be found in the file.";
        throw std::runtime_error("No valid spots could be found in the file.");
    }

    // Create the gene object and compute the total sums to add them to the gene objects
    // if total sum is == 0 then the gene is discarded
    const rowvec col_sum = sum(m_data.counts, COLUMN);
    std::vector<uword> to_keep_genes;
    QList<QString> genes;
    m_gene_index.clear();
    for (uword j = 0; j < m_data.counts.n_cols; ++j) {
        const double col_sum_value = col_sum.at(j);
        if (col_sum_value > 0) {
            const auto &gene = m_data.genes.at(j);
            auto gene_obj = GeneObjectType(new Gene(gene));
            gene_obj->totalCount(col_sum_value);
            genes.push_back(gene);
            to_keep_genes.push_back(j);
            m_genes.push_back(gene_obj);
            m_gene_index.insert(gene, m_genes.size() - 1);
        }
    }
    m_data.genes = genes;
    m_data.counts = m_data.counts.cols(uvec(to_keep_genes));

    if (m_genes.empty()) {
        qDebug() << "No valid genes could be found in the file.";
        throw std::runtime_error("No valid genes could be found in the file.");
    }
}

void STData::save(const QString &filename, const STData::STDataFrame &data)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        // write genes (1st row)
        for (const auto &gene : data.genes) {
            stream << "\t" << gene;
        }
        stream << endl;
        // write spots (1st column and the rest of the rows (counts))
        for (uword i = 0; i < data.counts.n_rows; ++i) {
            const auto spot = data.spots.at(i);
            stream <<  spot;
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

const STData::GeneListType &STData::genes() const
{
    return m_genes;
}

const STData::SpotListType &STData::spots() const
{
    return m_spots;
}

void STData::computeRenderingData(SettingsWidget::Rendering &rendering_settings)
{
    const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog;
    const bool do_values = rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;

    // reset visible/selecteed to 0
    #pragma omp parallel for
    for (int i = 0; i < m_rendering_selected.size(); ++i) {
        m_rendering_visible[i] = 0;
        m_rendering_selected[i] = 0;
    }

    // apply filters
    STDataFrame data = m_data;
    if (rendering_settings.reads_threshold > 0) {
        data.counts.clean(rendering_settings.reads_threshold);
    }

    // get columns (genes) >= min_genes
    uvec cols_to_keep = linspace<uvec>(0, data.counts.n_cols - 1, data.counts.n_cols);
    if (rendering_settings.genes_threshold > 0) {
        const rowvec num_genes = sum(data.counts, COLUMN);
        cols_to_keep = find(num_genes >= rendering_settings.genes_threshold);
    }

    // get rows (spots) >= min_spots
    uvec rows_to_keep = linspace<uvec>(0, data.counts.n_rows - 1, data.counts.n_rows);
    if (rendering_settings.spots_threshold > 0) {
        const colvec num_spots = sum(data.counts, ROW);
        rows_to_keep = find(num_spots >= rendering_settings.spots_threshold);
    }

    // early out if no genes or no spots after filtering
    if (rows_to_keep.empty() || cols_to_keep.empty()) {
        return;
    }

    // slice
    data.counts = data.counts.submat(rows_to_keep, cols_to_keep);

    // normalize
    data = normalizeCounts(data, rendering_settings.normalization_mode);

    // get genes that are visible
    std::vector<int> genes_visible_indexes;
    #pragma omp parallel for
    for (int j = 0; j < cols_to_keep.size(); ++j) {
        if (m_genes.at(j)->visible()) {
            genes_visible_indexes.push_back(j);
        }
    }

    // early out if no genes are visible in show genes mode
    if (genes_visible_indexes.empty() && !rendering_settings.show_spots) {
        return;
    }

    int num_genes;
    QColor merged_color;
    int visible;
    vec values;
    if (do_values) {
        values = use_genes ? conv_to<vec>::from(sum(data.counts > 0, ROW)) : sum(data.counts, ROW);
        if (use_log) {
            values = log(values);
        }
        rendering_settings.legend_min = values.min();
        rendering_settings.legend_max = values.max();
    }
    #pragma omp parallel for collapse(2)
    for (const auto i : rows_to_keep) {
        const auto spot_obj = m_spots.at(i);
        visible = 0;
        num_genes = 0;
        merged_color = Qt::white;
        if (rendering_settings.show_spots) {
            merged_color = spot_obj->color();
            visible = spot_obj->visible();
        } else {
            if (do_values) {
                const double value = values.at(i);
                if (value > 0) {
                    merged_color = Color::adjustVisualMode(
                                merged_color,
                                value,
                                rendering_settings.legend_min,
                                rendering_settings.legend_max,
                                rendering_settings.visual_mode);
                    visible = 1;
                }
            } else {
                for (int j = 0; j < genes_visible_indexes.size(); ++j) {
                    const auto gene_obj = m_genes.at(genes_visible_indexes.at(j));
                    if (data.counts.at(i,j) > 0 && gene_obj->color() != merged_color) {
                        merged_color = STMath::lerp(1.0 / ++num_genes, merged_color, gene_obj->color());
                        visible = 1;
                    }
                }
            }
        }
        m_rendering_selected[i] = visible && spot_obj->selected();
        m_rendering_colors[i] = fromQtColor(merged_color);
        m_rendering_visible[i] = visible;
    }
}

const QVector<int> &STData::renderingVisible() const
{
    return m_rendering_visible;
}

const QVector<QVector4D> &STData::renderingColors() const
{
    return m_rendering_colors;
}

const QVector<int> &STData::renderingSelected() const
{
    return m_rendering_selected;
}

const QVector<Spot::SpotType> &STData::renderingCoords() const
{
    return m_rendering_coords;
}

QMap<QString, QString> STData::parseSpotsMap(const QString &spots_file)
{
    qDebug() << "Parsing spots file " << spots_file;
    QMap<QString, QString> spotMap;
    QFile file(spots_file);
    // Parse the spots map = old_spot -> new_spot
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        bool parsed = true;
        while (!in.atEnd()) {
            line = in.readLine();
            if (!line.startsWith("x")) {
                fields = line.split("\t");
                QString old_spot;
                QString new_spot;
                const int n_fields = fields.length();
                if (n_fields == 4) {
                    // 3D format
                    old_spot = fields.at(0);
                    new_spot = fields.at(1) + "x" + fields.at(2) + "x" + fields.at(3);
                } else if (n_fields == 6) {
                    // 2D format (ST Spot detector)
                    old_spot = fields.at(0) + "x" + fields.at(1);
                    new_spot = fields.at(2) + "x" + fields.at(3);
                } else {
                    parsed = false;
                    break;
                }
                spotMap.insert(old_spot, new_spot);
            }
        }
        if (spotMap.empty() || !parsed) {
            qDebug() << "No valid spots were found in the spots file";
            file.close();
            throw std::runtime_error("No valid spots found in the spot coordinates file");
        }
    } else {
        qDebug() << "Could not open spots file";
        file.close();
        throw std::runtime_error("Coult not open/parse the spot coordinates file");
    }
    file.close();

    return spotMap;
}

STData::STDataFrame STData::normalizeCounts(const STDataFrame &data,
                                            SettingsWidget::NormalizationMode mode)
{
    STDataFrame norm_counts = data;
    if (mode == SettingsWidget::REL) {
        norm_counts.counts.each_col() /= sum(norm_counts.counts, ROW);
    } else if (mode == SettingsWidget::CPM) {
        const auto means = mean(norm_counts.counts, ROW);
        norm_counts.counts.each_col() /= sum(norm_counts.counts, ROW) % means;
    }
    return data;
}

urowvec STData::computeNonZeroColumns(const mat &matrix, const int min_value)
{
    return sum(matrix > min_value, COLUMN);
}

ucolvec STData::computeNonZeroRows(const mat &matrix, const int min_value)
{
    return sum(matrix > min_value, ROW);
}

STData::STDataFrame STData::filterCounts(const STDataFrame &data,
                                         const int min_reads,
                                         const int min_genes,
                                         const int min_spots)
{
    if (data.counts.n_cols == 0 || data.counts.n_rows == 0
            || (min_reads == 0 && min_genes == 0 && min_spots == 0)) {
        return data;
    }

    // temp copy
    mat M = data.counts;
    M.clean(min_reads);

    // get columns (genes) >= min_genes
    const rowvec num_genes = sum(M, COLUMN);
    const uvec cols_to_keep = find(num_genes >= min_genes);
    // get rows (spots) >= min_spots
    const colvec num_spots = sum(M, ROW);
    const uvec rows_to_keep = find(num_spots >= min_spots);
    // create filtered_matrix
    if (rows_to_keep.size() != data.counts.n_rows
            || cols_to_keep.size() != data.counts.n_cols) {
        STDataFrame sliced_data;
        sliced_data.counts = data.counts.submat(rows_to_keep, cols_to_keep);

        #pragma omp parallel for
        for (const auto &i : cols_to_keep) {
            sliced_data.genes.append(data.genes.at(i));
        }

        #pragma omp parallel for
        for (const auto &j : rows_to_keep) {
            sliced_data.spots.append(data.spots.at(j));
        }

        return sliced_data;
    } else {
        return data;
    }
}

const STData::STDataFrame STData::sliceDataSpots(const QList<QString> &spots)
{
    STDataFrame sliced_data;
    std::vector<int> to_keep_rows;

    #pragma omp parallel for
    for (const auto &spot : spots) {
        const int index = m_spot_index.value(spot, -1);
        if (index != -1) {
            to_keep_rows.push_back(index);
            sliced_data.spots.append(spot);
        }
    }
    sliced_data.genes = m_data.genes;
    sliced_data.counts = m_data.counts.rows(conv_to<uvec>::from(to_keep_rows));

    // Return the sliced data frame
    return sliced_data;
}

const STData::STDataFrame STData::sliceDataGenes(const QList<QString> &genes)
{
    STDataFrame sliced_data;
    std::vector<int> to_keep_cols;

    #pragma omp parallel for
    for (const auto &gene : genes) {
        const int index = m_gene_index.value(gene, -1);
        if (index != -1) {
            to_keep_cols.push_back(index);
            sliced_data.genes.append(gene);
        }
    }
    sliced_data.spots = m_data.spots;
    sliced_data.counts = m_data.counts.cols(conv_to<uvec>::from(to_keep_cols));

    // Return the sliced data frame
    return sliced_data;
}


STData::STDataFrame STData::aggregate(const QList<STDataFrame> &dataframes)
{
    if (dataframes.empty()) {
        qDebug() << "Trying to merge a list of empty data frames";
        return STData::STDataFrame();
    } else if (dataframes.size() == 1) {
        return dataframes.first();
    }

    // First
    QSet<QString> merged_genes;
    QList<QString> merged_spots;
    #pragma omp parallel for
    for (int i = 0; i < dataframes.size(); ++i) {
        const auto data = dataframes.at(i);
        merged_genes += data.genes.toSet();
        QList<QString> adj_spots;
        std::transform(data.spots.begin(), data.spots.end(), std::back_inserter(adj_spots),
                       [=](const auto &spot) { return QString::number(i) + "_" + spot; });
        merged_spots += adj_spots;
    }

    STDataFrame merged;
    merged.genes = merged_genes.toList();
    merged.spots = merged_spots;
    const int n_rows = merged_spots.size();
    const int n_cols = merged_genes.size();
    merged.counts = mat(n_rows, n_cols);
    merged.counts.fill(0.0);

    unsigned spot_counter = 0;
    #pragma omp parallel for collapse(3)
    for (const auto &data : dataframes) {
        std::vector<uword> gene_indexes;
        for (uword i = 0; i < data.counts.n_rows; ++i) {
            for (uword j = 0; j < n_cols; ++j) {
                const auto &gene = merged.genes.at(j);
                int index;
                if (i == 0) {
                    index = data.genes.indexOf(gene);
                    gene_indexes.push_back(index);
                } else {
                    index = gene_indexes.at(j);
                }
                if (index != -1) {
                    merged.counts.at(spot_counter, j) = data.counts(i, index);
                }
            }
            ++spot_counter;
        }
    }

    return merged;
}

void STData::clearSelection()
{
    QtConcurrent::blockingMap(m_spots, [] (auto spot) { spot->selected(false); });
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
    QtConcurrent::blockingMap(m_spots, [&](auto spot) {
        const auto &coord = spot->coordinates();
        if (path.contains(QPointF(coord.x(), coord.y()))) {
            spot->selected(!remove);
        }
    });
}

void STData::selectSpots(const QList<QString> &spots)
{
    clearSelection();
    #pragma omp parallel for
    for (const auto &spot : spots) {
        const int spot_index = m_spot_index.value(spot, -1);
        if (spot_index != -1) {
            m_spots.at(spot_index)->selected(true);
        }
    }
}

void STData::selectSpots(const QList<int> &spots_indexes)
{
    clearSelection();
    #pragma omp parallel for
    for (const auto index : spots_indexes) {
        if (index >= 0 && index < m_spots.size()) {
            m_spots.at(index)->selected(true);
        }
    }
}

void STData::loadSpotColors(const QHash<QString, QColor> &colors)
{
    QHash<QString, QColor>::const_iterator it = colors.constBegin();
    while (it != colors.constEnd()) {
        const auto &spot = it.key();
        const QColor color = it.value();
        const int spot_index = m_spot_index.value(spot, -1);
        if (spot_index != -1) {
            m_spots.at(spot_index)->color(color);
            m_spots.at(spot_index)->visible(true);
        }
        ++it;
    }
}

void STData::loadGeneColors(const QHash<QString, QColor> &colors)
{
    QHash<QString, QColor>::const_iterator it = colors.constBegin();
    while (it != colors.constEnd()) {
        const auto &gene = it.key();
        const QColor color = it.value();
        const int gene_index = m_gene_index.value(gene);
        if (gene_index != -1) {
            m_genes.at(gene_index)->color(color);
            m_genes.at(gene_index)->visible(true);
        }
        ++it;
    }
}

const QRectF STData::getBorder() const
{
    const auto mm_x = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().x() < rhs->coordinates().x();});

    const auto mm_y = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().y() < rhs->coordinates().y();});

    const auto min_x = (*mm_x.first)->coordinates().x();
    const auto min_y = (*mm_y.first)->coordinates().y();
    const auto max_x = (*mm_x.second)->coordinates().x();
    const auto max_y = (*mm_y.second)->coordinates().y();
    return QRectF(QPointF(min_x, min_y), QPointF(max_x, max_y));
}


bool STData::is3D() const
{
    return m_is3D;
}

void STData::is3D(bool is3D)
{
    m_is3D = is3D;
}

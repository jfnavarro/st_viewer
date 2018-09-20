#include "STData.h"
#include <QDebug>
#include <QMessageBox>
#include <QtConcurrent>
#include "math/Common.h"
#include "color/HeatMap.h"
#include "math/RInterface.h"

static const int ROW = 1;
static const int COLUMN = 0;

static QVector4D fromQtColor(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

static QColor fromOpenGLColor(const QVector4D &opengl_color)
{
    return QColor::fromRgbF(opengl_color.x(), opengl_color.y(),
                            opengl_color.z(), opengl_color.w());
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
    STDataFrame data;
    std::vector<std::vector<double>> values;

    // Open file
    std::ifstream f(filename.toStdString());
    qDebug() << "Opening ST Data file " << filename;

    // Process the rest of the lines (row names and counts)
    unsigned row_number = 0;
    unsigned col_number = 0;
    char sep = '\t';
    bool parsed = true;
    for (std::string line; std::getline(f, line);) {
        std::istringstream iss(line);
        std::string token;
        std::vector<double> values_row;
        col_number = 0;
        while(std::getline(iss, token, sep)) {
            if (row_number == 0) {
                const QString gene = QString::fromStdString(token).trimmed();
                if (data.genes.contains(gene)) {
                    f.close();
                    throw std::runtime_error("The matrix contains duplicated genes!");
                }
                if (!gene.isEmpty() && !gene.isNull()) {
                    data.genes.append(gene);
                }
            } else if (col_number == 0) {
                const QString spot = QString::fromStdString(token).trimmed();
                data.spots.append(spot);
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
    f.close();

    if (!parsed || data.spots.empty() || data.genes.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    // Create an armadillo matrix
    mat counts_matrix(row_number - 1, col_number - 1);
    for (unsigned i = 0; i < row_number - 1; ++i) {
        for (unsigned j = 0; j < col_number - 1; ++j) {
            counts_matrix.at(i, j) = values[i][j];
        }

    }
    data.counts = counts_matrix;

    qDebug() << "Parsed data file with " << data.genes.size()
             << " genes and " << data.spots.size() << " spots";

    // returns the data frame
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

    // The containers for the gene/spot objects and the rendering data
    m_genes.clear();
    m_spots.clear();
    m_rendering_selected.clear();
    m_rendering_visible.clear();
    m_rendering_colors.clear();
    m_rendering_coords.clear();

    // Create the spot object (if spot coordinates have been given only the spots
    // there will be added), compute the total sum of the spot to add it to the spot objects
    // and if the total sum == 0 the spot is discarded
    colvec row_sum = sum(m_data.counts, ROW);
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
        // add the spot only if it has any count
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
            m_rendering_visible.append(false);
            m_rendering_selected.append(false);
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
    rowvec col_sum = sum(m_data.counts, COLUMN);
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
    Q_ASSERT(m_data.counts.size() > 0);

    const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog;
    const bool do_color =
            rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange ||
            rendering_settings.visual_mode == SettingsWidget::VisualMode::Normal;
    const bool do_values = rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;

    // Create copy of the data frame so to reduce and normalize it
    STDataFrame data = m_data;

    // Remove genes that are not visible or do not pass threshold
    const urowvec spot_counts = computeNonZeroColumns(data.counts,
                                                      rendering_settings.ind_reads_threshold);
    std::vector<uword> to_keep_genes;
    for (uword j = 0; j < data.counts.n_cols; ++j) {
        const auto gene_obj = m_genes.at(j);
        if (gene_obj->visible() && spot_counts.at(j) >= rendering_settings.spots_threshold) {
            to_keep_genes.push_back(j);
        }
    }
    data.counts = data.counts.cols(uvec(to_keep_genes));

    double min_value = 10e6;
    double max_value = -10e6;
    bool visible;
    double merged_value;
    double num_genes;
    bool any_gene_selected;
    const bool show_spots = rendering_settings.show_spots;
    QColor merged_color;
    QVector<double> rendering_values(data.counts.n_rows);
    const ucolvec gene_counts = computeNonZeroRows(data.counts,
                                                   rendering_settings.ind_reads_threshold);
    //TODO make this paralell
    for (uword i = 0; i < data.counts.n_rows; ++i) {
        const auto spot_obj = m_spots.at(i);
        visible = false;
        merged_value = 0.0;
        num_genes = 0.0;
        any_gene_selected = false;
        merged_color = Qt::white;
        // check first if user wants to show the color of the spot
        if (show_spots && spot_obj->visible()) {
            merged_color = spot_obj->color();
            visible = true;
        } else if (!show_spots) {
            // Iterate the genes in the spot to compute the sum of values and color
            for (uword j = 0; j < data.counts.n_cols; ++j) {
                const int gene_index = m_gene_index.value(data.genes.at(j));
                const auto gene_obj = m_genes.at(gene_index);
                const double value = data.counts.at(i,j);
                if (rendering_settings.gene_cutoff && gene_obj->cut_off() >= value) {
                    continue;
                }
                ++num_genes;
                merged_value += value;
                if (do_color) {
                    merged_color = STMath::lerp(1.0 / num_genes, merged_color, gene_obj->color());
                }
                any_gene_selected |= gene_obj->selected();
            }
            if (merged_value >= rendering_settings.reads_threshold
                    && num_genes >= rendering_settings.genes_threshold) {
                // Use number of genes or total reads in the spot depending on settings
                if (do_values) {
                    merged_value = use_genes ? num_genes : merged_value;
                    merged_value = use_log ? std::log(merged_value) : merged_value;
                    min_value = std::min(min_value, merged_value);
                    max_value = std::max(max_value, merged_value);
                }
                visible = true;
            }
        }
        spot_obj->selected(visible && (spot_obj->selected() || any_gene_selected));
        m_rendering_colors[i] = fromQtColor(merged_color);
        m_rendering_selected[i] = spot_obj->selected();
        rendering_values[i] = merged_value;
        m_rendering_visible[i] = visible;
    }

    if (!show_spots) {
        rendering_settings.legend_min = min_value;
        rendering_settings.legend_max = max_value;
        if (do_values){
            for (int i = 0; i < rendering_values.size(); ++i) {
                const double value = rendering_values[i];
                if (value > 0 && m_rendering_visible[i]) {
                    m_rendering_colors[i] = fromQtColor(Color::adjustVisualMode(
                                                            fromOpenGLColor(m_rendering_colors[i]),
                                                            value,
                                                            min_value,
                                                            max_value,
                                                            rendering_settings.visual_mode));
                }
            }
        }
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
            if (!line.contains("x")) {
                fields = line.split("\t");
                if (fields.length() != 4 && fields.length() != 6) {
                    parsed = false;
                    break;
                }
                spotMap.insert(fields.at(0) + "x" + fields.at(1),
                               fields.at(2) + "x" + fields.at(3));
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
    switch (mode) {
    case (SettingsWidget::NormalizationMode::RAW): {
    } break;
    case (SettingsWidget::NormalizationMode::REL): {
        norm_counts.counts.each_col() /= sum(norm_counts.counts, ROW);
    } break;
    case (SettingsWidget::NormalizationMode::TPM): {
        norm_counts.counts.each_col() /= sum(norm_counts.counts, ROW);
        norm_counts.counts *= 1e6;
    } break;
    case (SettingsWidget::NormalizationMode::DESEQ): {
        const auto m_deseq_size_factors = RInterface::computeDESeqFactors(data.counts);
        norm_counts.counts.each_col() /= m_deseq_size_factors.t();
    } break;
    case (SettingsWidget::NormalizationMode::SCRAN): {
        const auto scran_size_factors = RInterface::computeScranFactors(data.counts, true);
        norm_counts.counts.each_col() /= scran_size_factors.t();
    } break;
    }
    return norm_counts;
}

STData::STDataFrame STData::sliceDataFrameSpots(const STDataFrame &data,
                                                const QList<QString> &spots)
{
    STDataFrame sliced_data = data;

    // Keep only the spots given in the list
    sliced_data.spots.clear();
    uvec to_keep_rows(spots.size());
    for (uword i = 0; i < spots.size(); ++i) {
        const auto &spot = spots.at(i);
        const int spot_index = data.spots.indexOf(spot);
        if (spot_index != -1) {
            to_keep_rows.at(i) = spot_index;
            sliced_data.spots.push_back(spot);
        }
    }
    sliced_data.counts = sliced_data.counts.rows(to_keep_rows);

    // Remove non present genes (total count == 0 after removing spots)
    rowvec gene_counts = sum(sliced_data.counts, COLUMN);
    std::vector<uword> to_keep_cols;
    QList<QString> new_genes;
    for (uword j = 0; j < sliced_data.counts.n_cols; ++j) {
        if (gene_counts.at(j) > 0) {
            const auto &gene = sliced_data.genes.at(j);
            to_keep_cols.push_back(j);
            new_genes.push_back(gene);
        }
    }
    sliced_data.genes = new_genes;
    sliced_data.counts = sliced_data.counts.cols(uvec(to_keep_cols));

    // Return the sliced data frame
    return sliced_data;
}

STData::STDataFrame STData::sliceDataFrameGenes(const STDataFrame &data,
                                                const QList<QString> &genes)
{
    STDataFrame sliced_data = data;

    // Keep only the genes given in the list
    sliced_data.genes.clear();
    uvec to_keep_cols(genes.size());
    for (uword j = 0; j < genes.size(); ++j) {
        const auto &gene = genes.at(j);
        const int gene_index = data.genes.indexOf(gene);
        if (gene_index != -1) {
            to_keep_cols.at(j) = gene_index;
            sliced_data.genes.push_back(gene);
        }
    }
    sliced_data.counts = data.counts.cols(to_keep_cols);

    // Remove non present spots (total count == 0 after removing genes)
    colvec spot_counts = sum(sliced_data.counts, ROW);
    std::vector<uword> to_keep_rows;
    QList<QString> new_spots;
    for (uword i = 0; i < sliced_data.counts.n_rows; ++i) {
        if (spot_counts.at(i) > 0) {
            const auto &spot = sliced_data.spots.at(i);
            to_keep_rows.push_back(i);
            new_spots.push_back(spot);
        }
    }
    sliced_data.spots = new_spots;
    sliced_data.counts = sliced_data.counts.rows(uvec(to_keep_rows));

    // Return the sliced data frame
    return sliced_data;
}

STData::STDataFrame STData::filterDataFrame(const STDataFrame &data,
                                            const int min_exp_value,
                                            const int min_reads_spot,
                                            const int min_genes_spot,
                                            const int min_spots_gene)
{
    if (data.counts.n_cols == 0 || data.counts.n_rows == 0) {
        return data;
    }

    STDataFrame sliced_data = data;

    // Filter out genes
    const urowvec spot_counts = computeNonZeroColumns(sliced_data.counts, min_exp_value);
    std::vector<uword> to_keep_genes;
    QList<QString> new_genes;
    for (uword j = 0; j < sliced_data.counts.n_cols; ++j) {
        if (spot_counts.at(j) > min_spots_gene) {
            const auto &gene = sliced_data.genes.at(j);
            to_keep_genes.push_back(j);
            new_genes.push_back(gene);
        }
    }
    sliced_data.genes = new_genes;
    sliced_data.counts = sliced_data.counts.cols(uvec(to_keep_genes));

    // Filter out spots
    const ucolvec gene_counts = computeNonZeroRows(sliced_data.counts, min_exp_value);
    std::vector<uword> to_keep_spots;
    QList<QString> new_spots;
    for (uword i = 0; i < sliced_data.counts.n_rows; ++i) {
        const rowvec row = sliced_data.counts.row(i);
        const double row_sum = sum(row.elem(find(row > min_exp_value)));
        if (row_sum > min_reads_spot && gene_counts.at(i) > min_genes_spot) {
            const auto &spot = sliced_data.spots.at(i);
            to_keep_spots.push_back(i);
            new_spots.push_back(spot);
        }
    }
    sliced_data.spots = new_spots;
    sliced_data.counts = sliced_data.counts.rows(uvec(to_keep_spots));

    // Return the filtered data
    return sliced_data;
}

STData::STDataFrame STData::aggregate(const QList<STDataFrame> &datasets)
{
    if (datasets.empty()) {
        qDebug() << "Trying to merge a list of empty data frames";
        return STData::STDataFrame();
    } else if (datasets.size() == 1) {
        return datasets.first();
    }

    QSet<QString> merged_genes;
    QList<QString> merged_spots;
    for (unsigned i = 0; i < datasets.size(); ++i) {
        const auto data = datasets.at(i);
        merged_genes += data.genes.toSet();
        QList<QString> adj_spots;
        std::transform(data.spots.begin(), data.spots.end(), std::back_inserter(adj_spots),
                       [=](auto spot) { return QString::number(i) + "_" + spot; });
        merged_spots += adj_spots;
        //merged_spots += QtConcurrent::blockingMapped<QList<QString> >(
        //            data.spots, [=] (auto spot) { return QString::number(i) + "_" + spot; });
    }

    STDataFrame merged;
    merged.genes = merged_genes.toList();
    merged.spots = merged_spots;
    const unsigned n_rows = merged_spots.size();
    const unsigned n_cols = merged_genes.size();
    merged.counts = mat(n_rows, n_cols);
    merged.counts.fill(0.0);

    unsigned spot_counter = 0;
    for (unsigned d = 0; d < datasets.size(); ++d) {
        const auto data = datasets.at(d);
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

urowvec STData::computeNonZeroColumns(const mat &matrix, const int min_value)
{
    return sum(matrix > min_value, COLUMN);
}

ucolvec STData::computeNonZeroRows(const mat &matrix, const int min_value)
{
    return sum(matrix > min_value, ROW);
}

void STData::clearSelection()
{
    QtConcurrent::blockingMap(m_spots, [] (auto spot) { spot->selected(false); });
    QtConcurrent::blockingMap(m_genes, [] (auto gene) { gene->selected(false); });
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
        const auto &coord = spot->coordinates();
        if (path.contains(QPointF(coord.x(), coord.y()))) {
            spot->selected(!remove);
        }
    }
}

void STData::selectSpots(const QList<QString> &spots)
{
    clearSelection();
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
    for (const auto index : spots_indexes) {
        if (index > 0 and index < m_spots.size()) {
            m_spots.at(index)->selected(true);
        }
    }
}

void STData::selectGenes(const QRegExp &regexp, const bool force)
{
    clearSelection();
    for (auto gene : m_genes) {
        const bool selected = regexp.exactMatch(gene->name());
        gene->selected(selected);
        gene->visible(gene->visible() || (force && selected));
    }
}

void STData::selectGenes(const QList<QString> &genes)
{
    clearSelection();
    for (const auto &gene : genes) {
        const int gene_index = m_gene_index.value(gene, -1);
        if (gene_index != -1) {
            m_genes.at(gene_index)->selected(true);
            m_genes.at(gene_index)->visible(true);
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

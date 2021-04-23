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
#include <omp.h>

constexpr int ROW = 1;
constexpr int COLUMN = 0;

namespace  {
inline QVector4D fromQtColor(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
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
    // open file
    std::ifstream f(filename.toStdString(), std::ios::in);
    qDebug() << "Opening ST Data file " << filename;

    // parse the file and extra spots/genes and values
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

    const uword n_spots = data.spots.size();
    const uword n_genes = data.genes.size();
    qDebug() << "Parsed data file with " << n_genes << " genes and " << n_spots << " spots";
    // TODO add a try-catch here
    data.counts = mat(values.data(), n_genes, n_spots).t();
    return data;
}

void STData::init(const QString &filename, const QString &spots_coordinates) {

    // first parse the matrix with counts
    try {
        m_data = read(filename);
    } catch (const std::exception &e) {
        qDebug() << "Error parsing the matrix of counts " << e.what();
        throw;
    }

    // parse the spot coordinates file
    QMap<QString, Spot::SpotType> spots_dict;
    try {
        spots_dict = parseSpotsMap(spots_coordinates);
    } catch (const std::exception &e) {
        qDebug() << "Error parsing the spots file " << e.what();
        throw;
    }

    // create the spot objects (only spots in the spots coordiantes file will be included)
    // compute the total sum of the spot to add it to the spot objects
    // and if the total sum == 0 the spot is discarded
    // also initialize the rendering data
    std::vector<uword> to_keep_spots;
    m_spots.clear();
    m_spot_index.clear();
    QFuture<void> future1 = QtConcurrent::run([&]() {
        const auto old_spots = m_data.spots;
        m_data.spots.clear();
        const colvec row_sum = arma::sum(m_data.counts, ROW);
        int index_spot = 0;
        for (uword i = 0; i < m_data.counts.n_rows; ++i) {
            const auto &spot = old_spots.at(i);
            if (spots_dict.contains(spot)) {
                const auto adj_spot = spots_dict[spot];
                const double row_sum_value = row_sum.at(i);
                if (row_sum_value > 0) {
                    to_keep_spots.push_back(i);
                    auto spot_obj = SpotObjectType(new Spot(spot));
                    spot_obj->adj_coordinates(adj_spot);
                    spot_obj->totalCount(row_sum_value);
                    m_spots.push_back(spot_obj);
                    m_data.spots.push_back(spot);
                    m_spot_index.insert(spot, index_spot++);
                    m_rendering_coords.append(spot_obj->adj_coordinates());
                    m_rendering_colors.append(QVector4D(1.0, 1.0, 1.0, 1.0));
                    m_rendering_visible.append(0);
                    m_rendering_selected.append(0);
                }
            }
        }
    });

    // create the gene objects
    // compute the total sum of the gene to add it to the gene objects
    // and if the total sum == 0 the gene is discarded
    std::vector<uword> to_keep_genes;
    m_gene_index.clear();
    m_genes.clear();
    QFuture<void> future2 = QtConcurrent::run([&]() {
        const auto old_genes = m_data.genes;
        m_data.genes.clear();
        const rowvec col_sum = sum(m_data.counts, COLUMN);
        int index_gene = 0;
        for (uword j = 0; j < m_data.counts.n_cols; ++j) {
            const double col_sum_value = col_sum.at(j);
            if (col_sum_value > 0) {
                const auto &gene = old_genes.at(j);
                auto gene_obj = GeneObjectType(new Gene(gene));
                gene_obj->totalCount(col_sum_value);
                m_data.genes.push_back(gene);
                to_keep_genes.push_back(j);
                m_genes.push_back(gene_obj);
                m_gene_index.insert(gene, index_gene++);
            }
        }
    });

    future1.waitForFinished();
    future2.waitForFinished();

    if (m_spots.empty() || m_genes.empty()) {
        qDebug() << "No valid spots or genes could be found in the file.";
        throw std::runtime_error("No valid spots or genes could be found in the file.");
    }

    // slice data
    m_data.counts = m_data.counts.submat(uvec(to_keep_spots),
                                         uvec(to_keep_genes));

    qDebug() << "Spots and genes present " << m_spots.size() << " " << m_genes.size();
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
    const bool do_values = rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;
    const bool drange = rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange;

    STDataFrame data = m_data;
    uvec cols_to_keep;
    uvec rows_to_keep;
    vec values;
    if (!rendering_settings.show_spots) {
        // to store the genes that pass the filters
        cols_to_keep = ones<uvec>(data.counts.n_cols - 1);
        // to store the spots that are visible and pass the filters
        rows_to_keep = zeros<uvec>(data.counts.n_rows - 1);
        // to store the visible genes
        uvec cols_to_keep_tmp = zeros<uvec>(data.counts.n_cols - 1);

        #pragma omp parallel for
        for (uword i = 0; i < data.counts.n_rows; ++i) {
            // reset visible/selected arrays to false
            m_rendering_visible[i] = false;
            m_rendering_selected[i] = false;
            if (m_spots.at(i)->visible()) {
                // get rows (spots) >= min_genes
                if (rendering_settings.genes_threshold > 0) {
                    const auto num_genes = sum(data.counts.row(i) >= rendering_settings.reads_threshold);
                    rows_to_keep.at(i) = num_genes >= rendering_settings.genes_threshold ? 1 : 0;
                } else {
                    rows_to_keep.at(i) = 1;
                }
            }
        }

        // early out if no no spots after filtering
        // rows_to_keep.is_zero() this fails
        if (!any(rows_to_keep)) {
            return;
        }

        // reset visible/selected to false
        #pragma omp parallel for
        for (uword j = 0; j < data.counts.n_cols; ++j) {
            // get columns (genes) >= min_spots
            if (rendering_settings.spots_threshold > 0) {
                const auto num_spots = sum(data.counts.col(j) > rendering_settings.reads_threshold);
                cols_to_keep.at(j) = num_spots >= rendering_settings.spots_threshold ? 1 : 0;
            }
            cols_to_keep_tmp.at(j) = m_genes.at(j)->visible();
        }

        // early out if no genes after filtering
        // cols_to_keep.is_zero() this fails
        if (!any(cols_to_keep) || !any(cols_to_keep_tmp)) {
            return;
        }

        // update genes to be kept with the genes filtered (element wise multiplcation)
        cols_to_keep_tmp = cols_to_keep_tmp % cols_to_keep;

        // get indeces of visible rows/gens
        rows_to_keep = find(rows_to_keep);
        cols_to_keep = find(cols_to_keep);

        // slice
        data.counts = data.counts.submat(rows_to_keep, cols_to_keep);

        if (do_values) {
            // normalize
            data = normalizeCounts(data, rendering_settings.normalization_mode);
        }

        // keep only visible genes
        cols_to_keep = find(cols_to_keep_tmp);
        data.counts = data.counts.cols(cols_to_keep);

        if (do_values) {
            // log scale
            if (rendering_settings.log_scale) {
                data.counts = log1p(data.counts);
            }

            // apply standard transformation if requested (by genes)
            if (rendering_settings.zscore) {
                data = ztransform(data);
            }

            // compute total sum (per spot)
            values = sum(data.counts, ROW);
            rendering_settings.legend_min = values.min();
            rendering_settings.legend_max = values.max();
        }

    } else {
        cols_to_keep = linspace<uvec>(0, data.counts.n_cols - 1, data.counts.n_cols);
        rows_to_keep = linspace<uvec>(0, data.counts.n_rows - 1, data.counts.n_rows);
    }

    // iterate spots to assign color, selected and visible status to each of them
    // and also update the rendering vectors so the data can be visualized
    #pragma omp parallel for
    for (uword i = 0; i < data.counts.n_rows; ++i) {
        const auto spot_index = rows_to_keep.at(i);
        const auto spot_obj = m_spots.at(spot_index);
        int visible = false;
        int num_genes = 0;
        double value = 0;
        QColor merged_color = Qt::white;
        if (rendering_settings.show_spots) {
            merged_color = spot_obj->color();
            visible = spot_obj->visible();
        } else {
            if (!do_values || drange) {
                // iterate only genes with expression in the spot
                const rowvec row_vec = data.counts.row(i);
                const uvec detected_genes = find(row_vec);
                for (const auto &j : detected_genes) {
                    const auto gene_obj = m_genes.at(cols_to_keep.at(j));
                    if (row_vec.at(j) > 0 && gene_obj->color() != merged_color) {
                        // merge colors (genes) in the same spot using linear interpolation
                        merged_color = STMath::lerp(1.0 / ++num_genes, merged_color, gene_obj->color());
                    }
                }
            } else if (do_values) {
                value = values.at(i);
                merged_color = Color::adjustVisualMode(
                            merged_color,
                            value,
                            rendering_settings.legend_min,
                            rendering_settings.legend_max,
                            rendering_settings.visual_mode);
            }
            visible = do_values || num_genes > 0;
        }
        m_rendering_selected[spot_index] = visible && spot_obj->selected();
        m_rendering_colors[spot_index] = fromQtColor(merged_color);
        m_rendering_visible[spot_index] = visible;
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

QMap<QString, Spot::SpotType> STData::parseSpotsMap(const QString &spots_file) const
{
    qDebug() << "Parsing spots file " << spots_file;

    QMap<QString, Spot::SpotType> spotMap;
    QFile file(spots_file);
    // Parse the spots map = old_spot -> pixel coordinates
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
                Spot::SpotType new_spot;
                const int n_fields = fields.length();
                if (n_fields == 3) {
                    // 2D format
                    old_spot = fields.at(0);
                    new_spot = Spot::SpotType(fields.at(1).toFloat(),
                                              fields.at(2).toFloat(),
                                              0);
                } else if (n_fields == 4) {
                    // 3D format
                    old_spot = fields.at(0);
                    new_spot = Spot::SpotType(fields.at(1).toFloat(),
                                              fields.at(2).toFloat(),
                                              fields.at(3).toFloat());
                } else if (n_fields == 5) {
                    // 2D format
                    old_spot = fields.at(0);
                    new_spot = Spot::SpotType(fields.at(3).toFloat(),
                                              fields.at(4).toFloat(),
                                              0);
                } else if (n_fields == 6) {
                    // 2D format (ST Spot detector)
                    old_spot = fields.at(0) + "x" + fields.at(1);
                    new_spot = Spot::SpotType(fields.at(4).toFloat(),
                                              fields.at(5).toFloat(),
                                              0);
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
        const auto sums = sum(norm_counts.counts, ROW);
        const auto means = mean(sums);
        norm_counts.counts = (norm_counts.counts.each_col() / sums) * means;
    }
    return norm_counts;
}

STData::STDataFrame STData::ztransform(const STDataFrame &data)
{
    STDataFrame norm_counts = data;
    const auto means = mean(norm_counts.counts, COLUMN);
    const auto sdev = stddev(norm_counts.counts, COLUMN);
    norm_counts.counts = (norm_counts.counts.each_row() - means).each_row() / sdev;
    return norm_counts;
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

    // get columns (genes) >= min_genes
    const urowvec num_genes = sum(data.counts >= min_reads, COLUMN);
    const uvec cols_to_keep = find(num_genes >= min_spots);

    // get rows (spots) >= min_spots
    const ucolvec num_spots = sum(data.counts >= min_reads, ROW);
    const uvec rows_to_keep = find(num_spots >= min_genes);

    // create filtered_matrix
    if (rows_to_keep.size() != data.counts.n_rows
            || cols_to_keep.size() != data.counts.n_cols) {

        STDataFrame sliced_data;
        sliced_data.counts = data.counts.submat(rows_to_keep, cols_to_keep);
        QFuture<void> future1 = QtConcurrent::run([&] {
            for (const auto &i : cols_to_keep) {
                sliced_data.genes.append(data.genes.at(i));
            }
        });

        QFuture<void> future2 = QtConcurrent::run([&]() {
            for (const auto &j : rows_to_keep) {
                sliced_data.spots.append(data.spots.at(j));
            }
        });

        future1.waitForFinished();
        future2.waitForFinished();

        return sliced_data;
    } else {
        return data;
    }
}

const STData::STDataFrame STData::sliceDataSpots(const QList<QString> &spots)
{
    STDataFrame sliced_data;
    std::vector<int> to_keep_rows;
    for (int i = 0; i < spots.size(); ++i) {
        const auto &spot = spots.at(i);
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
    for (int i = 0; i < genes.size(); ++i) {
        const auto &gene = genes.at(i);
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

    // first merge genes and add index to spots (dataset)
    QSet<QString> merged_genes;
    QList<QString> merged_spots;
    for (int i = 0; i < dataframes.size(); ++i) {
        const auto data = dataframes.at(i);
        merged_genes += QSet<QString>(data.genes.begin(), data.genes.end());
        QList<QString> adj_spots;
        std::transform(data.spots.begin(),
                       data.spots.end(),
                       std::back_inserter(adj_spots),
                       [=](const auto &spot) { return QString::number(i) + "_" + spot; });
        merged_spots += adj_spots;
    }

    // create merged data frame
    STDataFrame merged;
    merged.genes = merged_genes.values();
    merged.spots = merged_spots;
    const uword n_rows = merged_spots.size();
    const uword n_cols = merged_genes.size();
    merged.counts = mat(n_rows, n_cols);
    merged.counts.fill(0.0);

    // populate it with the counts
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
                    merged.counts.at(i, j) = data.counts(i, index);
                }
            }
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

    // update selection if spot inside the selection event
    const bool remove = (mode == SelectionEvent::SelectionMode::ExcludeSelection);
    QtConcurrent::blockingMap(m_spots, [&](auto spot) {
        const auto &coord = spot->adj_coordinates();
        if (path.contains(QPointF(coord.x(), coord.y()))) {
            spot->selected(!remove);
        }
    });
}

void STData::selectSpots(const QVector<QString> &spots)
{
    clearSelection();
    #pragma omp parallel for
    for (int i = 0; i < spots.size(); ++i) {
        const auto &spot = spots.at(i);
        const int spot_index = m_spot_index.value(spot, -1);
        if (spot_index != -1) {
            m_spots.at(spot_index)->selected(true);
        }
    }
}

void STData::selectSpots(const QVector<int> &spots_indexes)
{
    clearSelection();
    #pragma omp parallel for
    for (int i = 0; i < spots_indexes.size(); ++i) {
        const int index = spots_indexes.at(i);
        if (index >= 0 && index < m_spots.size()) {
            m_spots.at(index)->selected(true);
        }
    }
}

void STData::loadClusters(const ClusterListType &clusters)
{
    #pragma omp parallel for
    for (int i = 0; i < clusters.size(); ++i) {
        const auto cluster_obj = clusters.at(i);
        const auto &spots = cluster_obj->spots();
        const QColor &color = cluster_obj->color();
        for (int j = 0; j < spots.size(); ++j) {
            const int spot_index = m_spot_index.value(spots.at(j), -1);
            if (spot_index != -1) {
                // Reading should be thread-safe
                m_spots.at(spot_index)->color(color);
                m_spots.at(spot_index)->visible(true);
            }
        }
    }
    m_clusters = clusters;
}

void STData::updateClusters()
{
    #pragma omp parallel for
    for (int i = 0; i < m_clusters.size(); ++i) {
        const auto cluster_obj = m_clusters.at(i);
        const auto &spots = cluster_obj->spots();
        const QColor &color = cluster_obj->color();
        const bool &visible = cluster_obj->visible();
        for (int j = 0; j < spots.size(); ++j) {
            const int spot_index = m_spot_index.value(spots.at(j), -1);
            if (spot_index != -1) {
                // Reading should be thread-safe
                m_spots.at(spot_index)->color(color);
                m_spots.at(spot_index)->visible(visible);
            }
        }
    }
}

void STData::loadGeneColors(const QVector<QString> &genes,
                            const QVector<int> &colors)
{
    Q_ASSERT(genes.size() == colors.size());
    const auto min_max = std::minmax_element(colors.begin(), colors.end());
    const int min = *min_max.first;
    const int max = *min_max.second;
    #pragma omp parallel for
    for (int i = 0; i < genes.size(); ++i) {
        const QString &gene = genes.at(i);
        const int color_value = colors.at(i);
        const QColor color = Color::createCMapColor(color_value,
                                                    min,
                                                    max,
                                                    QCPColorGradient::gpJet);
        const int gene_index = m_gene_index.value(gene, -1);
        if (gene_index != -1) {
            // Reading should be thread-safe
            m_genes.at(gene_index)->color(color);
            m_genes.at(gene_index)->visible(true);
        }
    }
}

const QRectF STData::getBorder() const
{
    const auto mm_x = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().x() < rhs->coordinates().x();
    });

    const auto mm_y = std::minmax_element(m_spots.begin(), m_spots.end(),
                                          [] (const auto lhs, const auto rhs) {
        return lhs->coordinates().y() < rhs->coordinates().y();
    });

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

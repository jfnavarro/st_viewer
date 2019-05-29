#include "STData.h"
#include <QDebug>
#include <QMessageBox>
#include <QtConcurrent>
#include "math/Common.h"
#include "color/HeatMap.h"

#include "xtensor/xarray.hpp"
#include "xframe/xvariable_view.hpp"
#include "xframe/xframe_utils.hpp"
#include "xframe/xvariable.hpp"
#include "xframe/xvariable_masked_view.hpp"
#include "xframe/xaxis_index_slice.hpp"
#include "xtensor/xadapt.hpp"

#include <future>
#include <thread>
#include <variant>
#include <fstream>

//static const int ROW = 1;
//static const int COLUMN = 0;

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
    // Open file
    std::ifstream f(filename.toStdString(), std::ios::in);
    qDebug() << "Opening ST Data file " << filename;

    // Parse the file
    std::vector<std::vector<double>> values;
    std::vector<xf::fstring> genes;
    std::vector<xf::fstring> spots;
    unsigned row_number = 0;
    unsigned col_number = 0;
    char sep = '\t';
    bool parsed = true;
    for (std::string line; std::getline(f, line);) {
        std::istringstream iss(line);
        std::string token;
        std::vector<double> values_row;
        col_number = 0;
        while (std::getline(iss, token, sep)) {
            if (row_number == 0) {
                genes.push_back(token.c_str());
            } else if (col_number == 0) {
                spots.push_back(token.c_str());
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

    if (!parsed || spots.empty() || genes.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    if (values.size() != spots.size()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    if (values.front().size() != genes.size() && values.front().size() - 1) {
        genes.erase(genes.begin());
    } else if (values.front().size() != genes.size()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    qDebug() << "Parsed data file with " << genes.size() << " genes and " << spots.size() << " spots";

    // Convert counts to xtensor
    shape_type shape = {spots.size(), genes.size()};
    data_type data(shape);
    for (unsigned i = 0; i < spots.size(); ++i) {
        for (unsigned j = 0; j < genes.size(); ++j) {
            data(i,j) = values[i][j];
        }
    }

    // Create data frame
    auto spot_axis = axis_type(spots);
    auto gene_axis = axis_type(genes);
    auto coord = coordinate_type({{"spots", spot_axis}, {"genes", gene_axis}});
    auto dim = dimension_type({"spots", "genes"});
    auto dataframe = variable_type(data, coord, dim);

    // returns the data frame
    return dataframe;
}

void STData::init(const QString &filename, const QString &spots_coordinates) {

    // First parse the data frame with the counts
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
    m_spot_index.clear();
    m_gene_index.clear();

    const auto spot_list = xf::get_labels<xf::fstring>(m_data.coordinates()["spots"]);
    for (auto const& spot : spot_list) {
        const QString spot_str = QString::fromUtf8(spot.c_str());
        QString adj_spot = spot_str;
        if (!spots_dict.empty() && spots_dict.contains(spot_str)) {
            adj_spot = spots_dict[spot_str];
        } else if (!spots_dict.empty()) {
            continue;
        }

        const auto row = xf::locate(m_data, spot, xf::all());
        const double row_sum_value = xt::sum(row.data())().value();
        if (row_sum_value > 0) {
            auto spot_obj = SpotObjectType(new Spot(spot_str));
            spot_obj->adj_coordinates(Spot::getCoordinates(adj_spot));
            spot_obj->totalCount(row_sum_value);
            m_spots.push_back(spot_obj);
            m_spot_index.insert(spot_str, m_spots.size() - 1);

            // update the rendering vectors
            m_rendering_coords.append(spot_obj->adj_coordinates());
            m_rendering_colors.append(QVector4D(1.0, 1.0, 1.0, 1.0));
            m_rendering_visible.append(false);
            m_rendering_selected.append(false);
        }
    }

    const auto gene_list = xf::get_labels<xf::fstring>(m_data.coordinates()["genes"]);
    for (const auto &gene : gene_list) {
        const QString gene_str = QString::fromUtf8(gene.c_str());
        const auto col = xf::locate(m_data, xf::all(), gene);
        const double col_sum_value = xt::sum(col.data())().value();
        if (col_sum_value > 0) {
            auto gene_obj = GeneObjectType(new Gene(gene_str));
            gene_obj->totalCount(col_sum_value);
            m_genes.push_back(gene_obj);
            m_gene_index.insert(gene_str, m_genes.size() - 1);
        }
    }

    if (m_spots.empty()) {
        qDebug() << "No valid spots could be found in the file.";
        throw std::runtime_error("No valid spots could be found in the file.");
    }

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
        const auto gene_list = xf::get_labels<xf::fstring>(data.coordinates()["genes"]);
        for (const auto &gene : gene_list) {
            stream << "\t" << gene.c_str();
        }
        stream << endl;
        // write spots (1st column and the rest of the rows (counts))
        const auto spot_list = xf::get_labels<xf::fstring>(data.coordinates()["spots"]);
        for (auto const& spot : spot_list) {
            stream <<  spot.c_str();
            for (const auto &gene : gene_list) {
                stream << "\t" << data.locate(spot, gene).value();
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
    Q_ASSERT(m_data.size() > 0);

    /*const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog;
    const bool do_color =
            rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange ||
            rendering_settings.visual_mode == SettingsWidget::VisualMode::Normal;
    const bool do_values = rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;*/

    // Reset OpenGL arrays
    // TODO use QFuture to launch them in parallel
    QtConcurrent::blockingMap(m_rendering_visible, [] (auto visible) { visible = false; });
    QtConcurrent::blockingMap(m_rendering_selected, [] (auto selected) { selected = false; });

    // Apply filters
    std::cout << "Filtering" << std::endl;
    std::cout << m_data.shape().front() << " " << m_data.shape().back() << std::endl;
    //auto data = filterCounts(m_data,
    //                         rendering_settings.reads_threshold,
    //                         rendering_settings.genes_threshold,
    //                         rendering_settings.spots_threshold);

    const auto data_value = m_data.data().value();
    std::cout << data_value << std::endl;
    const auto row_sums = xt::sum(xt::greater(data_value, rendering_settings.reads_threshold), 1);
    std::cout << row_sums << std::endl;
    const auto row_indexes = xt::ravel_indices<xt::ravel_vector_tag>(
                xt::argwhere(xt::equal(row_sums > rendering_settings.spots_threshold, 1)), row_sums.shape());
    const auto col_sums = xt::sum(xt::greater(data_value, rendering_settings.reads_threshold), 0);
    std::cout << col_sums << std::endl;
    const auto col_indexes = xt::ravel_indices<xt::ravel_vector_tag>(
                xt::argwhere(xt::equal(col_sums > rendering_settings.genes_threshold, 1)), col_sums.shape());
    if (row_indexes.empty() || col_indexes.empty()) {
        std::cout << "Filtering returns an empy dataframe" << std::endl;
        return;
    }
    std::cout << "Reducing with " << row_indexes.size() << " " << col_indexes.size() << std::endl;
    variable_type data = xf::ilocate(m_data, xf::ikeep(row_indexes), xf::ikeep(col_indexes));

    // early out
    if (data.size() == 0) {
        return;
    }
    std::cout << data.data() << std::endl;

    // Normalize
    //std::cout << "Normalizing" << std::endl;
    //data = normalizeCounts(data, rendering_settings.normalization_mode);
    //std::cout << data.data() << std::endl;

    // Filter out selected genes/spots (async)
    std::future<std::vector<int> > f1 = std::async(std::launch::async, [=] {
        std::vector<int> spots_selected_indexes;
        for (int i = 0; i < m_spots.size(); ++i) {
            if (m_spots.at(i)->selected()) {
                spots_selected_indexes.push_back(i);
            }
        };
        return spots_selected_indexes;
    });

    std::future<std::vector<int> > f2 = std::async(std::launch::async, [=] {
        std::vector<int> genes_selected_indexes;
        for (int i = 0; i < m_genes.size(); ++i) {
            if (m_genes.at(i)->selected()) {
                genes_selected_indexes.push_back(i);
            }
        };
        return genes_selected_indexes;
    });

    f1.wait();
    f2.wait();
    const auto spot_idx = f1.get();
    const auto gene_idx = f2.get();

    // early out
    std::cout << "Keeping selected genes/spots" << std::endl;
    if (spot_idx.empty() && gene_idx.empty()) {
        return;
    } else if (spot_idx.empty()) {
        data = xf::iselect(data, {{"genes", xf::ikeep(gene_idx)}});
    } else  if (gene_idx.empty()){
        data = xf::iselect(data, {{"spots", xf::ikeep(spot_idx)}});
    } else {
        data = xf::ilocate(data, xf::ikeep(spot_idx), xf::ikeep(gene_idx));
    }
    std::cout << data.data() << std::endl;

    // Compute rendering colors

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
                if (fields.length() != 4 || fields.length() != 6) {
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
    if (mode == SettingsWidget::REL) {
        const auto row_sums = xt::sum(data.data().value(), {1});
        const auto normalized = data.data().value() / xt::view(row_sums, xt::all(), xt::newaxis());
        return variable_type(normalized, data.coordinates(), data.dimension_mapping());
    } else if (mode == SettingsWidget::CPM) {
        const auto row_sums = xt::sum(data.data().value(), {1});
        const auto mean = xt::mean(row_sums);
        const auto normalized = data.data().value() / xt::view(row_sums * mean, xt::all(), xt::newaxis());
        return variable_type(normalized, data.coordinates(), data.dimension_mapping());
    }
    return data;
}

STData::STDataFrame STData::filterCounts(const STDataFrame &data,
                                         const int min_reads,
                                         const int min_genes,
                                         const int min_spots)
{
    const auto row_sums = xt::sum(xt::greater(data.data().value(), min_reads), {1});
    const auto row_indexes = xt::ravel_indices<xt::ravel_vector_tag>(
                xt::argwhere(xt::equal(row_sums > min_spots, 1)), row_sums.shape());
    const auto col_sums = xt::sum(xt::greater(data.data().value(), min_reads), {0});
    const auto col_indexes = xt::ravel_indices<xt::ravel_vector_tag>(
                xt::argwhere(xt::equal(col_sums > min_genes, 1)), col_sums.shape());
    if (row_indexes.empty() || col_indexes.empty()) {
        std::cout << "Filtering returns an empy dataframe" << std::endl;
        return variable_type();
    }
    return xf::ilocate(data, xf::ikeep(row_indexes), xf::ikeep(col_indexes));
}

STData::STDataFrame STData::aggregate(const QList<STDataFrame> &dataframes)
{
    return STData::STDataFrame();
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
    QtConcurrent::blockingMap(m_spots, [&](QSharedPointer<Spot> spot) -> QSharedPointer<Spot> {
        const auto &coord = spot->coordinates();
        if (path.contains(QPointF(coord.x(), coord.y()))) {
            spot->selected(!remove);
        }
        return spot;
    });
}

void STData::selectSpots(const QList<QString> &spots)
{
    clearSelection();
    for (const auto &spot : spots) {
        QtConcurrent::run([=] {
            const int spot_index = m_spot_index.value(spot, -1);
            if (spot_index != -1) {
                m_spots.at(spot_index)->selected(true);
            }
        });
    }
}

void STData::selectSpots(const QList<int> &spots_indexes)
{
    clearSelection();
    for (const auto index : spots_indexes) {
        QtConcurrent::run([=] {
            if (index >= 0 && index < m_spots.size()) {
                m_spots.at(index)->selected(true);
            }
        });
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
        QtConcurrent::run([=] {
            const int gene_index = m_gene_index.value(gene, -1);
            if (gene_index != -1) {
                m_genes.at(gene_index)->selected(true);
                m_genes.at(gene_index)->visible(true);
            }
        });
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

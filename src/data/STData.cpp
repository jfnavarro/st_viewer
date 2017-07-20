#include "STData.h"
#include <QDebug>
#include <QMessageBox>
#include "math/Common.h"
#include "color/HeatMap.h"
#include "math/RInterface.h"

static const int ROW = 1;
static const int COLUMN = 0;
using namespace Math;

STData::STData()
    : m_data()
    , m_reads_threshold(0)
    , m_genes_threshold(5)
    , m_ind_reads_treshold(0)
    , m_spots_threshold(5)
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
    bool parsed = true;
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
                if (items.size() != 2) {
                    parsed = false;
                    break;
                }
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

    if (!parsed || data.spots.empty() || data.genes.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    // Remove the first empty column name (gene)
    if (data.genes.size() == col_number) {
        data.genes.removeAt(0);
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
    colvec row_sum = sum(m_data.counts, ROW);
    rowvec col_sum = sum(m_data.counts, COLUMN);
    // Create genes/spots lists
    for (uword i = 0; i < m_data.counts.n_rows; ++i) {
        auto spot = SpotObjectType(new Spot(m_data.spots.at(i)));
        spot->totalCount(row_sum.at(i));
        m_spots.append(spot);
    }
    for (uword j = 0; j < m_data.counts.n_cols; ++j) {
        auto gene = GeneObjectType(new Gene(m_data.genes.at(j)));
        gene->totalCount(col_sum.at(j));
        m_genes.append(gene);
    }
    m_rendering_colors.resize(m_spots.size());
    m_rendering_selected.resize(m_spots.size());
    m_rendering_visible.resize(m_spots.size());
    m_rendering_values.resize(m_spots.size());
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

    const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;
    const bool do_color =
            rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange ||
            rendering_settings.visual_mode == SettingsWidget::VisualMode::Normal;

    STDataFrame data = m_data;

    // Apply spike-ins and size factors if indicated by the user
    if (rendering_settings.spike_in) {
        data.counts.each_col() /= m_spike_in.t();
    }
    if (rendering_settings.size_factors) {
        data.counts.each_col() /= m_size_factors.t();
    }

    bool recompute_size_factors = false;
    if (m_reads_threshold != rendering_settings.ind_reads_threshold) {
        m_reads_threshold = rendering_settings.ind_reads_threshold;
        recompute_size_factors = true;
    }

    if (m_genes_threshold != rendering_settings.genes_threshold) {
        m_genes_threshold = rendering_settings.genes_threshold;
        recompute_size_factors = true;
    }

    if (m_ind_reads_treshold != rendering_settings.ind_reads_threshold) {
        m_ind_reads_treshold = rendering_settings.ind_reads_threshold;
        recompute_size_factors = true;
    }

    if (m_spots_threshold != rendering_settings.spots_threshold) {
        m_spots_threshold = rendering_settings.spots_threshold;
        recompute_size_factors = true;
    }

    // slice the data frame (TODO we could cache it and avoid this always)
    data = filterDataFrame(data,
                           rendering_settings.ind_reads_threshold,
                           rendering_settings.reads_threshold,
                           rendering_settings.genes_threshold,
                           rendering_settings.spots_threshold);

    // early out
    if (data.to_keep_genes.empty() || data.to_keep_spots.empty()) {
        return;
    }

    // check if we need to recompute normalization factors
    if (recompute_size_factors) {
        // recompute size factors then
        m_deseq_size_factors = RInterface::computeDESeqFactors(data.counts);
        m_scran_size_factors = RInterface::computeScranFactors(data.counts, false);
    }

    // set visible to false to all the spots
    std::fill(m_rendering_visible.begin(), m_rendering_visible.end(), true);

    // Normalize the counts
    data = normalizeCounts(data, m_deseq_size_factors, m_scran_size_factors,
                           rendering_settings.normalization_mode);

    // Iterate the spots and genes in the matrix to compute the rendering colors
    double min_value = 10e6;
    double max_value = -10e6;
    //TODO make this paralell
    for (uword i = 0; i < data.counts.n_rows; ++i) {
        const int spot_index = data.to_keep_spots.at(i);
        const auto spot = m_spots.at(spot_index);
        bool visible = false;
        double merged_value = 0.0;
        double num_genes = 0.0;
        bool any_gene_selected = false;
        QColor merged_color;
        // Iterate the genes in the spot to compute the sum of values and color
        for (uword j = 0; j < data.counts.n_cols; ++j) {
            const int gene_index = data.to_keep_genes.at(j);
            const auto gene = m_genes.at(gene_index);
            const double value = data.counts.at(i,j);
            if (!gene->visible() || value <= rendering_settings.ind_reads_threshold
                    || (rendering_settings.gene_cutoff && gene->cut_off() >= value)) {
                continue;
            }
            ++num_genes;
            merged_value += value;
            if (do_color) {
                merged_color = lerp(1.0 / num_genes, merged_color, gene->color());
            }
            any_gene_selected |= gene->selected();
        }
        // Update the color of the spot
        if (spot->visible()) {
            merged_color = spot->color();
            merged_value = 0.0;
            visible = true;
        } else if (merged_value > 0.0) {
            // Use number of genes or total reads in the spot depending on settings
            if (!do_color) {
                merged_value = use_genes ? num_genes : merged_value;
                merged_value = use_log ? std::log(merged_value) : merged_value;
                min_value = std::min(min_value, merged_value);
                max_value = std::max(max_value, merged_value);
            }
            visible = true;
        }
        spot->selected(visible && (spot->selected() || any_gene_selected));
        m_rendering_colors[spot_index] = merged_color;
        m_rendering_selected[spot_index] = spot->selected();
        m_rendering_values[spot_index] = merged_value;
        m_rendering_visible[spot_index] = visible;
    }
    rendering_settings.legend_min = min_value;
    rendering_settings.legend_max = max_value;
}

const QVector<bool> &STData::renderingVisible() const
{
    return m_rendering_visible;
}

const QVector<QColor> &STData::renderingColors() const
{
    return m_rendering_colors;
}

const QVector<bool> &STData::renderingSelected() const
{
    return m_rendering_selected;
}

const QVector<double> &STData::renderingValues() const
{
    return m_rendering_values;
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
            qDebug() << "No valid spots were found in the spots file";
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
        qDebug() << "No matching spots were found in the spots file";
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

bool STData::parseSpikeIn(const QString &spikeInFile)
{
    qDebug() << "Parsing spike-in file " << spikeInFile;
    QFile file(spikeInFile);
    std::vector<double> spike_ins;
    bool parsed = true;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() != 1) {
                parsed = false;
                break;
            }
            const double spike_in = fields.at(0).toFloat();
            spike_ins.push_back(spike_in);
        }

        if (spike_ins.empty() || !parsed) {
            qDebug() << "No valid spike-ins were found in the given file";
            parsed = false;
        }

    } else {
        qDebug() << "Could not parse spike-ins file";
        parsed = false;
    }
    file.close();

    if (!parsed) {
        return parsed;
    }

    if (spike_ins.size() != m_spots.size()) {
        qDebug() << "The number of spike-ins found is not the same as the number of rows";
        parsed = false;
    } else {
        m_spike_in = rowvec(spike_ins);
    }

    return parsed;
}

bool STData::parseSizeFactors(const QString &sizefactors)
{
    qDebug() << "Parsing size factors file " << sizefactors;
    QFile file(sizefactors);
    std::vector<double> size_factors;
    bool parsed = true;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() != 1) {
                parsed = false;
                break;
            }
            const double size_factor = fields.at(0).toFloat();
            size_factors.push_back(size_factor);
        }

        if (size_factors.empty() || !parsed) {
            qDebug() << "No valid size factors were found in the given file";
            parsed = false;
        }

    } else {
        qDebug() << "Could not parse size factors file";
        parsed = false;
    }
    file.close();

    if (!parsed) {
        return parsed;
    }

    if (size_factors.size() != m_spots.size()) {
        qDebug() << "The number of size factors found is not the same as the number of rows";
        parsed = false;
    } else {
        m_size_factors = rowvec(size_factors);
    }

    return parsed;
}

STData::STDataFrame STData::normalizeCounts(const STDataFrame &data,
                                            const rowvec deseq_size_factors,
                                            const rowvec scran_size_factors,
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
        norm_counts.counts.each_col() /= deseq_size_factors.t();
    } break;
    case (SettingsWidget::NormalizationMode::SCRAN): {
        norm_counts.counts.each_col() /= scran_size_factors.t();
    }
    }
    return norm_counts;
}

STData::STDataFrame STData::filterDataFrame(const STDataFrame &data,
                                            const int min_exp_value,
                                            const int min_reads_spot,
                                            const int min_genes_spot,
                                            const int min_spots_gene)
{
    STDataFrame sliced_data = data;
    std::vector<uword> to_keep_genes;

    // Filter out genes
    rowvec spot_counts = computeNonZeroColumns(sliced_data.counts, min_exp_value);
    QList<QString> genes;
    for (uword j = 0; j < sliced_data.counts.n_cols; ++j) {
        const double spot_count = spot_counts.at(j);
        if (spot_count > min_spots_gene) {
            to_keep_genes.push_back(j);
            genes.push_back(sliced_data.genes.at(j));
        }
    }
    sliced_data.genes = genes;
    sliced_data.to_keep_genes = to_keep_genes;
    sliced_data.counts = sliced_data.counts.cols(uvec(to_keep_genes));

    // Filter out spots
    colvec rowsum = sum(sliced_data.counts.elem(find(sliced_data.counts > min_exp_value)), ROW);
    colvec gene_counts = computeNonZeroRows(sliced_data.counts, min_exp_value);
    QList<Spot::SpotType> spots;
    std::vector<uword> to_keep_spots;
    for (uword i = 0; i < sliced_data.counts.n_rows; ++i) {
        const double reads_count = rowsum.at(i);
        const double genes_count = gene_counts.at(i);
        if (reads_count > min_reads_spot && genes_count > min_genes_spot) {
            to_keep_spots.push_back(i);
            spots.push_back(sliced_data.spots.at(i));
        }
    }
    sliced_data.spots = spots;
    sliced_data.to_keep_spots = to_keep_spots;
    sliced_data.counts = sliced_data.counts.rows(uvec(to_keep_spots));

    return sliced_data;
}

rowvec STData::computeNonZeroColumns(const mat &matrix, const int min_value)
{
    rowvec non_zeros(matrix.n_cols);
    for (uword i = 0; i < matrix.n_cols; ++i) {
        const uvec t = find(matrix.col(i) > min_value);
        non_zeros[i] = t.n_elem;
    }
    return non_zeros;
}

colvec STData::computeNonZeroRows(const mat &matrix, const int min_value)
{
    colvec non_zeros(matrix.n_rows);
    for (uword i = 0; i < matrix.n_rows; ++i) {
        const uvec t = find(matrix.row(i) > min_value);
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
        if (path.contains(QPointF(coord.first, coord.second))) {
            spot->selected(!remove);
        }
    }
}

void STData::selectSpots(const QList<Spot::SpotType> &spots)
{
   clearSelection();
   for (auto spot : m_spots) {
       auto coord = spot->coordinates();
       if (spots.contains(coord)) {
           spot->selected(true);
       }
   }
}

void STData::selectSpots(const QList<unsigned> &spots_indexes)
{
   clearSelection();
   for (auto index : spots_indexes) {
       m_spots.at(index)->selected(true);
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
    for (auto gene : m_genes) {
        for (auto gene2 : genes) {
            if (gene->name() == gene2) {
                gene->visible(true);
                gene->selected(true);
                break;
            }
        }
    }
}

void STData::loadSpotColors(const QHash<Spot::SpotType, QColor> &colors)
{
    QHash<Spot::SpotType, QColor>::const_iterator it = colors.constBegin();
    while (it != colors.constEnd()) {
        const Spot::SpotType oldspot = it.key();
        const QColor color = it.value();
        auto it_spot =
                std::find_if(m_spots.begin(), m_spots.end(),
                             [&](const auto spot) { return spot->coordinates() == oldspot; });
        if (it_spot != m_spots.end()) {
            (*it_spot)->color(color);
            (*it_spot)->visible(true);
        }
        ++it;
    }
}

void STData::loadGeneColors(const QHash<QString, QColor> &colors)
{
    QHash<QString, QColor>::const_iterator it = colors.constBegin();
    while (it != colors.constEnd()) {
        const QString oldgene = it.key();
        const QColor color = it.value();
        auto it_gene =
                std::find_if(m_genes.begin(), m_genes.end(),
                             [&](const auto gene) { return gene->name() == oldgene; });
        if (it_gene != m_genes.end()) {
            (*it_gene)->color(color);
            (*it_gene)->visible(true);
        }
        ++it;
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

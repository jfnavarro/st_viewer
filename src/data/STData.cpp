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
    , m_spike_in()
    , m_size_factors()
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
    // Create genes/spots lists
    for (uword i = 0; i < m_data.counts.n_rows; ++i) {
        m_spots.append(SpotObjectType(new Spot(m_data.spots.at(i))));
    }
    for (uword j = 0; j < m_data.counts.n_cols; ++j) {
        m_genes.append(GeneObjectType(new Gene(m_data.genes.at(j))));
    }
    // init spike-in and size-factors
    m_spike_in = rowvec(m_data.counts.n_rows);
    m_spike_in.fill(1.0);
    m_size_factors = rowvec(m_data.counts.n_rows);
    m_size_factors.fill(1.0);
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
    m_rendering_values.clear();

    const bool use_genes =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::Genes ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;
    const bool use_log =
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::ReadsLog ||
            rendering_settings.visual_type_mode == SettingsWidget::VisualTypeMode::GenesLog;
    const bool is_dynamic =
            rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange;
    const bool is_normal=
            rendering_settings.visual_mode == SettingsWidget::VisualMode::Normal;

    mat counts = m_data.counts;

    if (rendering_settings.spike_in) {
        counts.each_col() /= m_spike_in.t();
    }

    if (rendering_settings.size_factors) {
        counts.each_col() /= m_size_factors.t();
    }

    // Compute row-column sums
    colvec rowsum = sum(counts, ROW);
    colvec gene_counts = computeNonZeroRows(counts);
    rowvec spot_counts = computeNonZeroColumns(counts);

    // Filter out spots
    std::vector<uword> to_keep_spots;
    for (uword i = 0; i < counts.n_rows; ++i) {
        const double reads_count = rowsum.at(i);
        const double genes_count = gene_counts.at(i);
        if (reads_count >= rendering_settings.reads_threshold
                && genes_count >= rendering_settings.genes_threshold) {
            to_keep_spots.push_back(i);
        }
    }

    // Filter out genes
    std::vector<uword> to_keep_genes;
    for (uword j = 0; j < counts.n_cols; ++j) {
        const double spot_count = spot_counts.at(j);
        const auto gene = m_genes.at(j);
        if (gene->visible() && spot_count >= rendering_settings.spots_threshold) {
            to_keep_genes.push_back(j);
        }
    }

    if (to_keep_genes.empty() || to_keep_spots.empty()) {
        return;
    }

    // slice the matrix of counts
    counts = counts.submat(uvec(to_keep_spots), uvec(to_keep_genes));

    // check if the number of filterd genes/spots have changed
    if (m_counts.empty() || m_counts.n_cols != counts.n_cols || m_counts.n_rows != counts.n_rows) {
        // recompute size factors
        m_deseq_size_factors = RInterface::computeDESeqFactors(counts);
        m_scran_size_factors = RInterface::computeScranFactors(counts);
        m_counts = counts;
    }

    // Normalize the counts
    counts = normalizeCounts(counts,
                             rendering_settings.normalization_mode,
                             m_deseq_size_factors,
                             m_scran_size_factors);

    // Iterate the spots and genes in the matrix to compute the rendering colors
    //TODO make this paralell
    double min_value = 10e6;
    double max_value = -10e6;
    for (uword i = 0; i < counts.n_rows; ++i) {
        const auto spot = m_spots.at(i);
        bool visible = false;
        double merged_value = 0.0;
        double num_genes = 0.0;
        bool any_gene_selected = false;
        QColor merged_color;
        if (!spot->visible()) {
            // Iterage the genes in the spot to compute the sum of values and color
            for (uword j = 0; j < counts.n_cols; ++j)  {
                const auto gene = m_genes.at(j);
                const double value = counts.at(i,j);
                if (value <= rendering_settings.ind_reads_threshold
                        || (rendering_settings.gene_cutoff && gene->cut_off() >= value)) {
                    continue;
                }
                ++num_genes;
                merged_value += value;
                if (is_normal || is_dynamic) {
                    merged_color = lerp(1.0 / num_genes, merged_color, gene->color());
                }
                any_gene_selected |= gene->selected();
            }
            // Update the color of the spot
            if (merged_value > 0.0) {
                // Use number of genes or total reads in the spot depending on settings
                merged_value = use_genes ? num_genes : merged_value;
                merged_value = use_log ? std::log(merged_value) : merged_value;
                min_value = std::min(min_value, merged_value);
                max_value = std::max(max_value, merged_value);
                visible = true;
                spot->selected(spot->selected() || any_gene_selected);
            }
        } else {
            visible = true;
            merged_color = spot->color();
        }

        if (visible) {
            m_rendering_colors.append(merged_color);
            m_rendering_selected.append(spot->selected());
            m_rendering_values.append(merged_value);
            m_rendering_spots.append(spot->coordinates());
        }
    }
    rendering_settings.legend_min = min_value;
    rendering_settings.legend_max = max_value;
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
        QMessageBox::warning(nullptr,
                             QObject::tr("Spot coordinates"),
                             QObject::tr("No spots could be matched in the spots file"));
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
    } else {
        m_size_factors = rowvec(size_factors);
    }

    return parsed;
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
        norm_counts.each_col() /= sum(norm_counts, ROW);
        norm_counts *= 1e6;
    } break;
    case (SettingsWidget::NormalizationMode::DESEQ): {
        norm_counts.each_col() /= deseq_factors.t();
    } break;
    case (SettingsWidget::NormalizationMode::SCRAN): {
        norm_counts.each_col() /= scran_factors.t();
    }
    }
    return norm_counts;
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
        const bool selected = regexp.exactMatch(gene->name());
        gene->selected(selected);
        gene->visible(gene->visible() || (force && selected));
    }
}

void STData::selectGenes(const QList<QString> &genes)
{
    for (auto gene : m_genes) {
        for (auto gene2 : genes) {
            if (gene->name() == gene2) {
                gene->visible(true);
                break;
            }
        }
    }
}

void STData::loadSpotColors(const QVector<QColor> &colors)
{
    Q_ASSERT(m_spots.size() == colors.size());
    for (int i = 0; i < colors.size(); ++i) {
        m_spots[i]->color(colors.at(i));
    }
}

void STData::loadSpotColors(const QMap<Spot::SpotType,QColor> &colors)
{
    QMap<Spot::SpotType, QColor>::const_iterator it = colors.constBegin();
    while (it != colors.constEnd()) {
        const Spot::SpotType oldspot = it.key();
        const QColor color = it.value();
        auto it_spot =
                std::find_if(m_spots.begin(), m_spots.end(),
                             [&](const auto spot) { return spot->coordinates() == oldspot; });
        if (it_spot != m_spots.end()) {
            (*it_spot)->color(color);
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

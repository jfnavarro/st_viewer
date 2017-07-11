#ifndef STDATA_H
#define STDATA_H

#include <QSharedPointer>
#include <QList>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QColor>

#include "data/Gene.h"
#include "data/Spot.h"
#include "viewPages/SettingsWidget.h"
#include "math/QuadTree.h"
#include "viewRenderer/SelectionEvent.h"

#include <armadillo>

using namespace arma;

class STData
{

public:

    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QList<SpotObjectType> SpotListType;
    typedef QList<GeneObjectType> GeneListType;

    struct STDataFrame {
        mat counts;
        QList<QString> genes;
        QList<Spot::SpotType> spots;
        // when spots/genes are filtered
        std::vector<uword> to_keep_genes;
        std::vector<uword> to_keep_spots;
    };

    STData();
    ~STData();

    // Parses the matrix and initialize the size-factors and genes/spots containers
    void init(const QString &filename);

    // Functions to import/export the data
    static STDataFrame read(const QString &filename);
    static void save(const QString &filename, const STDataFrame &data);

    // Retrieves the original data frame (without filtering using the tresholds)
    STDataFrame data() const;

    // Returns the spot/gene objects corresponding to the data frame
    GeneListType genes();
    SpotListType spots();

    // Rendering functions
    void computeRenderingData(SettingsWidget::Rendering &rendering_settings);
    const QVector<bool> &renderingVisible() const;
    const QVector<QColor> &renderingColors() const;
    const QVector<bool> &renderingSelected() const;
    const QVector<double> &renderingValues() const;

    // to parse a file with spots coordinates old_spot -> new_spot
    // the spots coordinates will be updated and the spots
    // that are not found will be removed if the user says yes to this
    // it returns true if the parsing went fine
    bool parseSpotsMap(const QString &spots_file);

    // to parse a file with spike-in factors (one per spot)
    // it returns bool if the parsing was okay and the number of factors is the same as rows
    bool parseSpikeIn(const QString &spikeInFile);

    // to parse a file with size factors (one per spot)
    // it returns bool if the parsing was okay and the number of factors is the same as rows
    bool parseSizeFactors(const QString &spikeInFile);

    // helper function to filter out a data frame using thresholds
    static STDataFrame filterDataFrame(const STDataFrame &data,
                                       const int min_exp_value,
                                       const int min_reads_spot,
                                       const int min_genes_spot,
                                       const int min_spots_gene);
    // helper function to get the sum of non zeroes elements (by column, aka gene)
    static rowvec computeNonZeroColumns(const mat &matrix, const int min_value = 0);
    // helper function to get the sum of non zeroes elements (by row, aka spot)
    static colvec computeNonZeroRows(const mat &matrix, const int min_value = 0);
    // helper function that returns the normalized matrix counts using the rendering settings
    static STDataFrame normalizeCounts(const STDataFrame &data,
                                       const rowvec deseq_size_factors,
                                       const rowvec scran_size_factors,
                                       SettingsWidget::NormalizationMode mode);

    // functions to select spots
    void clearSelection();
    void selectSpots(const SelectionEvent &event);
    void selectSpots(const QList<Spot::SpotType> &spots);
    void selectSpots(const QList<unsigned> &spots_indexes);
    void selectGenes(const QRegExp &regexp, const bool force = true);
    void selectGenes(const QList<QString> &genes);

    // functions to change spot colors
    void loadSpotColors(const QHash<Spot::SpotType,QColor> &colors);

    // returns the boundaries (min spot and max spot)
    const QRectF getBorder() const;

private:

    // The matrix with the counts (spots are rows and genes are columns)
    STDataFrame m_data;

    // cache the thresholds settings to not re-compute always
    int m_reads_threshold;
    int m_genes_threshold;
    int m_ind_reads_treshold;
    int m_spots_threshold;

    // scran and deseq2 size factors
    rowvec m_deseq_size_factors;
    rowvec m_scran_size_factors;

    // user loaded spike-in
    rowvec m_spike_in;

    // user loaded size factors
    rowvec m_size_factors;

    // store gene/spots objects for the matrix (columns and rows)
    // each index in each vector correspond to a row index or column index in the matrix
    SpotListType m_spots;
    GeneListType m_genes;

    // rendering data
    QVector<bool> m_rendering_selected;
    QVector<bool> m_rendering_visible;
    QVector<QColor> m_rendering_colors;
    QVector<double> m_rendering_values;

    Q_DISABLE_COPY(STData)
};

#endif // STDATA_H

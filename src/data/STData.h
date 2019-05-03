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
#include "viewRenderer/SelectionEvent.h"

#include "xframe/xio.hpp"
#include "xframe/xvariable.hpp"

class STData
{

public:

    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QList<SpotObjectType> SpotListType;
    typedef QList<GeneObjectType> GeneListType;

    using coordinate_type = xf::xcoordinate<xf::fstring>;
    using variable_type = xf::xvariable<double, coordinate_type>;
    using data_type = variable_type::data_type;
    using shape_type = data_type::shape_type;

    typedef variable_type STDataFrame;

    STData();
    ~STData();

    // Parses the matrix and initialize the size-factors and genes/spots containers
    void init(const QString &filename, const QString &spots_coordinates = QString());

    // Functions to import/export the data
    static STDataFrame read(const QString &filename);
    static void save(const QString &filename, const STDataFrame &data);

    // Retrieves the original data frame (without filtering using the tresholds)
    STDataFrame data() const;

    // Returns the spot/gene objects corresponding to the data frame
    const GeneListType &genes() const;
    const SpotListType &spots() const;

    // Rendering functions
    void computeRenderingData(SettingsWidget::Rendering &rendering_settings);
    const QVector<int> &renderingVisible() const;
    const QVector<QVector4D> &renderingColors() const;
    const QVector<int> &renderingSelected() const;
    const QVector<Spot::SpotType> &renderingCoords() const;

    // to parse a file with spots coordinates old_spot -> new_spot
    // It returns a map of old_spots -> new_spots
    // It throws exceptions when errors during parsing or empty file
    QMap<QString, QString> parseSpotsMap(const QString &spots_file);

    // helper function to filter out a data frame using thresholds
    static STDataFrame filterDataFrame(const STDataFrame &data,
                                       const int min_exp_value,
                                       const int min_reads_spot,
                                       const int min_genes_spot,
                                       const int min_spots_gene);

    // helper function that returns the normalized matrix counts using the rendering settings
    static STDataFrame normalizeCounts(const STDataFrame &data,
                                       SettingsWidget::NormalizationMode mode);

    // helper function that merges two dataframes
    static STDataFrame aggregate(const QList<STDataFrame> &dataframes);

    // functions to select spots
    void clearSelection();
    void selectSpots(const SelectionEvent &event);
    void selectSpots(const QList<QString> &spots);
    void selectSpots(const QList<int> &spots_indexes);
    void selectGenes(const QRegExp &regexp, const bool force = true);
    void selectGenes(const QList<QString> &genes);

    // functions to change spot and gene colors
    void loadSpotColors(const QHash<QString, QColor> &colors);
    void loadGeneColors(const QHash<QString, QColor> &colors);

    // returns the boundaries (min spot and max spot)
    const QRectF getBorder() const;

    // set and get for the 3D flag
    bool is3D() const;
    void is3D(bool is3D);

private:

    // The matrix with the counts (spots are rows and genes are columns)
    STDataFrame m_data;
    STDataFrame m_norm_data;

    // store gene/spots objects for the matrix (columns and rows)
    // each index in each vector correspond to a row index or column index in the matrix
    SpotListType m_spots;
    GeneListType m_genes;

    // use hash tables for look-ups (spot and gene to matrix index)
    // so to not have to search the QLists
    QHash<QString, int> m_spot_index;
    QHash<QString, int> m_gene_index;

    // rendering data
    QVector<int> m_rendering_visible;
    QVector<QVector4D> m_rendering_colors;
    QVector<Spot::SpotType> m_rendering_coords;
    QVector<int> m_rendering_selected;

    // whether the data is in 3D or not
    bool m_is3D;

    Q_DISABLE_COPY(STData)
};

#endif // STDATA_H

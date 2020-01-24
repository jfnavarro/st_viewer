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

#include <armadillo>

using namespace arma;

class STData
{

public:

    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QVector<SpotObjectType> SpotListType;
    typedef QVector<GeneObjectType> GeneListType;

    struct STDataFrame {
        mat counts;
        QList<QString> genes;
        QList<QString> spots;
    };

    STData();
    ~STData();

    // Parses the data frame and genes/spots containers
    void init(const QString &filename, const QString &spots_coordinates = QString());

    // Functions to import/export a data frame
    static STDataFrame read(const QString &filename);
    static void save(const QString &filename, const STDataFrame &data);

    // Retrieves the original data frame (without filtering)
    STDataFrame data() const;

    // Returns the spot/gene objects corresponding to the data frame
    const GeneListType &genes() const;
    const SpotListType &spots() const;

    // Rendering functions (OpenGL)
    void computeRenderingData(SettingsWidget::Rendering &rendering_settings);
    const QVector<int> &renderingVisible() const;
    const QVector<QVector4D> &renderingColors() const;
    const QVector<int> &renderingSelected() const;
    const QVector<Spot::SpotType> &renderingCoords() const;

    // Parses a file with spots coordinates old_spot -> new_spot
    // It returns a map of old_spots -> new_spots
    // It throws exceptions when errors happen during parsing or an empty file
    QMap<QString, QString> parseSpotsMap(const QString &spots_file);

    // helper function that normalizes a data frame and returns it
    static STDataFrame normalizeCounts(const STDataFrame &data,
                                       SettingsWidget::NormalizationMode mode);

    // helper function filters a data frame and returns it
    static STDataFrame filterCounts(const STDataFrame &data,
                                    const int min_reads,
                                    const int min_genes,
                                    const int min_spots);
    const STDataFrame sliceDataSpots(const QList<QString> &spots);
    const STDataFrame sliceDataGenes(const QList<QString> &genes);

    // helper function that merges a list of dataframes
    static STDataFrame aggregate(const QList<STDataFrame> &dataframes);

    // functions to select spots
    void clearSelection();
    void selectSpots(const SelectionEvent &event);
    void selectSpots(const QList<QString> &spots);
    void selectSpots(const QList<int> &spots_indexes);

    // functions to change spot and gene colors
    void loadSpotColors(const QHash<QString, int> &colors);
    void loadGeneColors(const QHash<QString, int> &colors);

    // returns the boundaries of the spots in the data frame (min spot and max spot coordinates)
    const QRectF getBorder() const;

    // a flag telling if the data frame is 3D
    bool is3D() const;
    void is3D(bool is3D);

private:

    // The matrix with the counts (spots are rows and genes are columns)
    STDataFrame m_data;

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

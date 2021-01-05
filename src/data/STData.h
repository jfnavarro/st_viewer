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
#include "data/Cluster.h"
#include "viewPages/SettingsWidget.h"
#include "viewRenderer/SelectionEvent.h"

#include <armadillo>

using namespace arma;

class STData
{

public:

    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QSharedPointer<Cluster> ClusterObjectType;
    typedef QVector<SpotObjectType> SpotListType;
    typedef QVector<GeneObjectType> GeneListType;
    typedef QVector<ClusterObjectType> ClusterListType;

    struct STDataFrame {
        mat counts;
        QList<QString> genes;
        QList<QString> spots;
    };

    STData();
    ~STData();

    // parses the dataset (counts matrix and spot coordinates)
    void init(const QString &filename, const QString &spots_coordinates);

    // functions to import/export a counts matrix
    static STDataFrame read(const QString &filename);
    static void save(const QString &filename, const STDataFrame &data);

    // retrieves the original data matrix (without filtering)
    STDataFrame data() const;

    // returns the spot/gene objects corresponding to the data matrix
    const GeneListType &genes() const;
    const SpotListType &spots() const;

    // returns the clusters if any
    const ClusterListType &clusters() const;

    // updates the rendering (OpenGL) data
    void computeRenderingData(SettingsWidget::Rendering &rendering_settings);

    // returns the rendering (OpenGL) data vectors
    const QVector<int> &renderingVisible() const;
    const QVector<QVector4D> &renderingColors() const;
    const QVector<int> &renderingSelected() const;
    const QVector<Spot::SpotType> &renderingCoords() const;

    // helper function that normalizes a data frame and returns it
    static STDataFrame normalizeCounts(const STDataFrame &data,
                                       SettingsWidget::NormalizationMode mode);
    
    // helper function that applies the standard transformation (by columns) to a data frame
    // and returns it
    static STDataFrame ztransform(const STDataFrame &data);
    
    // helper functions to filter (slice) a data frame and returns it
    static STDataFrame filterCounts(const STDataFrame &data,
                                    const int min_reads,
                                    const int min_genes,
                                    const int min_spots);
    const STDataFrame sliceDataSpots(const QList<QString> &spots);
    const STDataFrame sliceDataGenes(const QList<QString> &genes);

    // helper function that merges a list of data matrices
    static STDataFrame aggregate(const QList<STDataFrame> &dataframes);

    // functions to select spots
    void clearSelection();
    void selectSpots(const SelectionEvent &event);
    void selectSpots(const QVector<QString> &spots);
    void selectSpots(const QVector<int> &spots_indexes);

    // load spot clusters with meta info
    void loadClusters(const ClusterListType &clusters);

    // to notify that the clusters have been updated
    void updateClusters();

    // Load gene colours
    void loadGeneColors(const QVector<QString> &genes,
                        const QVector<int> &colors);

    // returns the boundaries of the spots in the data matrix (min spot and max spot coordinates)
    const QRectF getBorder() const;

    // a flag telling if the data frame is 3D
    bool is3D() const;
    void is3D(bool is3D);

private:

    // parses a file with spots coordinates
    // it returns a map of spot -> pixel coordinates
    // it throws exceptions when errors happen during parsing or an empty file
    QMap<QString, Spot::SpotType> parseSpotsMap(const QString &spots_file) const;

    // the ST data frame (matrix of counts, genes and spots)
    STDataFrame m_data;

    // gene and Spot objects contaning all the info for each spot/gene
    // each index in each list correspond to a row index (spot) or column index (gene) in m_data
    SpotListType m_spots;
    GeneListType m_genes;

    // clusters can be loaded from a file or from AnalysisClustering
    // users can interact with clusters so to make change the visible/color
    // of the spots belonging to the clusters
    ClusterListType m_clusters;

    // convenience hash tables (gene name -> index) and (spot name -> index)
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

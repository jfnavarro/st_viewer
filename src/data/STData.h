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

class RInside;

class STData
{

public:
    typedef Mat<float> Matrix;
    typedef Row<float> rowvec;
    typedef Col<float> colvec;
    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QList<SpotObjectType> SpotListType;
    typedef QList<GeneObjectType> GeneListType;
    // lookup quadtree type (spot indexes)
    typedef QuadTree<int, 8> SpotsQuadTree;

    STData();
    ~STData();

    // Clone the data using only the spots and genes that are selected
    QSharedPointer<STData> clone() const;

    // Functions to import/export the matrix
    void read(const QString &filename);
    void save(const QString &filename) const;

    // Some gettings
    Matrix matrix_counts() const;
    size_t number_spots() const;
    size_t number_genes() const;
    GeneListType genes();
    SpotListType spots();

    // Rendering functions
    void initRenderingData();
    void computeRenderingData(SettingsWidget::Rendering &rendering_settings);
    const QVector<unsigned> &renderingIndexes() const;
    const QVector<QVector3D> &renderingVertices() const;
    const QVector<QVector2D> &renderingTextures() const;
    const QVector<QVector4D> &renderingColors() const;
    const QVector<float> &renderingSelected() const;

    // to parse a file with spots coordinates old_spot -> new_spot
    // the spots coordinates will be updated and the spots
    // that are not found will be removed if the user says yes to this
    bool parseSpotsMap(const QString &spots_file);

    // helper function to get the sum of non zeroes elements (by column, aka gene)
    static inline rowvec computeNonZeroColumns(const Matrix &matrix);
    // helper function to get the sum of non zeroes elements (by row, aka spot)
    static inline colvec computeNonZeroRows(const Matrix &matrix);
    // helper function that returns the normalized matrix counts using the rendering settings
    static inline Matrix normalizeCounts(const Matrix &counts,
                                         SettingsWidget::NormalizationMode mode,
                                         const rowvec &deseq_factors,
                                         const rowvec &scran_factors);
    // helper fuctions to adjust a spot's color according to the rendering settings
    static inline QColor adjustVisualMode(const QColor merged_color,
                                          const float &merged_value,
                                          const float &min_reads,
                                          const float &max_reads,
                                          const SettingsWidget::VisualMode mode);
    // functions to select spots
    void clearSelection();
    void selectSpots(const SelectionEvent &event);
    void selectGenes(const QRegExp &regexp, const bool force = true);
    // returns true if there are valid selections
    bool hasSelection() const;

    // returns the boundaries (min spot and max spot)
    const QRectF getBorder() const;

private:
    // function to compute a default values for individual genes cut-off
    void computeGenesCutoff();
    // helper function to update the color and selected of a spot (rendering data)
    inline void updateColor(const int index, const QColor &color);
    inline void updateSelected(const int index, const bool &selected);
    // update the size of the spots
    void updateSize(const float size);
    // helper functions to compute normalization size factors that are used to normalize
    void computeDESeqFactors();
    void computeScranFactors();

    // The matrix with the counts (spots are rows and genes are columns)
    Matrix m_counts_matrix;
    // cache the size factors to save computational time
    rowvec m_deseq_size_factors;
    rowvec m_scran_size_factors;
    // store gene/spots objects for the matrix (columns and rows)
    // each index in each vector correspond to a row index or column index in the matrix
    SpotListType m_spots;
    GeneListType m_genes;
    // rendering data
    QVector<float> m_selected;
    QVector<QVector3D> m_vertices;
    QVector<QVector2D> m_textures;
    QVector<QVector4D> m_colors;
    QVector<unsigned> m_indexes;
    // quad tree container (used to find spots by coordinates)
    SpotsQuadTree m_quadTree;
    // R terminal
    QScopedPointer<RInside> R;
    // Save the size to not recompute it always
    float m_size;

    Q_DISABLE_COPY(STData)
};

#endif // STDATA_H

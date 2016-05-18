#ifndef GENEDATA_H
#define GENEDATA_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QColor>

// This class contains the GeneRendererGL visual
// data containers and it presents an easy interface
// to add/remove/update data
// OpenGL buffers are configured automatically
// The main principle is that the data points are stored
// as triangles but they are represented as quads

class GeneData
{

public:
    GeneData();
    ~GeneData();

    // clear data and geometry arrays
    void clearData();

    // adds a new data point to the arrays (returns the new index of the element)
    int addQuad(const float x,
                const float y,
                const float size = 1.0,
                const QColor &color = Qt::white);

    // update geometry and rendering data
    void updateQuadSize(const int index, const float x, const float y, const float size);
    void updateQuadColor(const int index, const QColor &newcolor);
    void updateQuadSelected(const int index, const bool selected);
    void updateQuadVisible(const int index, const bool visible);
    void updateQuadReads(const int index, const unsigned reads);

    // some getters
    QColor quadColor(const int index) const;
    bool quadSelected(const int index) const;
    bool quadVisible(const int index) const;
    unsigned quadReads(const int index) const;

    // set selected array to all false
    void clearSelectionArray();

    // OpenGL data arrays
    QVector<QVector3D> m_vertices;
    QVector<QVector2D> m_textures;
    QVector<QVector4D> m_colors;
    QVector<unsigned> m_indexes;
    // Implicit converstion here
    // but the rendering data is being
    // refactored. Computation of colorr
    // will not be performed in the shaders..
    QVector<float> m_reads;
    QVector<float> m_visible;
    QVector<float> m_selected;

    /* NEW RENDERING DATA MODEL
    // the spots numered from 1 to num_spots
    std::vector<unsigned> indexes;
    // gene index is the position of the index in the list of counts for each spot
    // not to confuxed to the index of a spot. The gene index is just the position of the count
    std::unordered_map<QString, unsigned> gene_index;
    // a simple map from spot coordinate (x,y) to spot index
    std::unordered_map<std::pair<unsigned, unsigned>, unsigned> spot_index;
    // vector of vectors
    std::vector<std::vector<unsigned> > index_counts;

    std::vector<unsigned> getCountsGene(const QString &geneName) const;
    std::vector<unsigned> getCountsSpot(const std::pair<unsigned, unsigned> spot) const;
    std::vector<unsigned> getAccumulatedCounts() const;
    unsigned getCount(const QString &geneName, const std::pair<unsigned, unsigned> spot) const;
*/
    Q_DISABLE_COPY(GeneData)
};

#endif // GENEDATA_H

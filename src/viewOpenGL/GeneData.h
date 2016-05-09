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
    void updateQuadReads(const int index, const int reads);
    void updateQuadGenes(const int index, const int genes);

    // some getters
    QColor quadColor(const int index) const;
    bool quadSelected(const int index) const;
    bool quadVisible(const int index) const;
    int quadReads(const int index) const;
    int quadGenes(const int index) const;

    // set selected array to all false
    void clearSelectionArray();

    // data arrays
    QVector<QVector3D> m_vertices;
    QVector<QVector2D> m_textures;
    QVector<QVector4D> m_colors;
    QVector<unsigned int> m_indexes;
    QVector<float> m_reads;
    QVector<float> m_visible;
    QVector<float> m_selected;

    Q_DISABLE_COPY(GeneData)
};

#endif // GENEDATA_H

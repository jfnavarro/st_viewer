#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H
/*
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "math/QuadTree.h"
#include "SelectionEvent.h"
#include "SettingsVisual.h"

#include <unordered_set>
#include <unordered_map>

#include "GraphicItemGL.h"

// To allow to use std::shared_ptr in Qt containers
template<typename T>
inline uint qHash(const std::shared_ptr<T> &key, uint seed = 0)
{
    return qHash(key.get(), seed);
}

// Gene renderer is what renders the genes/features on the CellGLView canvas.
// It uses data arrays (GeneData) to render trough shaders.
// It has some attributes and variables changeable by slots.
// It also allows to select indexes(spots) trough manual selection or gene names
// To clarify, by index(spot) we mean the physical spot in the array
// and by feature we mean the gene-index combination

//TODO data and API will be split into two and the data containers will
//be optimized a lot
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    // must start by 0 as they are used to populate a combobox
    enum GeneShape { Circle = 0, Cross = 1, Square = 2 };

    // different visualization modes
    enum GeneVisualMode { NormalMode = 1, DynamicRangeMode = 2, HeatMapMode = 3 };

    // Visualization data
    // TODO this approach to store and visualization data will be refactored soon
    // visualization data will be stored and encapsulated in an object and it will
    // have a much more memory/speed efficient way to access and compute the
    // visualization data.

    // list of unique spot indexes
    // Qt containers are faster than STL containers
    typedef QSet<int> IndexesList;
    // Spot index to list of features (gene-spot)
    typedef QMultiHash<int, DataProxy::FeaturePtr> FeaturesByIndexMap;
    // Gene object to list of features (accross all spots)
    typedef QHash<DataProxy::GenePtr, std::vector<int> > FeaturesByGeneMap;
    // gene object to list of spot indexes
    typedef QMultiHash<DataProxy::GenePtr, int> IndexesByGeneMap;
    // spot index to total reads/genes
    typedef QHash<int, int> IndexTotalCount;
    // lookup quadtree type (spot indexes)
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    GeneRendererGL(QSharedPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create visualization data from the ST data present in dataProxy)
    void generateData();

    // This function computes a individual counts cutoff for each gene.
    // Spots whose gene's count is below the cut off will not be included.
    // The computation is performed for each gene by looking at the distribution
    // of reads and taking the point where the is a drastic change in the
    // distribution
    void compuateGenesCutoff();

    // clears data containers and reset variables to default
    void clearData();

    // set the dimensions of the bounding rect, also for the QuadTree
    void setDimensions(const QRectF &border);

    // makes a selection of spots given a list of genes (always account for the tresholds)
    void selectGenes(const DataProxy::GeneList &genes);

    // returns the currently selected features (counts on each selected spot)
    const DataProxy::FeatureList &getSelectedFeatures() const;

    // some getters for the thresholds
    int getMinReadsThreshold() const;
    int getMaxReadsThreshold() const;
    int getMinGenesThreshold() const;
    int getMaxGenesThreshold() const;
    int getMinTotalReadsThreshold() const;
    int getMaxTotalReadsThreshold() const;

public slots:

    // TODO slots should have the prefix "slot"

    // slots to change visual atttributes
    void setIntensity(float intensity);
    void setSize(float size);
    void setShape(const GeneShape &shape);

    // slots for the thresholds
    void setReadsLowerLimit(const int limit);
    void setReadsUpperLimit(const int limit);
    void setGenesLowerLimit(const int limit);
    void setGenesUpperLimit(const int limit);
    void setTotalReadsLowerLimit(const int limit);
    void setTotalReadsUpperLimit(const int limit);

    // slots to set visual modes and color computations modes
    void setVisualMode(const GeneVisualMode &mode);
    void setPoolingMode(const Visual::GenePooledMode &mode);
    void setColorComputingMode(const Visual::GeneColorMode &mode);

    // for the given genes list updates the color
    // of all the spots whose genes are in the list and visible
    // (always account for the tresholds)
    void updateColor(const DataProxy::GeneList &geneList);

    // for the given gene list see all its features to visible
    // according if the gene is selected or not
    // (always account for the tresholds)
    void updateVisible(const DataProxy::GeneList &geneList);

    // the user has changed the cut off value of a gene so we
    // should update the visual componets for the spots that contain that gene
    void updateGene(const DataProxy::GenePtr gene);

    // to disable/enable the individual genes cut-off
    void slotSetGenesCutOff(bool enable);

    // clear all the selected features and send a signal to notify
    void clearSelection();

signals:
    // to notify the gene selections model that a selection has been made
    void selectionUpdated();

protected:
    // Make a selections based on an area (box)
    void setSelectionArea(const SelectionEvent *event) override;
    // override method that returns the drawing size of this element
    const QRectF boundingRect() const override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // helper functions to test whether a feature is outside the threshold
    // area or not by reads/genes or TPM
    bool featureReadsOutsideRange(const int value);
    bool featureGenesOutsideRange(const int value);
    bool featureTotalReadsOutsideRange(const int value);

    // will iterate all the features to change size
    void updateSize();
    // will call updateVisual over all the unique genes present in all the
    // features
    void updateVisual();
    // will call updateVisual with the indexes that contain genes present in the
    // input
    void updateVisual(const DataProxy::GeneList &geneList);
    // goes trough each index(spot) and computes its rendering values by
    // iterating over all its features. Thresholds are applied too.
    void updateVisual(const IndexesList &indexes);
    // iterates the spots given and selects them to update the list of selected
    // features (spot-gene)
    // only features that are inside threshold will be counted
    // if any feature is selected in a spot then the spot will be in selected
    // status
    // mode can be = new , add or remove
    void selectSpots(const IndexesList &indexes, const SelectionEvent::SelectionMode &mode);

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

    // compiles and loads the shaders
    void setupShaders();

    // lookup data (features respesent counts, a feature = (gene,spot) count
    // index is the OpenGL index
    // just the set of indexes for convenience
    IndexesList m_indexes;
    // lookup data (index -> features)
    FeaturesByIndexMap m_geneInfoByIndex;
    // lookup data (gene -> indexes)
    IndexesByGeneMap m_geneInfoByGene;
    // look up data (gene -> counts)
    FeaturesByGeneMap m_geneInfoByGeneFeatures;
    // list of selected features
    DataProxy::FeatureList m_geneInfoSelectedFeatures;
    // gene look up (index -> total reads)
    IndexTotalCount m_geneInfoTotalReadsIndex;
    // gene look up (index -> total genes)
    IndexTotalCount m_geneInfoTotalGenesIndex;
    // quad tree container (used to find by coordinates)
    GeneInfoQuadTree m_geneInfoQuadTree;

    // visual attributes
    float m_intensity;
    float m_size;
    GeneShape m_shape;

    // threshold limits for gene hits
    int m_thresholdReadsLower;
    int m_thresholdReadsUpper;
    int m_thresholdGenesLower;
    int m_thresholdGenesUpper;
    int m_thresholdTotalReadsLower;
    int m_thresholdTotalReadsUpper;

    // enable/disable genes cutoff
    bool m_genes_cutoff;

    // local pooled min-max for rendering (Adjusted according to what is being
    // rendered)
    int m_localPooledMin;
    int m_localPooledMax;

    // bounding rect area
    QRectF m_border;

    // visual mode
    GeneVisualMode m_visualMode;

    // pooling mode (by gene count or reads counts or tpm counts)
    Visual::GenePooledMode m_poolingMode;

    // color computing mode (exp - log - linear)
    Visual::GeneColorMode m_colorComputingMode;

    // to know if the rendering data is ready
    bool m_isInitialized;

    // reference to dataProxy
    QSharedPointer<DataProxy> m_dataProxy;

    // OpenGL rendering variables
    GeneData m_geneData;
    QOpenGLShaderProgram m_shader_program;

    Q_DISABLE_COPY(GeneRendererGL)
};
*/
#endif // GENERENDERERGL_H

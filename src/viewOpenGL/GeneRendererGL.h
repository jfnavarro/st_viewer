#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "math/QuadTree.h"
#include "SelectionEvent.h"
#include "GeneData.h"
#include "data/DataProxy.h"
#include "SettingsVisual.h"

#include <unordered_set>
#include <unordered_map>

#include "GraphicItemGL.h"

// Gene renderer is what renders the genes/features on the CellGLView canvas.
// It uses data arrays (GeneData) to render trough shaders.
// It has some attributes and variables changeable by slots.
// It also allows to select indexes(spots) trough manual selection or gene names
// To clarify, by index(spot) we mean the physical spot in the array
// and by feature we mean the gene-index combination
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:
    // if user want to visualize read counts or number of genes or TPM read counts
    enum GenePooledMode { PoolReadsCount = 1, PoolNumberGenes = 2, PoolTPMs = 3 };

    // must start by 0 as they are used to populate a combobox
    enum GeneShape { Circle = 0, Cross = 1, Square = 2 };

    // different visualization modes
    enum GeneVisualMode { NormalMode = 1, DynamicRangeMode = 2, HeatMapMode = 3 };

    // Visualization data
    // TODO this approach to store and visualization data will be refactored soon
    // visualization data will be stored and encapsulated in an object and it will
    // have a much more memory/speed efficient way to access and compute the
    // visualization data

    // list of unique spot indexes
    typedef std::unordered_set<int> IndexesList;
    // Spot index to list of features (gene-spot)
    typedef std::unordered_multimap<int, DataProxy::FeaturePtr> GeneInfoByIndexMap;
    // gene object to list of spot indexes
    typedef std::unordered_multimap<DataProxy::GenePtr, int> GeneInfoByGeneMap;
    // gene object to list of features
    typedef std::unordered_multimap<DataProxy::GenePtr, DataProxy::FeaturePtr> GeneInfoByFeatureMap;
    // feature object to spot index
    typedef std::unordered_map<DataProxy::FeaturePtr, int> GeneInfoByFeatureIndexMap;
    // list of features
    typedef std::vector<DataProxy::FeaturePtr> GeneInfoSelectedFeatures;
    // spot index to total reads/genes
    typedef std::unordered_map<int, unsigned> GeneInfoFeatureCount;
    // lookup quadtree type (spot indexes)
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    GeneRendererGL(QSharedPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create data arrays from the features data in async ways)
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

    // makes a selection of features given a list of genes
    void selectGenes(const DataProxy::GeneList &genes);

    // returns the currently selected features
    const DataProxy::FeatureList &getSelectedFeatures() const;

    // some getters for the thresholds
    unsigned getMinReadsThreshold() const;
    unsigned getMaxReadsThreshold() const;
    unsigned getMinGenesThreshold() const;
    unsigned getMaxGenesThreshold() const;
    unsigned getMinTotalReadsThreshold() const;
    unsigned getMaxTotalReadsThreshold() const;

public slots:

    // TODO slots should have the prefix "slot"

    // slots to change visual atttributes
    void setIntensity(float intensity);
    void setSize(float size);
    void setShape(const GeneShape shape);

    // slots for the thresholds
    void setReadsLowerLimit(const unsigned limit);
    void setReadsUpperLimit(const unsigned limit);
    void setGenesLowerLimit(const unsigned limit);
    void setGenesUpperLimit(const unsigned limit);
    void setTotalReadsLowerLimit(const unsigned limit);
    void setTotalReadsUpperLimit(const unsigned limit);

    // slots to set visual modes and color computations modes
    void setVisualMode(const GeneVisualMode mode);
    void setPoolingMode(const GenePooledMode mode);
    void setColorComputingMode(const Visual::GeneColorMode mode);

    // for the given genes list updates the color
    // of all the featuers whose genes are in the list and visible
    // gene data must be initialized
    void updateColor(const DataProxy::GeneList &geneList);

    // for the given gene list see all its features to visible
    // according if the gene is selected or not
    // gene data must be initialized
    void updateVisible(const DataProxy::GeneList &geneList);

    // the user has changed the cut off value of a gene so we
    // should update the visual componets for the spots that contain that gene
    void updateGene(const DataProxy::GenePtr gene);

    // to disable/enable the individual genes cut-off
    void slotSetGenesCutOff(bool enable);

    // clear all the selected features and notify observers
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

    // helper functions to init OpenGL buffers
    void initBasicBuffers();
    void initDynamicBuffers();

    // helper functions to test whether a feature is outside the threshold
    // area or not by reads/genes or TPM
    bool featureReadsOutsideRange(const unsigned value);
    bool featureGenesOutsideRange(const unsigned value);
    bool featureTotalReadsOutsideRange(const unsigned value);

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

    // gene lookup data (indexes)
    IndexesList m_indexes;
    // gene lookup data (index -> features)
    GeneInfoByIndexMap m_geneInfoByIndex;
    // gene lookup data (gene -> indexes)
    GeneInfoByGeneMap m_geneInfoByGene;
    // gene lookup data (gene -> features)
    GeneInfoByFeatureMap m_geneInfoByFeature;
    // gene look up data (feature -> index)
    GeneInfoByFeatureIndexMap m_geneInfoByFeatureIndex;
    // list of selected features
    GeneInfoSelectedFeatures m_geneInfoSelectedFeatures;
    // gene look up (index -> total reads)
    GeneInfoFeatureCount m_geneInfoTotalReadsIndex;
    // gene look up (index -> total genes)
    GeneInfoFeatureCount m_geneInfoTotalGenesIndex;
    // quad tree container (used to find by coordinates)
    GeneInfoQuadTree m_geneInfoQuadTree;

    // visual attributes
    float m_intensity;
    float m_size;
    GeneShape m_shape;

    // threshold limits for gene hits
    unsigned m_thresholdReadsLower;
    unsigned m_thresholdReadsUpper;
    unsigned m_thresholdGenesLower;
    unsigned m_thresholdGenesUpper;
    unsigned m_thresholdTotalReadsLower;
    unsigned m_thresholdTotalReadsUpper;

    // enable/disable genes cutoff
    bool m_genes_cutoff;

    // local pooled min-max for rendering (Adjusted according to what is being
    // rendered)
    unsigned m_localPooledMin;
    unsigned m_localPooledMax;

    // bounding rect area
    QRectF m_border;

    // visual mode
    GeneVisualMode m_visualMode;

    // pooling mode (by gene count or reads counts)
    GenePooledMode m_poolingMode;

    // color computing mode (exp - log - linear)
    Visual::GeneColorMode m_colorComputingMode;

    // to know if visual data has changed or initialized
    bool m_isDirtyStaticData;
    bool m_isDirtyDynamicData;
    bool m_isInitialized;

    // reference to dataProxy
    QSharedPointer<DataProxy> m_dataProxy;

    // OpenGL rendering variables
    GeneData m_geneData;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertexsBuffer;
    QOpenGLBuffer m_indexesBuffer;
    QOpenGLBuffer m_texturesBuffer;
    QOpenGLBuffer m_colorsBuffer;
    QOpenGLBuffer m_selectedBuffer;
    QOpenGLBuffer m_visibleBuffer;
    QOpenGLBuffer m_readsBuffer;
    QOpenGLShaderProgram m_shader_program;

    Q_DISABLE_COPY(GeneRendererGL)
};

#endif // GENERENDERERGL_H

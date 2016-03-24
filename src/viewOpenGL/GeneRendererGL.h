#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include "GraphicItemGL.h"

#include <QFuture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "math/QuadTree.h"
#include "SelectionEvent.h"
#include "utils/Utils.h"
#include "GeneData.h"
#include "data/DataProxy.h"

class QOpenGLVertexArrayObject;

// Gene renderer is what renders the genes/features on the CellGLView canvas.
// It uses data arrays (GeneData) to render trough shaders.
// It has some attributes and variables changeable by slots.
// It also allows to select indexes(spots) trough areas or gene names

// To clarify, by index(spot) we mean the physical spot in the array
// and by feature we mean the gene-index combination
// TODO add support for individual gene-threshold and global % threshold
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

    // lookup maps for features
    // TODO the current schema to store the visual data does not seem the most
    // convenient, we should get deeper and optimize this

    // OpenGL index to features
    typedef QMultiHash<int, DataProxy::FeaturePtr> GeneInfoByIndexMap;
    // gene to OpenGL indexes
    typedef QMultiHash<DataProxy::GenePtr, int> GeneInfoByGeneMap;
    // gene to features
    typedef QMultiHash<DataProxy::GenePtr, DataProxy::FeaturePtr> GeneInfoByFeatureMap;
    // feature to OpenGL index
    typedef QHash<DataProxy::FeaturePtr, int> GeneInfoByFeatureIndexMap;
    // list of features
    typedef QList<DataProxy::FeaturePtr> GeneInfoSelectedFeatures;
    // index to total reads/genes
    typedef QHash<int, int> GeneInfoFeatureCount;
    // lookup quadtree type
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    GeneRendererGL(QPointer<DataProxy> dataProxy, QObject* parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create data arrays from the features data in async ways)
    // TODO the data is being parsed before in DataProxy, perhaps we could avoid parsing it twice
    void generateData();
    void generateDataAsync();

    // clears data and reset variables
    void clearData();

    // set the dimensions of the bounding rect, also for the QuadTree
    void setDimensions(const QRectF& border);

    // makes a selection of features given a list of genes
    void selectGenes(const DataProxy::GeneList& genes);

    // returns the currently selected features
    const DataProxy::FeatureList& getSelectedFeatures() const;

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
    void setIntensity(qreal intensity);
    void setSize(qreal size);
    void setShape(const GeneShape shape);

    // slots for the thresholds
    void setReadsLowerLimit(const int limit);
    void setReadsUpperLimit(const int limit);
    void setGenesLowerLimit(const int limit);
    void setGenesUpperLimit(const int limit);
    void setTotalReadsLowerLimit(const int limit);
    void setTotalReadsUpperLimit(const int limit);

    // slots to set visual modes and color computations modes
    void setVisualMode(const GeneVisualMode mode);
    void setPoolingMode(const GenePooledMode mode);
    void setColorComputingMode(const Globals::GeneColorMode mode);

    // for the given genes list updates the color
    // of all the featuers whose genes are in the list and visible
    // gene data must be initialized
    void updateColor(const DataProxy::GeneList& geneList);

    // for the given gene list see all its features to visible
    // according if the gene is selected or not
    // gene data must be initialized
    void updateVisible(const DataProxy::GeneList& geneList);

    // clear all the selected features and notify observers
    void clearSelection();

signals:

    // to notify the gene selections model controller that a selection
    // has been made
    void selectionUpdated();

protected:
    void setSelectionArea(const SelectionEvent* event) override;
    const QRectF boundingRect() const override;

private:
    void doDraw(QOpenGLFunctionsVersion& qopengl_functions) override;

    // helper functions to init OpenGL buffers
    void initBasicBuffers();
    void initDynamicBuffers();

    // helper functions to test whether a feature is outside the threshold
    // area or not by reads/genes or TPM
    bool featureReadsOutsideRange(const int value);
    bool featureGenesOutsideRange(const int value);
    bool featureTotalReadsOutsideRange(const int value);

    // will iterate all the features to change size
    void updateSize();
    // will call updateVisual over all the unique genes present in all the features
    void updateVisual();
    // will call updateVisual with the indexes that contain genes present in the input
    void updateVisual(const DataProxy::GeneList& geneList, const bool forceSelection = false);
    // goes trough each index(spot) and computes its rendering values by
    // iterating over all its features. Thresholds are applied too.
    // forceSelection will make visible features to turn selected
    void updateVisual(const QList<int>& indexes, const bool forceSelection = false);

    // helper function to be used when the user wants to select features using
    // a list of genes
    // gene data must be initialized
    void selectFeatures(const DataProxy::FeatureList& features);

    // reset quad tree to rect size
    void resetQuadTree(const QRectF& rect);

    // compiles and loads the shaders
    void setupShaders();

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
    int m_thresholdReadsLower;
    int m_thresholdReadsUpper;
    int m_thresholdGenesLower;
    int m_thresholdGenesUpper;
    int m_thresholdTotalReadsLower;
    int m_thresholdTotalReadsUpper;

    // local pooled min-max for rendering (Adjusted according to what is being rendered)
    int m_localPooledMin;
    int m_localPooledMax;

    // bounding rect area
    QRectF m_border;

    // visual mode
    GeneVisualMode m_visualMode;

    // pooling mode (by gene count or reads counts)
    GenePooledMode m_poolingMode;

    // color computing mode (exp - log - linear)
    Globals::GeneColorMode m_colorComputingMode;

    // to know if visual data has changed or initialized
    bool m_isDirtyStaticData;
    bool m_isDirtyDynamicData;
    bool m_isInitialized;

    // reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

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

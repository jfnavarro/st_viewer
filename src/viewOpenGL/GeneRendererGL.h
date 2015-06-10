/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
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

//Gene renderer is what renders the genes/features on the canvas.
//It uses data arrays (GeneData) to render trough shaders.
//It has some attributes and variables changeable by slots
//It also allows to select features trough areas or gene names
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    enum GenePooledMode {
        PoolReadsCount = 1,
        PoolNumberGenes = 2,
        PoolTPMs = 3
    };

    //must start by 0 as
    //they are used to populate a combobox
    enum GeneShape  {
        Circle = 0,
        Cross = 1,
        Square = 2
    };

    enum GeneVisualMode {
        NormalMode = 1,
        DynamicRangeMode = 2,
        HeatMapMode = 3
    };

    GeneRendererGL(QPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create data arrays from the features in async ways)
    void generateData();
    void generateDataAsync();

    // clears data and reset variables
    void clearData();

    // set the dimensions of the bounding rect, also for the QuadTree
    void setDimensions(const QRectF &border);

    // makes a selection of features given a list of genes
    void selectGenes(const DataProxy::GeneList& genes);

    // returns the currently selected genes (agregated from features)
    GeneSelection::selectedItemsList getSelectedGenes() const;

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

    //TODO slots should have the prefix "slot"

    //slot to change visual atttributes
    void setIntensity(qreal intensity);
    void setSize(qreal size);
    void setShape(const GeneShape shape);

    //slots for the thresholds
    void setReadsLowerLimit(const int limit);
    void setReadsUpperLimit(const int limit);
    void setGenesLowerLimit(const int limit);
    void setGenesUpperLimit(const int limit);
    void setTotalReadsLowerLimit(const int limit);
    void setTotalReadsUpperLimit(const int limit);

    //slots to set visual modes and color computations modes
    void setVisualMode(const GeneVisualMode mode);
    void setPoolingMode(const GenePooledMode mode);
    void setColorComputingMode(const Globals::GeneColorMode mode);

    //for the given gene list updates the color
    //according to the color of the selected genes
    //gene data must be initialized
    void updateColor(const DataProxy::GeneList &geneList);

    //for the given gene list see all its features to visible
    //according if the gene is selected or not
    //gene data must be initialized
    void updateVisible(const DataProxy::GeneList &geneList);

    //clear all the selected features and notify observers
    void clearSelection();

signals:

    //to notify the gene selections model controller that a selection
    //has been made
    void selectionUpdated();

protected:

    void setSelectionArea(const SelectionEvent *event) override;
    void draw(QOpenGLFunctionsVersion *m_qopengl_functions) override;
    const QRectF boundingRect() const override;

private:

    //helper functions to init OpenGL buffers
    void initBasicBuffers();
    void initDynamicBuffers();

    //helper functions to test whether a feature/index is outside the threshold
    //area or not by reads/genes or TPM
    bool featureReadsOutsideRange(const int value);
    bool featureGenesOutsideRange(const int value);
    bool featureTotalReadsOutsideRange(const int value);

    // will iterate all the features to change size
    void updateSize();
    //will call update visual with all the indexes
    void updateVisual();
    //will call updateVisual with the indexes that contain genes present in the input
    void updateVisual(const DataProxy::GeneList &geneList, const bool forceSelection = false);
    // update visual goes trough each index and computes its rendering values by
    // iterating over all its features. Thresholds are applied too.
    // forceSelection will make visible features to turn selected
    void updateVisual(const QList<int> &indexes, const bool forceSelection = false);

    // helper function to be used when user whan to select features using
    // a list of genes
    // gene data must be initialized
    void selectFeatures(const DataProxy::FeatureList& features);

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

    // compiles and loads the shaders
    void setupShaders();

    // lookup maps for features
    typedef QMultiHash<int, DataProxy::FeaturePtr> GeneInfoByIndexMap; //OpenGL index to features
    typedef QMultiHash<DataProxy::GenePtr, int> GeneInfoByGeneMap; //gene to OpenGL indexes
    typedef QMultiHash<DataProxy::GenePtr, DataProxy::FeaturePtr> GeneInfoByFeatureMap; //gene to features
    typedef QHash<DataProxy::FeaturePtr, int> GeneInfoByFeatureIndexMap; //feature to OpenGL index
    typedef QList<DataProxy::FeaturePtr> GeneInfoSelectedFeatures; // list of features
    typedef QHash<int, int> GeneInfoFeatureCount; //index to total reads/genes
    typedef QuadTree<int, 8> GeneInfoQuadTree; //lookup quadtree type

    // gene lookup data (index -> features)
    GeneInfoByIndexMap m_geneInfoByIndex;
    // gene lookup data (gene -> indexes)
    GeneInfoByGeneMap m_geneIntoByGene;
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
    // quad tree container
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

    //reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

    //OpenGL rendering variables
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

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include "GraphicItemGL.h"

#include <QImage>

#include "math/QuadTree.h"
#include "SelectionEvent.h"
#include "utils/Utils.h"
#include "GeneData.h"
#include "data/DataProxy.h"

class QGLPainter;
class QGLTexture2D;
class QVector2DArray;
class QGLShaderProgramEffect;

//Gene renderer is what renders the genes/features on the canvas.
//It uses data arrays (GeneData) to render trough shaders.
//It has some attributes and variables changeable by slots
//It also allows to select features trough areas or gene names
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    enum GenePooledMode {
        PoolNumberGenes = 0,
        PoolReadsCount = 1,
        PoolTPMs = 2
    };

    enum GeneShape  {
        Circle = 0,
        Cross = 1,
        Square = 2
    };

    enum GeneVisualMode {
        NormalMode = 0,
        DynamicRangeMode = 1,
        HeatMapMode = 2
    };

    GeneRendererGL(QPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create data arrays from the features in async ways)
    void generateData();
    void generateDataAsync();

    // clears data and reset variables
    void clearData();

    //set the dimensions of the bounding rect, also for the QuadTree
    void setDimensions(const QRectF border);

    //makes a selection of features given a list of genes
    void selectGenes(const DataProxy::GeneList& genes);

    //returns the currently selected genes (agregated from features)
    GeneSelection::selectedItemsList getSelectedGenes() const;

    //returns the currently selected features
    const DataProxy::FeatureList& getSelectedFeatures() const;

    // threshold limits pooled and not pooled for feature hits
    void setHitCount(const int min, const int max);

public slots:

    //TODO slots should have the prefix "slot"

    //slot to change visual atttributes
    void setIntensity(qreal intensity);
    void setSize(qreal size);
    void setShape(const GeneShape &shape);

    //slots for the thresholds
    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

    //slots to set visual modes and color computations modes
    void setVisualMode(const GeneVisualMode &mode);
    void setPoolingMode(const GenePooledMode &mode);
    void setColorComputingMode(const Globals::GeneColorMode &mode);

    //for the given gene list updates the color
    //according to the color of the selected genes
    // gene data must be initialized
    void updateColor(DataProxy::GeneList geneList);

    //for the given gene list see all its features to visible
    //according if the gene is selected or not
    // gene data must be initialized
    void updateVisible(DataProxy::GeneList geneList);

    //clear all the selected features and notify observers
    void clearSelection();

signals:

    //to notify the gene selections model controller that a selection
    //has been made
    void selectionUpdated();

    //to notify when the local pooled mix-max have changed
    void localPooledMinChanged(const qreal value);
    void localPooledMaxChanged(const qreal value);

protected:

    void setSelectionArea(const SelectionEvent *event) override;
    void draw(QGLPainter *painter) override;
    const QRectF boundingRect() const override;

private:

    //helper function to test whether a feature is outside the threshold
    //area or not
    bool isFeatureOutsideRange(const int hits);

    // internal rendering functions that alters the rendering data
    // gene data must be initialized
    void updateSize();
    void updateVisual();

    // helper function to be used when user whan to select features using
    // a list of genes
    // gene data must be initialized
    void selectFeatures(const DataProxy::FeatureList& features);

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

    // compiles and loads the shaders
    void setupShaders();

    // lookup maps for features
    typedef QHash<DataProxy::FeaturePtr, int> GeneInfoByIdMap;
    typedef QHash<int, DataProxy::FeaturePtr> GeneInfoReverseMap;
    typedef QList<DataProxy::FeaturePtr> GeneInfoSelectedFeatures;
    // lookup quadtree type
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    // gene visual data
    GeneData m_geneData;
    QPointer<QGLSceneNode> m_geneNode;

    // gene lookup data (feature -> index)
    GeneInfoByIdMap m_geneInfoById;
    // gene lookup data (index -> features)
    //TODO can probably be removed
    GeneInfoReverseMap m_geneInfoReverse;
    // vector of selected features
    GeneInfoSelectedFeatures m_geneInfoSelectedFeatures;
    // quad tree container
    GeneInfoQuadTree m_geneInfoQuadTree;

    // visual attributes
    qreal m_intensity;
    qreal m_size;
    GeneShape m_shape;

    // threshold limits for gene hits
    int m_thresholdLower;
    int m_thresholdUpper;

    // local pooled min-max for rendering (Adjusted according to what is being rendered)
    float m_localPooledMin;
    float m_localPooledMax;

    // total sum of reads for the TPM mode (normalized)
    int m_totalReads;

    // bounding rect area
    QRectF m_border;

    // visual mode
    GeneVisualMode m_visualMode;

    // pooling mode (by gene count or reads counts)
    GenePooledMode m_poolingMode;

    // color computing mode (exp - log - linear)
    Globals::GeneColorMode m_colorComputingMode;

    // shader program (TODO use smart pointer)
    QGLShaderProgramEffect *m_shaderProgram;

    // to know if visual data has changed or initialized
    bool m_isDirty;
    bool m_isInitialized;

    //reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(GeneRendererGL)
};

#endif // GENERENDERERGL_H

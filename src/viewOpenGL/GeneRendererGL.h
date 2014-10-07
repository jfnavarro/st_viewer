/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include "GraphicItemGL.h"

#include "math/QuadTree.h"
#include "SelectionEvent.h"
#include "data/DataProxy.h"
#include "utils/Utils.h"
#include "GeneData.h"
#include <QImage>

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

    GeneRendererGL(QPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create data arrays from the features)
    void generateData();

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

    // stores a local copy of the tissue image in genes cordinate space
    // so we can obtain pixel intensity values when storing selections
    void setImage(const QImage& image);

    // threshold limits pooled and not pooled for feature hits
    void setHitCount(const int min,
                     const int max,
                     const int pooledMin,
                     const int pooledMax);

public slots:

    //TODO slots should have the prefix "slot"

    //slot to change visual atttributes
    void setIntensity(qreal intensity);
    void setSize(qreal size);
    void setShine(qreal shine);
    void setShape(Globals::GeneShape shape);

    void setLowerLimit(int limit);
    void setUpperLimit(int limit);
    void setPooledLowerLimit(int limit);
    void setPooledUpperLimit(int limit);

    void setVisualMode(const Globals::GeneVisualMode &mode);

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

protected:

    void setSelectionArea(const SelectionEvent *event) override;
    void draw(QGLPainter *painter) override;
    const QRectF boundingRect() const override;

private:

    //helper function to test whether a feature is outside the threshold
    //area or not
    bool isFeatureOutsideRange(const int hits, const float totalValue);

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
    qreal m_shine;
    Globals::GeneShape m_shape;

    // cell tissue image (in genes coordinate system)
    QImage m_image;

    // threshold limits for gene hits
    int m_thresholdLower;
    int m_thresholdUpper;
    int m_thresholdLowerPooled;
    int m_thresholdUpperPooled;

    // local statistics (Adjusted according to what is being rendered)
    float m_localPooledMin;
    float m_localPooledMax;

    // bounding rect area
    QRectF m_border;

    // visual mode
    Globals::GeneVisualMode m_visualMode;

    // shader program (TODO use smart pointer)
    QGLShaderProgramEffect *m_shaderProgram;

    // to know if visual data has changed
    bool m_isDirty;

    //reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(GeneRendererGL)
};


#endif // GENERENDERERGL_H

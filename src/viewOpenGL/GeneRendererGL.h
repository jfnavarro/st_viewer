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

//TODO add better comments to class and functions
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit GeneRendererGL(QPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builders
    void generateData();
    void clearData();

    //set the dimensions of the bounding rect, also for the QuadTree
    void setDimensions(const QRectF border);

    //selection functions
    void selectGenes(const DataProxy::GeneList&);
    void selectFeatures(const DataProxy::FeatureList&);

    //returns the currently selected genes
    const GeneSelection::selectedItemsList getSelectedIItems() const;

    //sets the local copy of the cell tissue image
    //be aware the local transformation matrix has been assigned
    void setImage(const QImage& image);

    // threshold limits pooled and not pooled for feature hits
    void setHitCount(int min, int max, int pooledMin, int pooledMax);

public slots:

    void setIntensity(qreal intensity);
    void setSize(qreal size);
    void setShine(qreal shine);
    void setShape(Globals::GeneShape shape);
    void setLowerLimit(int limit);
    void setUpperLimit(int limit);
    void setVisualMode(const Globals::GeneVisualMode &mode);

    //for the given gene list updates the color
    //according to the color of the selected genes
    void updateColor(DataProxy::GeneList geneList);

    //for the given gene list see all its features to visible
    //according if the gene is selected or not
    void updateVisible(DataProxy::GeneList geneList);

    //clear all the selected features
    void clearSelection();

signals:

    void selectionUpdated();

protected:

    void setSelectionArea(const SelectionEvent *event) override;
    void draw(QGLPainter *painter) override;
    const QRectF boundingRect() const override;

private:

    // internal rendering functions
    void updateSize();
    void updateVisual();

    // helper function to update all features selected and color attributes
    void updateFeaturesSelected(bool selected);
    void updateFeaturesColor(QColor color);

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

    // compiles and loads the shaders
    void setupShaders();

    // lookup maps
    typedef QHash<DataProxy::FeaturePtr, int> GeneInfoByIdMap;
    typedef QHash<int, DataProxy::FeaturePtr> GeneInfoReverseMap;

    // lookup quadtree
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    // gene visual data
    GeneData m_geneData;
    QPointer<QGLSceneNode> m_geneNode;

    // gene lookup data
    GeneInfoByIdMap m_geneInfoById;
    GeneInfoReverseMap m_geneInfoReverse; //TODO can probably be removed
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

    // dataset Statistics
    int m_min;
    int m_max;
    int m_pooledMin;
    int m_pooledMax;

    // bounding rect area
    QRectF m_border;

    // visual mode
    Globals::GeneVisualMode m_visualMode;

    // shader program (TODO use smart pointer)
    QGLShaderProgramEffect *m_shaderProgram;

    // tells if something has changed
    bool m_isDirty;

    //reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

    //max value of pixel intensity used to normalize
    int m_maxPixelIntensity;

    Q_DISABLE_COPY(GeneRendererGL)
};


#endif // GENERENDERERGL_H

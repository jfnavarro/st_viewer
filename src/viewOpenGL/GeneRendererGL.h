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

class ColorScheme;
class QGLPainter;
class QGLTexture2D;
class QVector2DArray;
class QGLShaderProgramEffect;

class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    enum updateOptions {
        geneColor = 1,
        geneVisual = 2,
        geneSize = 3,
        geneSelection = 4,
        geneThreshold = 5
    };

    explicit GeneRendererGL(QObject *parent = 0);
    virtual ~GeneRendererGL();

    //rendering functions
    void generateData();
    //void updateData(updateOptions flags);
    void rebuildData();
    void clearData();

    void setDimensions(const QRectF &border);

    //selection functions
    void selectGenes(const DataProxy::GeneList&);
    void selectFeatures(const DataProxy::FeatureList&);
    void setSelectionArea(const SelectionEvent *event);
    void clearSelection();

    //getters
    DataProxy::FeatureListPtr getSelectedFeatures();

public slots:

    void setIntensity(qreal intensity);
    void setSize(qreal size);
    void setShine(qreal shine);

    void setShape(Globals::GeneShape shape);

    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

    void setVisualMode(const Globals::GeneVisualMode &mode);

    void setHitCount(int min, int max, int pooledMin, int pooledMax);

    void updateColor(DataProxy::GenePtr);
    void updateSelection(DataProxy::GenePtr);

protected:

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

    //internal rendering functions
    //void updateGene(DataProxy::GenePtr, updateOptions flags);
    //void updateFeatures(DataProxy::FeatureListPtr, updateOptions flags);

    void updateSize();
    void updateVisual();

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    const QRectF boundingRect() const;

    void setupShaders();

private:
    // lookup maps
    typedef QHash<DataProxy::FeaturePtr, int> GeneInfoByIdMap;
    typedef QHash<int, DataProxy::FeaturePtr> GeneInfoReverseMap;

    // lookup quadtree
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    // selection set
    typedef QSet<int> GeneInfoSelectedSet;

    // gene visual data
    GeneData m_geneData;
    QGLSceneNode *m_geneNode;

    // gene lookup data
    GeneInfoByIdMap m_geneInfoById;
    GeneInfoReverseMap m_geneInfoReverse;
    GeneInfoQuadTree m_geneInfoQuadTree;

    // gene selection data
    GeneInfoSelectedSet m_geneInfoSelection;

    // visual attributes
    qreal m_intensity;
    qreal m_size;
    qreal m_shine;
    Globals::GeneShape m_shape;

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

    QRectF m_border;

    // color scheme
    ColorScheme *m_colorScheme = nullptr;

    // visual mode
    Globals::GeneVisualMode m_visualMode;

    // shader programs
    QGLShaderProgramEffect *shaderCircle;
    QGLShaderProgramEffect *shaderRectangle;
    QGLShaderProgramEffect *shaderCross;
};


#endif // GENERENDERERGL_H

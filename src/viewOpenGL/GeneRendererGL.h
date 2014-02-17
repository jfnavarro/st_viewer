/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include <QGLSceneNode>

#include "math/QuadTree.h"
#include "SelectionEvent.h"
#include "data/DataProxy.h"
#include "utils/Utils.h"

#include <qglnamespace.h>

class ColorScheme;
class QGLPainter;
class QGLTexture2D;
class QVector2DArray;
class QGeometryData;
class QGLShaderProgramEffect;


class GeneRendererGL : public QGLSceneNode
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
    void clearData();

    // variables
    void setAlignmentMatrix(const QTransform &transform);
    const QTransform& alignmentMatrix() const;

    void setDimensions(const QRectF &border, const QRectF &rect);

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

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
    //void setShine(qreal shine);

    void setLowerLimit(int limit);
    void setUpperLimit(int limit);

    void setThresholdMode(const Globals::GeneThresholdMode &mode);
    void setVisualMode(const Globals::GeneVisualMode &mode);

    void setHitCount(int min, int max, int pooledMin, int pooledMax);

protected:

    //internal rendering functions
    //void updateGene(DataProxy::GenePtr, updateOptions flags);
    //void updateFeatures(DataProxy::FeatureListPtr, updateOptions flags);
    void updateColor(DataProxy::FeatureListPtr);
    void updateSelection(DataProxy::FeatureListPtr);
    void updateSize();
    void updateVisual(DataProxy::FeatureListPtr);

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    int addQuad(qreal x, qreal y, QColor4ub color = Qt::white);
    void updateQuadSize(const int index, qreal x, qreal y);
    void updateQuadColor(const int index, QColor4ub color);

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
    QGeometryData m_geneData;
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

    // threshold limits for gene hits
    int m_thresholdLower;
    int m_thresholdUpper;

    // dataset Statistics
    int m_min;
    int m_max;
    int m_pooledMin;
    int m_pooledMax;

    // color scheme
    ColorScheme *m_colorScheme = nullptr;

    // visual mode
    Globals::GeneVisualMode m_visualMode;

    // threshold mode
    Globals::GeneThresholdMode m_thresholdMode;

    // shader programs
    QGLShaderProgramEffect *shaderCircle;
    QGLShaderProgramEffect *shaderRectangle;
    QGLShaderProgramEffect *shaderCross;

    // alignment matrix transformation
    QTransform m_transform;
};


#endif // GENERENDERERGL_H

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include "GLCommon.h"
#include "data/GLElementDataGene.h"
#include "data/GLElementShapeFactory.h"
#include "math/GLQuadTree.h"
#include "SelectionEvent.h"
#include "controller/data/DataProxy.h"
#include "utils/Utils.h"

class ColorScheme;

class GeneRendererGL
{
public:

    enum updateOptions {
        geneColor,
        geneVisible,
        geneSize,
        All = (geneColor | geneVisible | geneSize)
    };

    GeneRendererGL();
    virtual ~GeneRendererGL();

    //rendering functions
    void generateData();
    void updateData(updateOptions flags);
    void clearData();
    void rebuildData();

    //update functions
    void updateGene(DataProxy::GenePtr, updateOptions flags);
    //void updateGenes(DataProxy::GeneListPtr);
    //void updateFeature(DataProxy::FeaturePtr);
    void updateFeatures(DataProxy::FeatureListPtr, updateOptions flags);
    void resetQuadTree(const QRectF &rect);

    //selection functions
    void selectGenes(const DataProxy::GeneList&);
    void selectFeatures(const DataProxy::FeatureList&);
    void setSelectionArea(const SelectionEvent *event);
    void clearSelection();

    //hit count limits
    void setHitCount(int min, int max, int sum);

    //getters
    DataProxy::FeatureListPtr getSelectedFeatures();

    inline const qreal intensity() const { return m_intensity; }
    inline const qreal size() const { return m_size; }
    inline GL::GLElementDataGene getData() { return m_geneData; }
    inline const Globals::ThresholdMode thresholdMode() const { return m_thresholdMode; }
    inline const int lowerLimit() const { return m_geneLowerLimit; }
    inline const int upperLimit() const { return m_geneUpperLimit; }
    inline const Globals::VisualMode& visualMode() const { return m_visualMode; }

    //setters
    inline void setIntensity(qreal intensity) { m_intensity = intensity; }
    inline void setSize(qreal size) { m_size = size;}
    inline void setThresholdMode(const Globals::ThresholdMode &mode) { m_thresholdMode = mode; }
    inline void setLowerLimit(int geneLimit) { m_geneLowerLimit = geneLimit; setVisualMode(Globals::NormalMode); }
    inline void setUpperLimit(int geneLimit) { m_geneUpperLimit = geneLimit; setVisualMode(Globals::NormalMode); }
    void setVisualMode(const Globals::VisualMode &mode);

private:
    // lookup maps
    typedef QHash<DataProxy::FeaturePtr, GL::GLindex> GeneInfoByIdMap;
    typedef QHash<GL::GLindex, DataProxy::FeaturePtr> GeneInfoReverseMap;
    // lookup quadtree
    typedef GL::GLQuadTree<GL::GLindex, 8> GeneInfoQuadTree;
    // selection set
    typedef QSet<GL::GLindex> GeneInfoSelectedSet;

    // gene visual data
    GL::GLElementDataGene  m_geneData;
    // gene lookup data
    GeneInfoByIdMap m_geneInfoById;
    GeneInfoReverseMap m_geneInfoReverse;
    GeneInfoQuadTree m_geneInfoQuadTree;
    // gene selection data
    GeneInfoSelectedSet m_geneInfoSelection;
    //atributes
    qreal m_intensity;
    qreal m_size;
    // upper && lower thresholds
    int m_geneLowerLimit;
    int m_geneUpperLimit;
    // hit count limits
    int m_min;
    int m_max;
    int m_sum;
    // color scheme
    ColorScheme *m_colorScheme;
    // visual mode
    Globals::VisualMode m_visualMode;
    // threshold mode
    Globals::ThresholdMode m_thresholdMode;
};

#endif // GENERENDERERGL_H

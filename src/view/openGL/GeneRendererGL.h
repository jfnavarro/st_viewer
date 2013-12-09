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
        geneColor = 0x01,
        geneVisual = 0x02,
        geneSize = 0x04,
        geneSelection = 0x08,
        geneThreshold = 0x012,
        geneFilter = (geneSelection | geneThreshold)
    };

    GeneRendererGL();
    virtual ~GeneRendererGL();

    //rendering functions
    void generateData();
    void updateData(updateOptions flags);
    void clearData();
    void rebuildData();
    void updateGene(DataProxy::GenePtr, updateOptions flags);
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
    inline const GL::GLElementDataGene& getData() const { return m_geneData; }

    DataProxy::FeatureListPtr getSelectedFeatures();

    inline qreal intensity() const { return m_intensity; }
    inline qreal size() const { return m_size; }

    inline int lowerLimit() const { return m_geneLowerLimit; }
    inline int upperLimit() const { return m_geneUpperLimit; }

    inline const Globals::VisualMode& visualMode() const { return m_visualMode; }
    inline const Globals::ThresholdMode& thresholdMode() const { return m_thresholdMode; }

    //setters
    void setIntensity(qreal intensity);
    void setSize(qreal size);

    inline void setLowerLimit(int geneLimit) { m_geneLowerLimit = geneLimit; }
    inline void setUpperLimit(int geneLimit) { m_geneUpperLimit = geneLimit; }

    void setThresholdMode(const Globals::ThresholdMode &mode);
    void setVisualMode(const Globals::VisualMode &mode);

protected:

    //internal rendering functions
    void updateColor(DataProxy::FeatureListPtr);
    void updateSelection(DataProxy::FeatureListPtr);
    void updateSize(DataProxy::FeatureListPtr);
    void updateVisual(DataProxy::FeatureListPtr);

private:
    // lookup maps
    typedef QHash<DataProxy::FeaturePtr, GL::GLindex> GeneInfoByIdMap;
    typedef QHash<GL::GLindex, DataProxy::FeaturePtr> GeneInfoReverseMap;

    // lookup quadtree
    typedef GL::GLQuadTree<GL::GLindex, 8> GeneInfoQuadTree;

    // selection set
    typedef QSet<GL::GLindex> GeneInfoSelectedSet;

    // gene visual data
    GL::GLElementDataGene m_geneData;

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

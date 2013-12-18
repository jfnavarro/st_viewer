/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include "controller/data/DataProxy.h"
#include "model/core/ColorScheme.h"
#include "model/core/DynamicRangeColor.h"
#include "model/core/FeatureColor.h"
#include "model/core/HeatMapColor.h"

#include "data/GLElementRectangleFactory.h"
#include "qgl.h"
#include "GLCommon.h"
#include "math/GLFloat.h"

GeneRendererGL::GeneRendererGL() : m_colorScheme(0)
{
    clearData();
}

GeneRendererGL::~GeneRendererGL()
{
    if (m_colorScheme) {
        delete m_colorScheme;
    }
    m_colorScheme = 0;
}

void GeneRendererGL::clearData()
{
    // gene plot data
    m_geneData.clear();

    // selected genes data
    m_geneInfoSelection.clear();

    // lookup data
    m_geneInfoById.clear();
    m_geneInfoReverse.clear();

    // quad tree
    //m_geneInfoQuadTree.clear();

    // variables
    m_intensity = Globals::gene_intensity;
    m_size = Globals::gene_size;
    m_thresholdMode = Globals::IndividualGeneMode;
    m_min = Globals::gene_lower_limit;
    m_max = Globals::gene_upper_limit;
    m_sum = Globals::gene_upper_limit;

    // allocate color scheme
    setVisualMode(Globals::NormalMode);
}

void GeneRendererGL::resetQuadTree(const QRectF &rect)
{
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(GL::toGLaabb(rect));
}

void GeneRendererGL::setHitCount(int min, int max, int sum)
{
    if ((m_hitCountMin != min) ||
        (m_hitCountMax != max) ||
        (m_hitCountSum != sum)) {

        m_hitCountMin = min;
        m_hitCountMax = max;
        m_hitCountSum = sum;

        m_min = min;
        m_max = max;
        m_sum = sum;

        // update gene data
        m_geneData.setHitCount(min,max,sum);
    }
}

void GeneRendererGL::setIntensity(qreal intensity)
{
    m_intensity = intensity;
    // update gene data
    m_geneData.setIntensity(intensity);
}

void GeneRendererGL::setSize(qreal size)
{
    m_size = size;
    // update factory
    GL::GLElementRectangleFactory factory(m_geneData);
    factory.setSize((GLfloat) m_size);
}

void GeneRendererGL::setUpperLimit(int limit)
{   
    const int adjusted_limit = static_cast<int>( (qreal(limit) /
                                                  qreal(Globals::gene_threshold_max - Globals::gene_threshold_min) ) *
                                                 qreal(m_hitCountMax - m_hitCountMin));
    if ( m_max != adjusted_limit ) {
        m_max = adjusted_limit;
        m_geneData.setUpperLimit(limit);
        //setTotalSum(GeneRendererGL::recomputeSum(m_min, m_max));
    }
}

void GeneRendererGL::setLowerLimit(int limit)
{
    const int adjusted_limit = static_cast<int>( (qreal(limit) /
                                                  qreal(Globals::gene_threshold_max - Globals::gene_threshold_min) ) *
                                                 qreal(m_hitCountMax - m_hitCountMin));
    if ( m_min != adjusted_limit ) {
        m_min = adjusted_limit;
        m_geneData.setLowerLimit(limit);
        //setTotalSum(GeneRendererGL::recomputeSum(m_min, m_max));
    }
}

/*void GeneRendererGL::setTotalSum(int sum)
{
    if ( m_sum != sum ) {
        m_sum = sum;
        m_geneData.setTotalSum(sum);
    }
}*/

/*int GeneRendererGL::recomputeSum(int min, int max)
{
    int new_sum = 0;
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());
    foreach(const DataProxy::FeaturePtr feature, (*features)) {
        if ( feature->hits() >= min && feature->hits() <= max ) {
            new_sum += feature->hits();
        }
    }
    return new_sum;
}*/

void GeneRendererGL::generateData()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());

    const GL::GLflag flags =
            GL::GLElementShapeFactory::AutoAddColor |
            GL::GLElementShapeFactory::AutoAddTexture |
            GL::GLElementShapeFactory::AutoAddOption;
    GL::GLElementRectangleFactory factory(m_geneData, flags);

    factory.setSize((GLfloat) m_size);

    // generate geometry
    foreach(const DataProxy::FeaturePtr feature, (*features)) {

        // feature cordinates
        const GL::GLpoint point = GL::toGLpoint(feature->x(), feature->y());

        // test if point already exists
        GeneInfoQuadTree::PointItem item = {point, GL::INVALID_INDEX};
        m_geneInfoQuadTree.select(point, item);

        // this will be the index of the new point (if it does not exists) or the present point
        GL::GLindex index = -1;

        //if it exists
        if (item.second != GL::INVALID_INDEX) {
            index = item.second;
            m_geneInfoById.insert(feature, index);
            m_geneInfoReverse.insertMulti(index, feature);
        }
        // else insert point and create the link
        else {
            index = factory.addShape(point);
            m_geneInfoById.insert(feature, index);
            m_geneInfoReverse.insert(index, feature);
            m_geneInfoQuadTree.insert(point, index);
            m_geneData.addFeatCount(1u);
            m_geneData.addValue(0u); // we initialize to 0
            m_geneData.addRefCount(0u); // we initialize to 0
        }

        m_geneData.setFeatCount(index, m_geneData.getFeatCount(index) + 1);
        if (m_geneData.getFeatCount(index) > 1) {
            // set complex option
            const GL::GLoption option = factory.getOption(index);
            factory.setOption(index, GL::bitSet(option, 1));
        }

        // set default color
        factory.setColor(index, GL::GLcolor(GL::White));

    } //endforeach
}

void GeneRendererGL::updateData(updateOptions flags)
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());
    //early out
    if (m_geneData.isEmpty() || features.isNull()) {
        return;
    }
    updateFeatures(features,flags);
}

void GeneRendererGL::updateGene(DataProxy::GenePtr gene, updateOptions flags)
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name());
    //early out
    if (m_geneData.isEmpty() || features.isNull()) {
        return;
    }
    updateFeatures(features,flags);
}

void GeneRendererGL::updateFeatures(DataProxy::FeatureListPtr features, updateOptions flags)
{
    //TODO replace for a switch

    if ( flags == geneVisual || flags == geneThreshold ) {
        updateVisual(features);
    }
    else if (flags == geneColor) {
        updateColor(features);
    }
    else if ( flags == geneSelection ) {
        updateSelection(features);
    }
    else if (flags == geneSize) {
        updateSize(features);
    }
    else {
        qDebug () << "GeneRendererGL : Error, unrecognized update mode";
    }
}

void GeneRendererGL::updateSize(DataProxy::FeatureListPtr features)
{
    GL::GLElementRectangleFactory factory(m_geneData);
    // update features
    foreach(DataProxy::FeaturePtr feature, *(features)){
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature);
        Q_ASSERT(it != m_geneInfoById.end());
        // easy access
        const GL::GLindex index = it.value();
        const GLfloat x = feature->x();
        const GLfloat y = feature->y();
        factory.setShape(index, GL::GLrectangle(x, y, m_size));
    }
}

void GeneRendererGL::updateColor(DataProxy::FeatureListPtr features)
{
    GL::GLElementRectangleFactory factory(m_geneData);
    DataProxy *dataProxy = DataProxy::getInstance();

    foreach(DataProxy::FeaturePtr feature, *(features)) {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature);
        Q_ASSERT(it != m_geneInfoById.end());

        //TODO gene should be passed as parameter (no need for this each iteration)
        DataProxy::FeaturePtr first_feature = features->first();
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), first_feature->gene());
        const bool selected = gene->selected();
        const QColor geneQColor = gene->color();

        const GL::GLindex index = it.value();
        const int refCount = m_geneData.getRefCount(index);

        // feature update color
        const QColor oldQColor = m_colorScheme->getColor(feature, m_min, m_max);
        QColor newQColor = oldQColor;

        if (feature->color() != geneQColor) {
            feature->color(geneQColor);
            newQColor = m_colorScheme->getColor(feature, m_min, m_max);
        }
        
        // convert to OpenGL colors
        const GL::GLcolor oldColor = GL::toGLcolor(oldQColor);
        const GL::GLcolor newColor = GL::toGLcolor(newQColor);
        
        // update the color if gene is visible
        if (selected && (refCount > 0)) {
            GL::GLcolor color = factory.getColor(index);
            if (refCount > 1) {
                color = GL::invlerp((1.0f / GLfloat(refCount)), color, oldColor);
            }
            color = GL::lerp((1.0f / GLfloat(refCount)), color, newColor);
            factory.setColor(index, color);
        }
    }
}

void GeneRendererGL::updateSelection(DataProxy::FeatureListPtr features)
{
    GL::GLElementRectangleFactory factory(m_geneData);
    DataProxy *dataProxy = DataProxy::getInstance();

    // iterate features
    foreach(DataProxy::FeaturePtr feature, *(features)) {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature);
        Q_ASSERT(it != m_geneInfoById.end());

        //TODO gene should be passed as parameter (no need for this each iteration)
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        bool selected = gene->selected();
        const GL::GLindex index = it.value();

        // update ref count
        const int oldRefCount = m_geneData.getRefCount(index);
        int newRefCount = (oldRefCount + (selected ? 1 : -1));
        m_geneData.setRefCount(index, newRefCount);

        // update values
        const int currentHits = feature->hits();
        const int oldValue = m_geneData.getValue(index);
        const int newValue = (oldValue + (selected ? currentHits : -currentHits));
        m_geneData.setValue(index, newValue);

        // update color
        if (newRefCount != 0) {
            GL::GLcolor featureColor = GL::toGLcolor(m_colorScheme->getColor(feature, m_min, m_max));
            if ( ( m_thresholdMode == Globals::IndividualGeneMode &&
                   (newValue < m_min || newValue > m_max) ) ) {
                featureColor.alpha = 0.0f;
            }
            GL::GLcolor color = factory.getColor(index);
            color = (selected) ?
                        GL::lerp((1.0f / GLfloat(newRefCount)), color, featureColor) :
                        GL::invlerp((1.0f / GLfloat(oldRefCount)), color, featureColor);
            factory.setColor(index, color);
        }

        // update visible
        if (selected && (newRefCount == 1)) {
            // toggle visible on and add index
            factory.connect(index);
        }
        else if ( !selected && (newRefCount == 0)) {
            // toggle visible off and add index
            factory.deconnect(index);
        }
    }
}

void GeneRendererGL::updateVisual(DataProxy::FeatureListPtr features)
{
    GL::GLElementRectangleFactory factory(m_geneData);
    DataProxy *dataProxy = DataProxy::getInstance();

    // reset ref count when in visual mode
    m_geneData.resetRefCount();
    m_geneData.resetValCount();

    // iterate features
    foreach(DataProxy::FeaturePtr feature, *(features)) {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature);
        Q_ASSERT(it != m_geneInfoById.end());

        // easy access
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        bool selected = gene->selected();
        const GL::GLindex index = it.value();

        // update ref count
        const int oldRefCount = m_geneData.getRefCount(index);
        const int newRefCount = (oldRefCount + (selected ? 1 : 0));
        m_geneData.setRefCount(index, newRefCount);

        // update values
        const int currentHits = feature->hits();
        const int oldValue = m_geneData.getValue(index);
        const int newValue = (oldValue + (selected ? currentHits : 0));
        m_geneData.setValue(index, newValue);

        // update color
        if (selected && (newRefCount > 0)) {
            GL::GLcolor featureColor = GL::toGLcolor(m_colorScheme->getColor(feature, m_min, m_max));
            if (  ( m_thresholdMode == Globals::IndividualGeneMode &&
                  (newValue < m_min || newValue > m_max) )  ) {
                featureColor.alpha = 0.0f;
            }
            GL::GLcolor color = factory.getColor(index);
            color = GL::lerp((1.0f / GLfloat(newRefCount)), color, featureColor);
            factory.setColor(index, color);
        }
    }
}

void GeneRendererGL::rebuildData()
{
    // clear data
    m_geneData.clear(GL::GLElementData::Arrays & ~GL::GLElementData::IndexArray);
    m_geneData.setMode(GL_QUADS);
    generateData();
}

void GeneRendererGL::clearSelection()
{
    GL::GLElementRectangleFactory factory(m_geneData);
    foreach(GL::GLindex index, m_geneInfoSelection) {
        const GL::GLoption option = factory.getOption(index);
        factory.setOption(index, GL::bitClear(option, 0));
    }
    m_geneInfoSelection.clear();
}

DataProxy::FeatureListPtr GeneRendererGL::getSelectedFeatures()
{
    DataProxy::FeatureListPtr featureList = DataProxy::FeatureListPtr(new DataProxy::FeatureList);
    foreach(const GL::GLindex index, m_geneInfoSelection) {
        GeneInfoReverseMap::iterator it = m_geneInfoReverse.find(index), end = m_geneInfoReverse.end();
        // hash map represent one to many connection. iterate over all matches.
        for (; (it != end) && (it.key() == index); ++it) {
            featureList->append(it.value());
        }
    }
    return featureList;
}

void GeneRendererGL::selectGenes(const DataProxy::GeneList &geneList)
{
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::FeatureList aggregateFeatureList;
    foreach(DataProxy::GenePtr gene, geneList) {
        aggregateFeatureList << *(dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name()));
    }
    selectFeatures(aggregateFeatureList);
}

void GeneRendererGL::selectFeatures(const DataProxy::FeatureList &featureList)
{
    //create factory
    GL::GLElementRectangleFactory factory(m_geneData);
    // unselect previous selecetion
    foreach(const GL::GLindex index, m_geneInfoSelection) {
        const GL::GLoption option = factory.getOption(index);
        factory.setOption(index, GL::bitClear(option, 0));
    }
    m_geneInfoSelection.clear();
    //select all
    foreach(const DataProxy::FeaturePtr feature, featureList) {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature), end = m_geneInfoById.end();
        if (it == end) {
            continue;
        }
        const GL::GLindex index = it.value();
        const int refCount = m_geneData.getRefCount(index);
        const GL::GLoption option = factory.getOption(index);
        // do not select non-visible features (check tresholds and visible too)
        if (refCount <= 0) {
            continue;
        }
        m_geneInfoSelection.insert(index);
        factory.setOption(index, GL::bitSet(option, 0));
    }
}

void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    //get selection area
    QRectF rect = event->path().boundingRect();
    GL::GLaabb aabb = GL::toGLaabb(rect);
    // init gene factory
    GL::GLElementRectangleFactory factory(m_geneData);
    //clear selection
    SelectionEvent::SelectionMode mode = event->mode();
    if (mode == SelectionEvent::NewSelection) {
        // unselect previous selecetion
        foreach(GL::GLindex index, m_geneInfoSelection) {
            const GL::GLoption option = factory.getOption(index);
            factory.setOption(index, GL::bitClear(option, 0));
        }
        m_geneInfoSelection.clear();
    }
    // get selected genes
    GeneInfoQuadTree::PointItemList list;
    m_geneInfoQuadTree.select(aabb, list);
    // select new selecetion
    GeneInfoQuadTree::PointItemList::const_iterator it, end = list.end();
    for (it = list.begin(); it != end; ++it) {
        GL::GLindex index = it->second;
        const int refCount = m_geneData.getRefCount(index);
        const GL::GLoption option = factory.getOption(index);
        // do not select non-visible features
        if (refCount <= 0) {
            continue;
        }
        // make the selection
        if (mode == SelectionEvent::ExcludeSelection) {
            m_geneInfoSelection.remove(index);
            factory.setOption(index, GL::bitClear(option, 0));
        } else {
            m_geneInfoSelection.insert(index);
            factory.setOption(index, GL::bitSet(option, 0));
        }
    }
}

void GeneRendererGL::setVisualMode(const Globals::VisualMode &mode)
{
    // update visual mode
    m_visualMode = mode;

    // set new color scheme deleting old if needed
    if (m_colorScheme) {
        delete m_colorScheme;
    }

    // update color scheme
    switch (m_visualMode) {
    case Globals::DynamicRangeMode:
        m_colorScheme = new DynamicRangeColor();
        break;
    case Globals::HeatMapMode:
        m_colorScheme = new HeatMapColor();
        break;
    case Globals::NormalMode:
    default:
        m_colorScheme = new FeatureColor();
        break;
    }
    // update geneData
    m_geneData.setColorMode(m_visualMode);
}

void GeneRendererGL::setThresholdMode(const Globals::ThresholdMode &mode)
{
    m_thresholdMode = mode;
    m_geneData.setThresholdMode(m_thresholdMode);
}

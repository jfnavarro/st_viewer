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
#include "model/core/HeatmapColor.h"

#include "qgl.h"
#include "GLCommon.h"
#include "math/GLFloat.h"

GeneRendererGL::GeneRendererGL() : m_colorScheme(0)
{

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
    m_geneInfoQuadTree.clear();
    // variables
    m_intensity = Globals::gene_intensity;
    m_size = Globals::gene_size;
    m_geneLowerLimit = Globals::gene_lower_limit;
    m_geneUpperLimit = Globals::gene_upper_limit;
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
    m_min = min;
    m_max = max;
    m_sum = sum;
    setVisualMode(m_visualMode);
}

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
        const GL::GLpoint point = GL::toGLpoint(feature->x(), feature->y());
        // test if point already exists
        GeneInfoQuadTree::PointItem item = {point, (-1u)};
        m_geneInfoQuadTree.select(point, item);
        //if it exists
        if (item.second != -1) {
            const GL::GLindex index = item.second;
            m_geneInfoById.insert(feature, index);
            m_geneInfoReverse.insertMulti(index, feature);
            m_geneData.setRefCount(index, m_geneData.getRefCount(index) + 1);
            m_geneData.setValue(index, m_geneData.getValue(index) + feature->hits());
            //m_geneData.setVisible(index, 0);
            if (m_geneData.getRefCount(index) > 1) {
                // set complex option
                const GL::GLoption option = factory.getOption(index);
                factory.setOption(index, GL::bitSet(option, 1));
            }
        }
        // else insert point and create the link
        else {
            const GL::GLindex index = factory.addShape(point).index();
            m_geneInfoById.insert(feature, index);
            m_geneInfoReverse.insert(index, feature);
            m_geneInfoQuadTree.insert(point, index);
            m_geneData.addRefCount(0u);
            m_geneData.addValue(feature->hits());
            m_geneData.addVisible(0u);
            // set default color
            factory.setColor(index, GL::GLcolor(GL::White));
        }
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

void GeneRendererGL::updateFeatures(DataProxy::FeatureListPtr features, updateOptions flags)
{
    // create factory
    GL::GLElementRectangleFactory factory(m_geneData);
    factory.setSize((GLfloat) m_size);
    // dataproxy instance
    DataProxy* dataProxy = DataProxy::getInstance();
    // iterate features
    foreach(DataProxy::FeaturePtr feature, *(features)) {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature);
        Q_ASSERT(it != m_geneInfoById.end());
        //pass if hits are below/above limits
        if (feature->hits() < m_geneLowerLimit || feature->hits() > m_geneUpperLimit) {
            continue;
        }
        // easy access
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        const bool selected = gene->selected();
        const GL::GLindex index = it.value();
        // bump ref count
        int oldRefCount = m_geneData.getRefCount(index);
        const int newRefCount = (oldRefCount + (selected ? 1 : -1));
        m_geneData.setRefCount(index,newRefCount);
        // update color
        GL::GLcolor color(GL::White);
        if (newRefCount > 0) {
            //NOTE in order to derive color we do a weighted addition or
            //     subtraction of feature color
            const GL::GLcolor featureColor = GL::toGLcolor(m_colorScheme->getColor(feature));
            //size
            const int x = feature->x();
            const int y = feature->y();
            factory.setShape(index, GL::GLrectangle(x, y, m_size));
            //color
            color = factory.getColor(index);
            color = (selected) ?
                    GL::lerp((1.0f / GLfloat(newRefCount)), color, featureColor) :
                    GL::invlerp((1.0f / GLfloat(oldRefCount)), color, featureColor);
            color.alpha *= m_intensity; // set alpha
        }

        factory.setColor(index, color);
        // toggle visible
        if (selected && (newRefCount == 1)) {
            // add index
            factory.connect(index);
        }
        // if toggle invisible
        else if (!selected && (newRefCount == 0)) {
            factory.deconnect(index);
        }
    }
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
    const int max = m_thresholdMode == Globals::IndividualGeneMode ? m_max : m_sum;
    const int min = m_min;
    switch (m_visualMode) {
    case Globals::DynamicRangeMode:
        m_colorScheme = new DynamicRangeColor(min, max);
        break;
    case Globals::HeatMapMode:
        m_colorScheme = new HeatMapColor(min, max);
        break;
    case Globals::NormalMode:
    default:
        m_colorScheme = new FeatureColor(min, max);
        break;
    }
}

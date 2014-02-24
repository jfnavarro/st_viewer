/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include "utils/DebugHelper.h"

#include "data/DataProxy.h"
#include "color/ColorScheme.h"
#include "color/DynamicRangeColor.h"
#include "color/FeatureColor.h"
#include "color/HeatMapColor.h"

#include <QGLBuilder>
#include <QGLShaderProgramEffect>
#include <QFile>

static const int INVALID_INDEX = -1;

GeneRendererGL::GeneRendererGL(QObject *parent)
    : GraphicItemGL(parent)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, true);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, true);

    m_geneNode = new QGLSceneNode;

    clearData();
    setupShaders();
}

GeneRendererGL::~GeneRendererGL()
{
    if (m_colorScheme) {
        delete m_colorScheme;
    }
    m_colorScheme = 0;

    if (m_geneNode) {
        delete m_geneNode;
    }
    m_geneNode = 0;
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

    // variables
    m_intensity = Globals::GENE_INTENSITY_DEFAULT;
    m_size = Globals::GENE_SIZE_DEFAULT;
    m_shine = Globals::GENE_SHINNE_DEFAULT;
    m_min = 0;
    m_thresholdLower = 0;
    m_pooledMin = 0;
    m_thresholdLowerPooled = 0;
    m_max = 1;
    m_thresholdUpper = 1;
    m_pooledMax = 1;
    m_thresholdUpperPooled = 1;

    // allocate color scheme
    setVisualMode(Globals::NormalMode);
}

void GeneRendererGL::resetQuadTree(const QRectF &rect)
{
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(rect);
}

void GeneRendererGL::setHitCount(int min, int max, int pooledMin, int pooledMax)
{
    m_min = min;
    m_thresholdLower = min;
    m_pooledMin = pooledMin;
    m_thresholdLowerPooled = pooledMin;
    m_max = max;
    m_thresholdUpper = max;
    m_pooledMax = pooledMax;
    m_thresholdUpperPooled = pooledMax;
}

void GeneRendererGL::setIntensity(qreal intensity)
{
    if ( m_intensity != intensity) {
        m_intensity = intensity;
        emit updated();
    }
}

void GeneRendererGL::setSize(qreal size)
{
    if ( m_size != size ) {
        m_size = size;
        updateSize();
        emit updated();
    }
}

void GeneRendererGL::setShine(qreal shine)
{
    if ( m_shine != shine ) {
        m_shine = shine;
        emit updated();
    }
}

void GeneRendererGL::setUpperLimit(int limit)
{   
    static const qreal offlimit = Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (qreal(limit) / offlimit ) * range;
    const qreal range_pooled = m_pooledMax - m_pooledMin;
    const qreal adjusted_limit_pooled =  (qreal(limit) / offlimit ) * range_pooled;

    if ( m_thresholdUpper != adjusted_limit ) {
        m_thresholdUpper = adjusted_limit;
        m_thresholdUpperPooled = adjusted_limit_pooled;
        updateVisual();
    }
}

void GeneRendererGL::setLowerLimit(int limit)
{
    static const qreal offlimit = Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (qreal(limit) / offlimit ) * range;
    const qreal range_pooled = m_pooledMax - m_pooledMin;
    const qreal adjusted_limit_pooled =  (qreal(limit) / offlimit ) * range_pooled;

    if ( m_thresholdLower != adjusted_limit ) {
        m_thresholdLower = adjusted_limit;
        m_thresholdLowerPooled = adjusted_limit_pooled;
        updateVisual();
    }
}

void GeneRendererGL::generateData()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());

    Q_ASSERT(m_geneData.count() == 0);

    foreach(const DataProxy::FeaturePtr feature, (*features)) {

        Q_ASSERT(feature != 0);

        // feature cordinates
        const QPointF point(feature->x(), feature->y());

        // test if point already exists
        GeneInfoQuadTree::PointItem item = { point, INVALID_INDEX };
        m_geneInfoQuadTree.select(point, item);

        //if it exists
        if (item.second != INVALID_INDEX) {
            const int index = item.second;
            m_geneInfoById.insert(feature, index); // same position = same feature = same index
            m_geneInfoReverse.insertMulti(index, feature); //multiple features per index
        }
        // else insert point and create the link
        else {
            // create quad and add it to the data and return first index
            const int index = m_geneData.addQuad(feature->x(), feature->y(), m_size, Qt::white);
            // update look up containers
            m_geneInfoById.insert(feature, index);
            m_geneInfoReverse.insert(index, feature);
            m_geneInfoQuadTree.insert(point, index);
        }

    } //endforeach
}

void GeneRendererGL::updateSize()
{
    GeneInfoByIdMap::const_iterator it = m_geneInfoById.begin();
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    for( ; it != end; ++it) {
        // update size of the shape
        const int index = it.value();
        DataProxy::FeaturePtr feature = it.key();
        Q_ASSERT(feature);
        m_geneData.updateQuadSize(index, feature->x(), feature->y(), m_size);
    }
}

void GeneRendererGL::updateColor(DataProxy::GenePtr gene)
{
    if ( m_geneInfoById.empty() ) {
        return;
    }
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name());

    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();

    const bool selected = gene->selected();

    foreach(DataProxy::FeaturePtr feature, *(features)) {

        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        const QColor geneQColor = gene->color();
        const int index = it.value();
        const int refCount = m_geneData.quadRefCount(index);

        // feature update color
        const QColor oldQColor = m_colorScheme->getColor(feature, m_thresholdLower, m_thresholdUpper);
        QColor newQColor = oldQColor;

        if (feature->color() != geneQColor) {
            feature->color(geneQColor);
            newQColor = m_colorScheme->getColor(feature, m_min, m_max);
        }
        
        // convert to OpenGL colors
        const QColor4ub oldColor = QColor4ub(oldQColor);
        const QColor4ub newColor = QColor4ub(newQColor);
        
        // update the color if gene is visible
        if (selected && (refCount > 0)) {
            QColor4ub color = m_geneData.quadColor(index);
            if (refCount > 1) {
                color = STMath::invlerp((1.0f / qreal(refCount)), color, oldColor);
            }
            color = STMath::lerp((1.0f / qreal(refCount)), color, newColor);
            m_geneData.updateQuadColor(index, color);
        }
    }

    //TODO this causes a error when calling this method many times
    // due to over call to the render function
    //emit updated();
}

void GeneRendererGL::updateSelection(DataProxy::GenePtr gene)
{
    if ( m_geneInfoById.empty() ) {
        return;
    }
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name());

    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();

    const bool selected = gene->selected();

    foreach(DataProxy::FeaturePtr feature, *(features)) {

        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        const int index = it.value();
        const int currentHits = feature->hits();

        // update values
        const int oldValue = m_geneData.quadValue(index);
        const int newValue = (oldValue + (selected ? currentHits : -currentHits));
        m_geneData.updateQuadValue(index, newValue);

        // update ref count
        const int oldRefCount = m_geneData.quadRefCount(index);
        int newRefCount = (oldRefCount + (selected ? 1 : -1));
        bool offlimits =  (m_visualMode == Globals::NormalMode
                           && ( currentHits < m_thresholdLower || currentHits > m_thresholdUpper ) );
        if ( selected && offlimits ) {
                newRefCount = oldRefCount;
        }
        m_geneData.updateQuadRefCount(index, newRefCount);

        if ( newRefCount > 0 ) {
            QColor4ub featureColor = QColor4ub(m_colorScheme->getColor(feature, m_thresholdLower, m_thresholdUpper));
            QColor4ub color = m_geneData.quadColor(index);
            color = (selected && !offlimits) ?
                        STMath::lerp((1.0f / qreal(newRefCount)), color, featureColor) :
                        STMath::invlerp((1.0f / qreal(oldRefCount)), color, featureColor);
            m_geneData.updateQuadColor(index, color);
        }

        // update visible
        if ( selected && newRefCount == 1 ) {
            m_geneData.updateQuadVisible(index, true);
        }
        else if ( !selected && newRefCount == 0 ) {
            m_geneData.updateQuadVisible(index, false);
        }
    }

    //TODO this causes a error when calling this method many times
    // due to over call to the render function
    //emit updated();
}

void GeneRendererGL::updateVisual()
{
    if ( m_geneInfoById.empty() ) {
        return;
    }
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());

    // reset ref count and values when in visual mode
    m_geneData.resetRefCount();
    m_geneData.resetValues();

    GeneInfoByIdMap::const_iterator it = m_geneInfoById.begin();
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();

    foreach(DataProxy::FeaturePtr feature, *(features)) {

        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        // easy access
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        Q_ASSERT(gene);

        bool selected = gene->selected();
        const int index = it.value();
        const int currentHits = feature->hits();

        // update values
        const int oldValue = m_geneData.quadValue(index);
        const int newValue = (oldValue + (selected ? currentHits : 0));
        m_geneData.updateQuadValue(index, newValue);

        // update ref count
        const int oldRefCount = m_geneData.quadRefCount(index);
        int newRefCount = (oldRefCount + (selected ? 1 : 0));
        bool offlimits =  (m_visualMode == Globals::NormalMode
                           && ( currentHits < m_thresholdLower || currentHits > m_thresholdUpper ) );
        if ( selected && offlimits ) {
                newRefCount = oldRefCount;
        }
        m_geneData.updateQuadRefCount(index, newRefCount);

        // update color
        if ( selected && newRefCount > 0 && !offlimits) {
            QColor4ub featureColor = QColor4ub(m_colorScheme->getColor(feature, m_thresholdLower, m_thresholdUpper));
            QColor4ub color = m_geneData.quadColor(index);
            color = STMath::lerp((1.0f / qreal(newRefCount)), color, featureColor);
            m_geneData.updateQuadColor(index, color);
        }

        if ( newRefCount == 0 ) {
            QColor4ub featureColor = QColor4ub(Qt::white);
            featureColor.setAlphaF(0.0);
            m_geneData.updateQuadColor(index, featureColor);
        }
    }

    //TODO this causes a error when calling this method many times
    // due to over call to the render function
    //emit updated();
}

void GeneRendererGL::rebuildData()
{
    // clear data and regenerate
    m_geneData.clear();
    generateData();
}

void GeneRendererGL::clearSelection()
{
    foreach(const int index, m_geneInfoSelection) {
        m_geneData.updateQuadSelected(index, false);
    }
    m_geneInfoSelection.clear();
}

DataProxy::FeatureListPtr GeneRendererGL::getSelectedFeatures()
{
    DataProxy::FeatureListPtr featureList = DataProxy::FeatureListPtr(new DataProxy::FeatureList);
    GeneInfoReverseMap::const_iterator it;
    GeneInfoReverseMap::const_iterator end = m_geneInfoReverse.end();
    foreach(const int index, m_geneInfoSelection) {
        // hash map represent one to many connection. iterate over all matches.
        for (it = m_geneInfoReverse.find(index); (it != end) && (it.key() == index); ++it) {
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
        aggregateFeatureList <<
                                *(dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name()));
    }
    selectFeatures(aggregateFeatureList);
}

void GeneRendererGL::selectFeatures(const DataProxy::FeatureList &featureList)
{
    Q_UNUSED(featureList);

    /*
    // unselect previous selecetion
    foreach(const GL::GLindex index, m_geneInfoSelection) {
        m_geneData.setOption(index, 0u);
    }
    m_geneInfoSelection.clear();

    //select all
    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    foreach(const DataProxy::FeaturePtr feature, featureList) {

        it =  m_geneInfoById.find(feature);
        if (it == end) {
            continue;
        }

        const GL::GLindex index = it.value();
        const int refCount = m_geneData.getRefCount(index);
        //const int option = m_geneData.getOption(index);
        const int value = m_geneData.getValue(index);

        // do not select non-visible features or outside threshold (global mode)
        if (refCount <= 0
                || (m_thresholdMode == Globals::GlobalGeneMode
                    && (value < m_min_local || value > m_max_local) ) ) {
            continue;
        }

        // make the selection
        m_geneInfoSelection.insert(index);
        m_geneData.setOption(index, 1);
    }
    */
}

void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    Q_UNUSED(event);

    /*
    //get selection area
    QRectF rect = event->path().boundingRect();
    GL::GLaabb aabb(rect);

    //clear selection
    SelectionEvent::SelectionMode mode = event->mode();
    if (mode == SelectionEvent::NewSelection) {
        // unselect previous selecetion
        foreach(GL::GLindex index, m_geneInfoSelection) {
            m_geneData.setOption(index, 0u);
        }
        m_geneInfoSelection.clear();
    }

    // get selected genes
    GeneInfoQuadTree::PointItemList list;
    m_geneInfoQuadTree.select(aabb, list);

    // select new selecetion
    GeneInfoQuadTree::PointItemList::const_iterator it;
    GeneInfoQuadTree::PointItemList::const_iterator end = list.end();
    for (it = list.begin(); it != end; ++it) {

        const GL::GLindex index = it->second;
        const int refCount = m_geneData.getRefCount(index);
        //const int option = factory.getOption(index);
        const int value = m_geneData.getValue(index);

        // do not select non-visible features or outside threshold (global mode)
        if (refCount <= 0
                || (m_thresholdMode == Globals::GlobalGeneMode
                    && (value < m_min_local || value > m_max_local) ) ) {
            continue;
        }

        // make the selection
        if (mode == SelectionEvent::ExcludeSelection) {
            m_geneInfoSelection.remove(index);
            m_geneData.setOption(index, 0u);
        } else {
            m_geneInfoSelection.insert(index);
            m_geneData.setOption(index, 1u);
        }
    }
    */
}

void GeneRendererGL::setVisualMode(const Globals::GeneVisualMode &mode)
{
    // update visual mode
    m_visualMode = mode;

    // set new color scheme deleting old if needed
    if (m_colorScheme) {
        delete m_colorScheme;
    }
    m_colorScheme = 0;

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

   // update the gene colors
   updateVisual();
}

void GeneRendererGL::draw(QGLPainter *painter)
{   
    Q_ASSERT(m_geneNode);

    //TODO add material to node to control shine

    //QGLMaterial *material = new QGLMaterial(this);
    //material->setColor(Qt::red);
    //material->setAmbientColor(Qt::red);
    //material->setDiffuseColor(Qt::red);
    //m_geneNode->setMaterial(material);

    //TODO add a dirty variable to this all the time
    m_geneNode->setGeometry(m_geneData);
    m_geneNode->setCount(m_geneData.indices().size());
    m_geneNode->setUserEffect(shaderCircle);
    shaderCircle->update(painter, QGLPainter::UpdateAll);
    shaderCircle->setActive(painter, true);
    m_geneNode->draw(painter);
    shaderCircle->setActive(painter, false);
}

void GeneRendererGL::drawGeometry(QGLPainter *painter)
{
    Q_UNUSED(painter);
}

void GeneRendererGL::setupShaders()
{
    shaderCircle = new QGLShaderProgramEffect();
    //shaderRectangle = new QGLShaderProgramEffect();
    //shaderCross = new QGLShaderProgramEffect();

    //shaderRectangle->setVertexShaderFromFile(":shader/geneSquare.vert");
    //shaderRectangle->setFragmentShaderFromFile(":shader/geneSquare.frag");

    shaderCircle->setFragmentShaderFromFile(":shader/geneCircle.frag");
    shaderCircle->setVertexShaderFromFile(":shader/geneCircle.vert");

    //shaderCross->setFragmentShaderFromFile( ":shader/geneCross.frag");
    //shaderCross->setVertexShaderFromFile(":shader/geneCross.vert");
}

void GeneRendererGL::setDimensions(const QRectF &border)
{
    m_border = border;
    // reflect bounds to quad tree
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(QuadTreeAABB(border));
}

const QRectF GeneRendererGL::boundingRect() const
{
    return m_border;
}

void GeneRendererGL::setShape(Globals::GeneShape shape)
{
    //const Globals::GeneShape shape = static_cast<Globals::GeneShape>(geneShape);
    if ( m_shape != shape ) {
        m_shape = shape;
        emit updated();
    }
}

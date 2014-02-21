/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include "data/DataProxy.h"
#include "color/ColorScheme.h"
#include "color/DynamicRangeColor.h"
#include "color/FeatureColor.h"
#include "color/HeatMapColor.h"

#include <QGeometryData>
#include <QGLBuilder>
#include <QGLShaderProgramEffect>
#include <QFile>

static const int INVALID_INDEX = -1;
//static const QGL::VertexAttribute valuesVertex = QGL::CustomVertex0;
//static const QGL::VertexAttribute refCountVertex = QGL::CustomVertex1;
static const QGL::VertexAttribute selectionVertex = QGL::CustomVertex1;
static const QGL::VertexAttribute visibleVertex = QGL::CustomVertex0;

GeneRendererGL::GeneRendererGL(QObject *parent)
    : GraphicItemGL(parent)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, true);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);

    clearData();
    setupShaders();
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

    // variables
    m_intensity = Globals::GENE_INTENSITY_DEFAULT;
    m_size = Globals::GENE_SIZE_DEFAULT;
    m_shine = Globals::GENE_SHINNE_DEFAULT;

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
    m_max = max;
    m_pooledMin = pooledMin;
    m_pooledMax = pooledMax;
    emit updated();
}

void GeneRendererGL::setIntensity(qreal intensity)
{
    m_intensity = intensity;
    emit updated();
}

void GeneRendererGL::setSize(qreal size)
{
    m_size = size;
    updateSize();
    emit updated();
}

void GeneRendererGL::setShine(qreal shine)
{
    m_shine = shine;
    emit updated();
}

void GeneRendererGL::setUpperLimit(int limit)
{   
    Q_UNUSED(limit);
 /*   const qreal upper_offset = qreal(Globals::gene_threshold_max - Globals::gene_threshold_min);
    // limit ranks 0 - 100, I normalize it to my rank of hit_max - hit_min
    const int adjusted_limit = static_cast<int>( ( qreal(limit) / upper_offset ) *
                                                 qreal(m_hitCountMax - m_hitCountMin) );

    const int adjusted_limit_local = static_cast<int>( ( qreal(limit) / upper_offset ) *
                                                 qreal(m_hitCountLocalMax - m_hitCountLocalMin) );

    if ( m_max != adjusted_limit ) {
        m_max = adjusted_limit;
        m_max_local = adjusted_limit_local;
        // render data only needs local limits
        m_geneData.setUpperLimit(adjusted_limit_local);
    }*/
}

void GeneRendererGL::setLowerLimit(int limit)
{
    Q_UNUSED(limit);
/*    const qreal upper_offset = qreal(Globals::gene_threshold_max - Globals::gene_threshold_min);
    // limit ranks 0 - 100, I normalize it to my rank of hit_max - hit_min
    const int adjusted_limit = static_cast<int>( ( qreal(limit) / upper_offset ) *
                                                 qreal(m_hitCountMax - m_hitCountMin) );

    const int adjusted_limit_local = static_cast<int>( ( qreal(limit) / upper_offset ) *
                                                 qreal(m_hitCountLocalMax - m_hitCountLocalMin) );

    if ( m_min != adjusted_limit ) {
        m_min = adjusted_limit;
        m_min_local = adjusted_limit_local;
        // render data only needs local limits
        m_geneData.setLowerLimit(adjusted_limit_local);
    }*/
}

int GeneRendererGL::addQuad(qreal x, qreal y, QColor4ub color)
{
    static const QVector2D ta(0.0f, 0.0f);
    static const QVector2D tb(0.0f, 1.0f);
    static const QVector2D tc(1.0f, 1.0f);
    static const QVector2D td(1.0f, 0.0f);

    m_geneData.appendVertex( QVector3D(x - m_size / 2.0f , y - m_size / 2.0f, 0.0f) );
    m_geneData.appendVertex( QVector3D(x + m_size / 2.0f, y - m_size / 2.0f, 0.0f) );
    m_geneData.appendVertex( QVector3D(x + m_size / 2.0f, y + m_size / 2.0f, 0.0f) );
    m_geneData.appendVertex( QVector3D(x - m_size / 2.0f, y + m_size / 2.0f, 0.0f) );
    m_geneData.appendTexCoord(ta, tb, tc, td, QGL::TextureCoord0);
    m_geneData.appendColor(color, color, color, color);

    // return first index of the quad created
    return m_geneData.count(QGL::Position);
}

void GeneRendererGL::updateQuadSize(const int index, qreal x, qreal y)
{
    m_geneData.vertex(index) = QVector3D(x - m_size / 2.0f , y - m_size / 2.0f, 0.0f);
    m_geneData.vertex(index + 1) = QVector3D(x + m_size / 2.0f, y - m_size / 2.0f, 0.0f);
    m_geneData.vertex(index + 2) = QVector3D(x + m_size / 2.0f, y + m_size / 2.0f, 0.0f);
    m_geneData.vertex(index + 3) = QVector3D(x - m_size / 2.0f, y + m_size / 2.0f, 0.0f);
}

void GeneRendererGL::updateQuadColor(const int index, QColor4ub color)
{
    m_geneData.color(index) = color;
    m_geneData.color(index + 1) = color;
    m_geneData.color(index + 2) = color;
    m_geneData.color(index + 3) = color;
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
            const int index = addQuad(feature->x(), feature->y());

            // update custom vertex arrays
            //m_geneData.appendAttribute(0.0f, valuesVertex); // we initialize to 0 (it will be fetched afterwards)
            //m_geneData.appendAttribute(0.0f, refCountVertex); // we initialize to 0 (it will be fetched afterwards)
            m_geneData.appendAttribute(0.0f, selectionVertex);  // we initialize to false (it will be fetched afterwards)
            m_geneData.appendAttribute(0.0f, visibleVertex); // we initialize to false (it will be fetched afterwards)

            // update look up containers
            m_geneInfoById.insert(feature, index);
            m_geneInfoReverse.insert(index, feature);
            m_geneInfoQuadTree.insert(point, index);
        }

    } //endforeach

    QGLBuilder builder;
    //data.generateTextureCoordinates();
    builder.addQuads(m_geneData);
    m_geneNode = builder.finalizedSceneNode();
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
        updateQuadSize(index, feature->x(), feature->y());
    }
    QGLBuilder builder;
    //data.generateTextureCoordinates();
    builder.addQuads(m_geneData);
    m_geneNode = builder.finalizedSceneNode();
}

void GeneRendererGL::updateColor(DataProxy::FeatureListPtr features)
{
    Q_UNUSED(features);

    /*
   // GL::GLElementRectangleFactory factory(m_geneData);
    DataProxy *dataProxy = DataProxy::getInstance();

    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();

    foreach(DataProxy::FeaturePtr feature, *(features)) {

        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        //TODO gene should be passed as parameter (no need for this each iteration)
        DataProxy::FeaturePtr first_feature = features->first();
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), first_feature->gene());
        Q_ASSERT(gene != 0);

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
        const QColor4ub oldColor = QColor4ub(oldQColor);
        const QColor4ub newColor = QColor4ub(newQColor);
        
        // update the color if gene is visible
        if (selected && (refCount > 0)) {
            QColor4ub color = factory.getColor(index);
            if (refCount > 1) {
                color = GL::invlerp((1.0f / qreal(refCount)), color, oldColor);
            }
            color = GL::lerp((1.0f / qreal(refCount)), color, newColor);
            factory.setColor(index, color);
        }
    }
    */
}

void GeneRendererGL::updateSelection(DataProxy::FeatureListPtr features)
{
    Q_UNUSED(features);

    /*
    GL::GLElementRectangleFactory factory(m_geneData);
    DataProxy *dataProxy = DataProxy::getInstance();

    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();

    foreach(DataProxy::FeaturePtr feature, *(features)) {

        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        //TODO gene should be passed as parameter (no need for this each iteration)
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        Q_ASSERT(gene != 0);

        bool selected = gene->selected();
        const GL::GLindex index = it.value();
        const int currentHits = feature->hits();

        // update values
        const int oldValue = m_geneData.getValue(index);
        const int newValue = (oldValue + (selected ? currentHits : -currentHits));
        m_geneData.setValue(index, newValue);

        // update ref count
        const int oldRefCount = m_geneData.getRefCount(index);
        int newRefCount = (oldRefCount + (selected ? 1 : -1));
        bool offlimits =  (m_thresholdMode == Globals::IndividualGeneMode
                           && ( currentHits < m_min || currentHits > m_max ) );
        if ( selected && offlimits ) {
                newRefCount = oldRefCount;
        }
        m_geneData.setRefCount(index, newRefCount);

        if ( newRefCount > 0 ) {
            QColor4ub featureColor = QColor4ub(m_colorScheme->getColor(feature, m_min, m_max));
            QColor4ub color = factory.getColor(index);
            color = (selected && !offlimits) ?
                        GL::lerp((1.0f / qreal(newRefCount)), color, featureColor) :
                        GL::invlerp((1.0f / qreal(oldRefCount)), color, featureColor);
            factory.setColor(index, color);
        }

        // update visible
        if ( selected && newRefCount == 1 ) {
            factory.connect(index);
        }
        else if ( !selected && newRefCount == 0 ) {
            factory.deconnect(index);
        }
    }
    */
}

void GeneRendererGL::updateVisual(DataProxy::FeatureListPtr features)
{
    Q_UNUSED(features);

    /*
    GL::GLElementRectangleFactory factory(m_geneData);
    DataProxy *dataProxy = DataProxy::getInstance();

    // reset ref count when in visual mode
    m_geneData.resetRefCount();
    m_geneData.resetValCount();

    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();

    foreach(DataProxy::FeaturePtr feature, *(features)) {

        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        // easy access
        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        Q_ASSERT(gene != 0);

        bool selected = gene->selected();
        const GL::GLindex index = it.value();
        const int currentHits = feature->hits();

        // update values
        const int oldValue = m_geneData.getValue(index);
        const int newValue = (oldValue + (selected ? currentHits : 0));
        m_geneData.setValue(index, newValue);

        // update ref count
        const int oldRefCount = m_geneData.getRefCount(index);
        int newRefCount = (oldRefCount + (selected ? 1 : 0));
        bool offlimits =  (m_thresholdMode == Globals::IndividualGeneMode
                           && ( currentHits < m_min || currentHits > m_max ) );
        if ( selected && offlimits ) {
                newRefCount = oldRefCount;
        }
        m_geneData.setRefCount(index, newRefCount);

        // update color
        if ( selected && newRefCount > 0 && !offlimits) {
            QColor4ub featureColor = QColor4ub(m_colorScheme->getColor(feature, m_min, m_max));
            QColor4ub color = factory.getColor(index);
            color = GL::lerp((1.0f / qreal(newRefCount)), color, featureColor);
            factory.setColor(index, color);
        }

        if ( newRefCount == 0 ) {
            QColor4ub featureColor = QColor4ub(Qt::white);
            featureColor.setAlphaF(0.0);
            factory.setColor(index, featureColor);
        }
    }
    */
}

//void GeneRendererGL::rebuildData()
//{
    // clear data
    //m_geneData.clear(GL::GLElementDataGene::Arrays & ~GL::GLElementDataGene::IndexArray);
    //m_geneData.setMode(GL_QUADS);
    //generateData();
//}

void GeneRendererGL::clearSelection()
{
    //foreach(GL::GLindex index, m_geneInfoSelection) {
    //    m_geneData.setOption(index, 0u);
    //}
    //m_geneInfoSelection.clear();
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
    // update geneData
    //m_geneData.setColorMode(m_visualMode);
}

void GeneRendererGL::draw(QGLPainter *painter)
{   
    if (m_geneNode == nullptr) {
        return;
    }

    //QGLMaterial *material = new QGLMaterial(this);
    //material->setColor(Qt::red);
    //material->setAmbientColor(Qt::red);
    //material->setDiffuseColor(Qt::red);
    //cube->setMaterial(material);
    //cube->setDrawingMode(QGL::Points);
    //cube->setEffect(QGL::LitMaterial);

    m_geneNode->setUserEffect(shaderCircle);
    //shader->setActive(painter,true);
    m_geneNode->draw(painter);
}

void GeneRendererGL::drawGeometry(QGLPainter *painter)
{
    QGLSceneNode::drawGeometry(painter);
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

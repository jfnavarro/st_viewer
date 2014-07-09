/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include "data/DataProxy.h"
#include "dataModel/GeneSelection.h"

#include <QGLShaderProgramEffect>
#include <QOpenGLShaderProgram>
#include <QGLAttributeValue>
#include <QImageReader>

static const int INVALID_INDEX = -1;

GeneRendererGL::GeneRendererGL(QPointer<DataProxy> dataProxy, QObject *parent)
    : GraphicItemGL(parent),
      m_geneNode(nullptr),
      m_shaderProgram(nullptr),
      m_isDirty(false),
      m_dataProxy(dataProxy)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, true);

    clearData();
}

GeneRendererGL::~GeneRendererGL()
{
    m_geneNode->deleteLater();
    m_geneNode = nullptr;

   if (m_shaderProgram != nullptr) {
       delete m_shaderProgram;
   }
   m_shaderProgram = nullptr;
}

void GeneRendererGL::clearData()
{
    // clear gene plot data
    m_geneData.clearData();

    //clear selection
    updateFeaturesSelected(false);

    //clear colors
    updateFeaturesColor(Globals::DEFAULT_COLOR_GENE);

    // lookup data
    m_geneInfoById.clear();
    m_geneInfoReverse.clear();

    // variables
    m_intensity = Globals::GENE_INTENSITY_DEFAULT;
    m_size = Globals::GENE_SIZE_DEFAULT;
    m_shine = Globals::GENE_SHINNE_DEFAULT;
    m_min = Globals::GENE_THRESHOLD_MIN;
    m_thresholdLower = Globals::GENE_THRESHOLD_MIN;
    m_pooledMin = Globals::GENE_THRESHOLD_MIN;
    m_thresholdLowerPooled = Globals::GENE_THRESHOLD_MIN;
    m_max = Globals::GENE_THRESHOLD_MAX;
    m_thresholdUpper = Globals::GENE_THRESHOLD_MAX;
    m_pooledMax = Globals::GENE_THRESHOLD_MAX;
    m_thresholdUpperPooled = Globals::GENE_THRESHOLD_MAX;
    m_shape = Globals::DEFAULT_SHAPE_GENE;

    // update visual mode
    m_visualMode = Globals::NormalMode;

    //reset scene node
    if (!m_geneNode.isNull()) {
        m_geneNode->deleteLater();
        m_geneNode = nullptr;
    }
    m_geneNode = new QGLSceneNode();

    //update shader
    setupShaders();

    m_isDirty = true;
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
    m_isDirty = true;
}

void GeneRendererGL::setIntensity(qreal intensity)
{
    if (m_intensity != intensity) {
        m_intensity = intensity;
        m_isDirty = true;
        emit updated();
    }
}

void GeneRendererGL::setSize(qreal size)
{
    if (m_size != size) {
        m_size = size;
        updateSize();
    }
}

void GeneRendererGL::setShine(qreal shine)
{
    if (m_shine != shine) {
        m_shine = shine;
        m_isDirty = true;
        emit updated();
    }
}

void GeneRendererGL::setUpperLimit(int limit)
{   
    static const qreal offlimit =
            Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (qreal(limit) / offlimit ) * range;
    const qreal range_pooled = m_pooledMax - m_pooledMin;
    const qreal adjusted_limit_pooled =  (qreal(limit) / offlimit ) * range_pooled;

    if (m_thresholdUpper != adjusted_limit) {
        m_thresholdUpper = adjusted_limit;
        m_thresholdUpperPooled = adjusted_limit_pooled;
        updateVisual();
    }
}

void GeneRendererGL::setLowerLimit(int limit)
{
    static const qreal offlimit =
            Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;
    const qreal range = m_max - m_min;
    const qreal adjusted_limit =  (qreal(limit) / offlimit ) * range;
    const qreal range_pooled = m_pooledMax - m_pooledMin;
    const qreal adjusted_limit_pooled =  (qreal(limit) / offlimit ) * range_pooled;

    if (m_thresholdLower != adjusted_limit) {
        m_thresholdLower = adjusted_limit;
        m_thresholdLowerPooled = adjusted_limit_pooled;
        updateVisual();
    }
}

void GeneRendererGL::generateData()
{
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());

    foreach(const DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());

        // feature cordinates
        const QPointF point(feature->x(), feature->y());

        // test if point already exists
        GeneInfoQuadTree::PointItem item(point, INVALID_INDEX);
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

    m_isDirty = true;
}

void GeneRendererGL::updateSize()
{
    GeneInfoByIdMap::const_iterator it = m_geneInfoById.begin();
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    for( ; it != end; ++it) {
        // update size of the shape
        const int index = it.value();
        DataProxy::FeaturePtr feature = it.key();
        Q_ASSERT(!feature.isNull());
        m_geneData.updateQuadSize(index, feature->x(), feature->y(), m_size);
    }

    m_isDirty = true;
    emit updated();
}

void GeneRendererGL::updateColor(DataProxy::GeneList geneList)
{
    foreach (DataProxy::GenePtr gene, geneList) {
        Q_ASSERT(!gene.isNull());

        const auto& features =
                m_dataProxy->getGeneFeatureList(m_dataProxy->getSelectedDataset(), gene->name());

        const bool selected = gene->selected();

        GeneInfoByIdMap::const_iterator it;
        GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
        Q_UNUSED(end);
        foreach(DataProxy::FeaturePtr feature, features) {
            it = m_geneInfoById.find(feature);
            Q_ASSERT(it != end);

            const int index = it.value();
            const int refCount = m_geneData.quadRefCount(index);

            // feature update color
            const QColor oldQColor = feature->color();
            const QColor geneQColor = gene->color();
            if (oldQColor != geneQColor) {
                feature->color(geneQColor);
            }
            QColor newQColor = feature->color();

            // update the color if gene is visible
            if (selected && (refCount > 0)) {
                QColor4ub color = m_geneData.quadColor(index);
                if (refCount > 1) {
                    // do color interpolation
                    color = STMath::invlerp((1.0f / qreal(refCount)), color, QColor4ub(oldQColor));
                }
                color = STMath::lerp((1.0f / qreal(refCount)), color, QColor4ub(newQColor));
                m_geneData.updateQuadColor(index, color);
            }
        }
    }

    m_isDirty = true;
    emit updated();
}

void GeneRendererGL::updateSelection(DataProxy::GeneList geneList)
{
    foreach (DataProxy::GenePtr gene, geneList) {
        Q_ASSERT(!gene.isNull());

        const auto& features =
                m_dataProxy->getGeneFeatureList(m_dataProxy->getSelectedDataset(), gene->name());

        const bool selected = gene->selected();

        GeneInfoByIdMap::const_iterator it;
        GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
        Q_UNUSED(end);
        foreach(DataProxy::FeaturePtr feature, features) {
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
            const bool offlimits =  (m_visualMode == Globals::NormalMode
                                     && ( currentHits < m_thresholdLower || currentHits > m_thresholdUpper ) );
            if ( selected && offlimits ) {
                newRefCount = oldRefCount;
            }
            m_geneData.updateQuadRefCount(index, newRefCount);

            if ( newRefCount > 0 ) {
                QColor4ub featureColor = QColor4ub(feature->color());
                QColor4ub color = m_geneData.quadColor(index);
                // inverse or normal color interpolation if selected
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
    }

    m_isDirty = true;
    emit updated();
}

void GeneRendererGL::updateVisual()
{
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());

    // reset ref count and values when in visual mode
    m_geneData.resetRefCount();
    m_geneData.resetValues();

    // clear previous selections
    clearSelection();

    GeneInfoByIdMap::const_iterator it = m_geneInfoById.begin();
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    Q_UNUSED(end);
    foreach(DataProxy::FeaturePtr feature, features) {
        it = m_geneInfoById.find(feature);
        Q_ASSERT(it != end);

        // easy access
        DataProxy::GenePtr gene =
                m_dataProxy->getGene(m_dataProxy->getSelectedDataset(), feature->gene());
        Q_ASSERT(!gene.isNull());

        const bool selected = gene->selected();
        const int index = it.value();
        const int currentHits = feature->hits();

        // update values
        const int oldValue = m_geneData.quadValue(index);
        const int newValue = (oldValue + (selected ? currentHits : 0));
        m_geneData.updateQuadValue(index, newValue);

        // update ref count
        const int oldRefCount = (int) m_geneData.quadRefCount(index);
        int newRefCount = (oldRefCount + (selected ? 1 : 0));
        const bool offlimits =  (m_visualMode == Globals::NormalMode
                                 && ( currentHits < m_thresholdLower || currentHits > m_thresholdUpper ) );
        if ( selected && offlimits ) {
            newRefCount = oldRefCount;
        }
        m_geneData.updateQuadRefCount(index, newRefCount);

        // update color && visible
        if ( selected && newRefCount > 0 && !offlimits ) {
            const QColor4ub featureColor = QColor4ub(feature->color());
            QColor4ub color = m_geneData.quadColor(index);
            color = STMath::lerp((1.0f / qreal(newRefCount)), color, featureColor);
            m_geneData.updateQuadColor(index, color);
            m_geneData.updateQuadVisible(index, true);
        } else {
            m_geneData.updateQuadVisible(index, false);
        }
    }

    m_isDirty = true;
    emit updated();
}

void GeneRendererGL::clearSelection()
{
    m_geneData.resetSelection(false);
    updateFeaturesSelected(false);
    m_isDirty = true;
    emit selectionUpdated();
    emit updated();
}

void GeneRendererGL::updateFeaturesSelected(bool selected)
{
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());
    foreach(DataProxy::FeaturePtr feature, features) {
        feature->selected(selected);
    }
}

void GeneRendererGL::updateFeaturesColor(QColor color)
{
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());
    foreach(DataProxy::FeaturePtr feature, features) {
        feature->color(color);
    }
}

void GeneRendererGL::selectGenes(const DataProxy::GeneList &geneList)
{
    DataProxy::FeatureList aggregateFeatureList;
    foreach(DataProxy::GenePtr gene, geneList) {
        Q_ASSERT(!gene.isNull());
        aggregateFeatureList <<
                                m_dataProxy->getGeneFeatureList(m_dataProxy->getSelectedDataset(), gene->name());
    }
    selectFeatures(aggregateFeatureList);
}

void GeneRendererGL::selectFeatures(const DataProxy::FeatureList &featureList)
{
    // unselect previous selecetion
    clearSelection();

    GeneInfoByIdMap::const_iterator it;
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    foreach(const DataProxy::FeaturePtr feature, featureList) {
        it = m_geneInfoById.find(feature);
        if (it != end) {
            const int index = it.value();
            const int refCount = m_geneData.quadRefCount(index);
            const int hits = feature->hits();
            const int value = m_geneData.quadValue(index);
            // do not select non-visible features or outside threshold
            if (refCount <= 0
                    || (m_visualMode == Globals::NormalMode
                        && (hits < m_thresholdLower || hits > m_thresholdUpper) )
                    || (m_visualMode != Globals::NormalMode
                        && (value < m_thresholdLowerPooled || value > m_thresholdUpperPooled) )) {
                continue;
            }
            // make the selection
            feature->selected(true);
            m_geneData.updateQuadSelected(index, true);
        }
    }

    m_isDirty = true;
    emit selectionUpdated();
    emit updated();
}

const GeneSelection::selectedItemsList GeneRendererGL::getSelectedIItems() const
{
    //TODO optimize with STL and/or concurrent methods

    //the max pixel value (gray scale)
    const int maxPixelValue = 255;
    //get the selected items
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());
    QMap<QString, SelectionType> geneSelectionsMap;
    GeneSelection::selectedItemsList geneSelectionsList;
    foreach(DataProxy::FeaturePtr feature, features) {
        if (feature->selected()) {
            //TODO m_max is the 3rd quartile (not the total max) check this assumption is correct
            const int adjustedReads = std::min(feature->hits(), m_max);
            const QString gene = feature->gene();
            geneSelectionsMap[gene].reads += adjustedReads;
            geneSelectionsMap[gene].normalizedReads += adjustedReads;
            geneSelectionsMap[gene].pixeIntensity += qGray(m_image.pixel(feature->x(), feature->y()));
            geneSelectionsMap[gene].count++;
        }
    }

    QMap<QString, SelectionType>::const_iterator it = geneSelectionsMap.begin();
    QMap<QString, SelectionType>::const_iterator end = geneSelectionsMap.end();
    for( ; it != end; ++it) {
        const int count = it.value().count;
        //TODO consider dividing reads by counts too
        const int reads = it.value().reads;
        const qreal adjustedNormalizedReads = it.value().normalizedReads /
                static_cast<qreal>(m_max * count);
        const qreal adjustedNormalizedPixelIntensity = it.value().pixeIntensity /
                static_cast<qreal>(maxPixelValue * count);
        const QString gene = it.key();
        geneSelectionsList.append(SelectionType(gene, reads,
                                                adjustedNormalizedReads, adjustedNormalizedPixelIntensity));
    }

    return geneSelectionsList;
}

void GeneRendererGL::setImage(const QImage &image)
{
    Q_ASSERT(!image.isNull());
    Q_ASSERT(!transform().isIdentity());
    m_image = image.transformed(transform().inverted().toAffine());
    //TODO get max pixel value here and store it
}

void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    //get selection area
    QRectF rect = event->path();
    QuadTreeAABB aabb(rect);

    //clear selection
    SelectionEvent::SelectionMode mode = event->mode();
    if (mode == SelectionEvent::NewSelection) {
        // unselect previous selecetion
        clearSelection();
    }

    // get selected genes
    GeneInfoQuadTree::PointItemList list;
    m_geneInfoQuadTree.select(aabb, list);

    // makes the selection
    GeneInfoQuadTree::PointItemList::const_iterator it;
    GeneInfoQuadTree::PointItemList::const_iterator end = list.end();
    for (it = list.begin(); it != end; ++it) {
        const int index = it->second;
        const int refCount = m_geneData.quadRefCount(index);
        const int value = m_geneData.quadValue(index);

        // iterate all the features in the position to get the min value of hits
        // TODO this can be optimized using STD and a better approach
        // TODO test and finish this
        int hits = m_max;
        GeneInfoReverseMap::const_iterator it2 = m_geneInfoReverse.find(index);
        GeneInfoReverseMap::const_iterator end2 = m_geneInfoReverse.end();
        while (it2 != end2 && it2.key() == index && hits != m_min) {
            hits = std::min(hits, it2.value()->hits());
            ++it2;
        }

        // do not select non-visible features or outside threshold
        if (refCount <= 0
                || (m_visualMode == Globals::NormalMode
                    && (hits < m_thresholdLower || hits > m_thresholdUpper) )
                || (m_visualMode != Globals::NormalMode
                    && (value < m_thresholdLowerPooled || value > m_thresholdUpperPooled) )) {
            continue;
        }

        // make the selection
        if (mode == SelectionEvent::ExcludeSelection) {
            //TODO refactor this
            GeneInfoReverseMap::const_iterator it2 = m_geneInfoReverse.find(index);
            GeneInfoReverseMap::const_iterator end2 = m_geneInfoReverse.end();
            while (it2 != end2 && it2.key() == index) {
                it2.value()->selected(false);
                ++it2;
            }
            m_geneData.updateQuadSelected(index, false);
        } else {
            //TODO refactor this
            GeneInfoReverseMap::const_iterator it2 = m_geneInfoReverse.find(index);
            GeneInfoReverseMap::const_iterator end2 = m_geneInfoReverse.end();
            while (it2 != end2 && it2.key() == index) {
                it2.value()->selected(true);
                ++it2;
            }
            m_geneData.updateQuadSelected(index, true);
        }
    }

    emit selectionUpdated();
    emit updated();
    m_isDirty = true;
}

void GeneRendererGL::setVisualMode(const Globals::GeneVisualMode &mode)
{
    // update visual mode
    if (m_visualMode != mode) {
        m_visualMode = mode;
        updateVisual();
    }
}

void GeneRendererGL::draw(QGLPainter *painter)
{   
    if (m_geneNode.isNull()) {
        return;
    }

    if (m_isDirty) {
        m_isDirty = false;
        // add data to node
        m_geneNode->setGeometry(m_geneData);
        m_geneNode->setCount(m_geneData.indices().size());
    }

    // enable shader
    m_shaderProgram->setActive(painter, true);

    // add UNIFORM values to shader program
    int mode = m_shaderProgram->program()->uniformLocation("in_visualMode");
    m_shaderProgram->program()->setUniformValue(mode, static_cast<GLint>(m_visualMode));

    int shine = m_shaderProgram->program()->uniformLocation("in_shine");
    m_shaderProgram->program()->setUniformValue(shine, static_cast<GLfloat>(m_shine));

    int upperLimit = m_shaderProgram->program()->uniformLocation("in_pooledUpper");
    m_shaderProgram->program()->setUniformValue(upperLimit, static_cast<GLint>(m_thresholdUpperPooled));

    int lowerLimit = m_shaderProgram->program()->uniformLocation("in_pooledLower");
    m_shaderProgram->program()->setUniformValue(lowerLimit, static_cast<GLint>(m_thresholdLowerPooled));

    int intensity = m_shaderProgram->program()->uniformLocation("in_intensity");
    m_shaderProgram->program()->setUniformValue(intensity, static_cast<GLfloat>(m_intensity));

    int shape = m_shaderProgram->program()->uniformLocation("in_shape");
    m_shaderProgram->program()->setUniformValue(shape, static_cast<GLint>(m_shape));

    // draw the data
    m_geneNode->draw(painter);

    // unable shader
    m_shaderProgram->setActive(painter, false);
}

void GeneRendererGL::setupShaders()
{
    if (m_shaderProgram != nullptr) {
        delete m_shaderProgram;
        m_shaderProgram = nullptr;
    }
    m_shaderProgram = new QGLShaderProgramEffect();

    //load and compile shaders
    m_shaderProgram->setVertexShaderFromFile(":shader/geneShader.vert");
    m_shaderProgram->setFragmentShaderFromFile(":shader/geneShader.frag");

    Q_ASSERT(!m_geneNode.isNull());
    // add shader program to node
    m_geneNode->setUserEffect(m_shaderProgram);
}

void GeneRendererGL::setDimensions(const QRectF border)
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
    if (m_shape != shape) {
        m_shape = shape;
        m_isDirty = true;
        emit updated();
    }
}

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
#include <QApplication>

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

    //initialize variables
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

    // set dirty to true when the geometry changes
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
    m_pooledMin = pooledMin;
    m_max = max;
    m_pooledMax = pooledMax;

    //this assumption holds for the default value of the threshold = 1
    m_thresholdLowerPooled = pooledMin;
    m_thresholdUpperPooled = pooledMax;
    m_thresholdLower = min;
    m_thresholdUpper = max;
}

void GeneRendererGL::setIntensity(qreal intensity)
{
    if (m_intensity != intensity) {
        m_intensity = intensity;
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
        emit updated();
    }
}

void GeneRendererGL::setUpperLimit(int limit)
{   
    //limit will be in a range 0...100, we adjust the threshold
    //using the distribution of reads (min - max)
    static const qreal offlimit =
            Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;

    //We do not want the threshold to be bigger than the max value of the distribution
    const int adjusted_limit =
            std::min(m_max, static_cast<int>((limit / offlimit) * (m_max - m_min)));
    const int adjusted_limit_pooled =
            std::min(m_pooledMax, static_cast<int>((limit / offlimit) * (m_pooledMax - m_pooledMin)));

    if (m_thresholdUpper != adjusted_limit || m_thresholdLowerPooled != adjusted_limit_pooled) {
        m_thresholdUpper = adjusted_limit;
        m_thresholdUpperPooled = adjusted_limit_pooled;
        updateVisual();
    }
}

void GeneRendererGL::setLowerLimit(int limit)
{
    //limit will be in a range 0...100, we adjust the threshold
    //using the distribution of reads (min - max)

    static const qreal offlimit =
            Globals::GENE_THRESHOLD_MAX - Globals::GENE_THRESHOLD_MIN;

    //for distributions where the values are very spread, we do not want
    //the first level of the threshold to be bigger than the minimum.
    //TODO there are cleaner and nicer ways to achieve this

    //We do not want the threshold to be lower than the min value of the distribution

    const int adjusted_limit =
            limit == Globals::GENE_THRESHOLD_MIN ?
                m_min : std::max(m_min, static_cast<int>((limit / offlimit) * (m_max - m_min)));
    const int adjusted_limit_pooled =
            limit == Globals::GENE_THRESHOLD_MIN ?
                m_pooledMin : std::max(m_pooledMin, static_cast<int>((limit / offlimit) * (m_pooledMax - m_pooledMin)));

    if (m_thresholdLower != adjusted_limit || m_thresholdLowerPooled != adjusted_limit_pooled) {
        m_thresholdLower = adjusted_limit;
        m_thresholdLowerPooled = adjusted_limit_pooled;
        updateVisual();
    }
}

void GeneRendererGL::generateData()
{
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());

    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());

        // feature cordinates
        const QPointF point(feature->x(), feature->y());

        // test if point already exists (quad tree)
        GeneInfoQuadTree::PointItem item(point, INVALID_INDEX);
        m_geneInfoQuadTree.select(point, item);

        // index corresponds to the index in the vertex arrays of the quad
        int index = item.second;

        // if does not exists, create a quad and store the index
        if (item.second == INVALID_INDEX) {
            index = m_geneData.addQuad(feature->x(), feature->y(), m_size,
                                       Globals::DEFAULT_COLOR_GENE);
            // update look up container for the quad tree
            m_geneInfoQuadTree.insert(point, index);
        }
        // update look up container for the features and indexes
        m_geneInfoById.insert(feature, index); // same position = same feature = same index
        m_geneInfoReverse.insertMulti(index, feature); //multiple features per index

    } //endforeach

    m_isDirty = true;
}

void GeneRendererGL::updateSize()
{
    GeneInfoByIdMap::const_iterator it = m_geneInfoById.begin();
    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    for ( ; it != end; ++it) {
        // update size of the quad for all features
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
    if (geneList.empty()) {
        return;
    }

    //TODO Well, right now we call updateVisual() which will
    //build the data arrays from scratch, there should not be
    //any need to do that expensive operation when updating few genes
    //there seems to be problems with this function when turning genes on/off
    //mainly related to interpolation of colors. Once those problems
    //are fixed the following two lines should be removed. Other problems
    //are related to the threshold filtering.
    updateVisual();

    /*
    //iterate the list of genes
    foreach (DataProxy::GenePtr gene, geneList) {
        Q_ASSERT(!gene.isNull());

        const auto& features =
                m_dataProxy->getGeneFeatureList(m_dataProxy->getSelectedDataset(), gene->name());

        // is gene selected?
        const bool selected = gene->selected();

        // iterate the list of features for the specific gene
        foreach(DataProxy::FeaturePtr feature, features) {
            Q_ASSERT(!feature.isNull());

            const int index = m_geneInfoById.value(feature); //the key should be present
            const float refCount = m_geneData.quadRefCount(index);
            const int currentHits = feature->hits();
            const int currentValue = m_geneData.quadValue(index);

            //we do not want to show features outside the threshold
            if (isFeatureOutsideRange(currentHits, currentValue)) {
                continue;
            }

            // update feature color
            const QColor4ub oldFeatureColor = feature->color();
            if (feature->color() != gene->color()) {
                feature->color(gene->color());
            }

            // update the color data if visible and selected
            if (selected && refCount > 0) {
                QColor4ub dataColor = m_geneData.quadColor(index);
                if (refCount > 1) {
                    // do old color inverse interpolation
                    dataColor = STMath::invlerp(1.0 / refCount, dataColor, oldFeatureColor);
                }
                // do normal interpolation with new color
                dataColor = STMath::lerp(1.0 / refCount, dataColor, feature->color());
                m_geneData.updateQuadColor(index, dataColor);
            }
        }
    }

    m_isDirty = true;
    emit updated();
    */
}

void GeneRendererGL::updateVisible(DataProxy::GeneList geneList)
{
    if (geneList.empty()) {
        return;
    }

    //TODO Well, right now we call updateVisual() which will
    //build the data arrays from scratch, there should not be
    //any need to do that expensive operation when updating few genes
    //there seems to be problems with this function when turning genes on/off
    //mainly related to interpolation of colors. Once those problems
    //are fixed the following two lines should be removed. Other problems
    //are related to the threshold filtering.
    updateVisual();

    /*
    // clear previous selections when updating visible
    //TODO this is expensive, check if needed or find a way around
    clearSelection();

    foreach (DataProxy::GenePtr gene, geneList) {
        Q_ASSERT(!gene.isNull());

        const auto& features =
                m_dataProxy->getGeneFeatureList(m_dataProxy->getSelectedDataset(), gene->name());

        // is gene selected?
        const bool selected = gene->selected();

        // iterate the features
        foreach(DataProxy::FeaturePtr feature, features) {
            Q_ASSERT(!feature.isNull());

            const int index = m_geneInfoById.value(feature); //the key should be present
            const int currentHits = feature->hits();

            // compute values
            const float oldValue = m_geneData.quadValue(index);
            const float newValue = oldValue + (selected ? currentHits : -currentHits);
            // compute ref count
            const float oldRefCount = m_geneData.quadRefCount(index);
            const float newRefCount = oldRefCount + (selected ? 1 : -1);

            //we do not want to show features outside the threshold
            if (isFeatureOutsideRange(currentHits, newValue)
                    && newRefCount != 0 && newValue != 0) {
                continue;
            }

            //update value data
            m_geneData.updateQuadValue(index, newValue);
            //update ref count data
            m_geneData.updateQuadRefCount(index, newRefCount);
            //update color data
            QColor4ub dataColor = m_geneData.quadColor(index);
            // inverse or normal color interpolation if selected
            dataColor =  selected ?
                        STMath::lerp(1.0 / newRefCount, dataColor, feature->color()) :
                        STMath::invlerp(1.0 / oldRefCount, dataColor, feature->color());
            m_geneData.updateQuadColor(index, dataColor);

        }
    }

    m_isDirty = true;
    emit updated();
    */
}

void GeneRendererGL::updateVisual()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    const auto& features =
            m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());

    // reset ref count and values when updating visuals
    m_geneData.resetRefCount();
    m_geneData.resetValues();

    // clear previous selections when updating visuals
    clearSelection();

    // iterate the features
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());

        //TODO gene ptr should be a member variable of the feature
        DataProxy::GenePtr gene =
                m_dataProxy->getGene(m_dataProxy->getSelectedDataset(), feature->gene());
        Q_ASSERT(!gene.isNull());

        const bool selected = gene->selected();
        const int index = m_geneInfoById.value(feature); //the key should be present
        const int currentHits = feature->hits();

        // compute values
        const float oldValue = m_geneData.quadValue(index);
        const float newValue = oldValue + (selected ? currentHits : 0);

        // compute ref count
        const float oldRefCount = m_geneData.quadRefCount(index);
        const float newRefCount = oldRefCount + (selected ? 1 : 0);

        // we do not want to show features outside the threshold
        if (isFeatureOutsideRange(currentHits, newValue)
                && newRefCount != 0 && newValue != 0) {
            continue;
        }

        // update feature color
        if (feature->color() != gene->color()) {
            feature->color(gene->color());
        }

        // update ref count data
        m_geneData.updateQuadRefCount(index, newRefCount);
        // update values data
        m_geneData.updateQuadValue(index, newValue);

        // update color data for visible features
        if (selected && newRefCount > 0) {
            QColor4ub color = m_geneData.quadColor(index);
            color = STMath::lerp(1.0 / newRefCount, color, feature->color());
            m_geneData.updateQuadColor(index, color);
        }
    }

    m_isDirty = true;
    emit updated();

    QApplication::restoreOverrideCursor();
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
        Q_ASSERT(!feature.isNull());
        feature->selected(selected);
    }
}

void GeneRendererGL::updateFeaturesColor(QColor color)
{
    const auto& features = m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());
        feature->color(color);
    }
}

void GeneRendererGL::selectGenes(const DataProxy::GeneList &genes)
{
    DataProxy::FeatureList aggregateFeatureList;
    foreach(DataProxy::GenePtr gene, genes) {
        Q_ASSERT(!gene.isNull());
        if (gene->selected()) {
            aggregateFeatureList <<
                                m_dataProxy->getGeneFeatureList(m_dataProxy->getSelectedDataset(), gene->name());
        }
    }
    selectFeatures(aggregateFeatureList);
}

void GeneRendererGL::selectFeatures(const DataProxy::FeatureList &features)
{
    // unselect previous selection
    clearSelection();

    // iterate the features
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());
        const int index = m_geneInfoById.value(feature);
        const int refCount = m_geneData.quadRefCount(index);
        const int hits = feature->hits();
        const int value = m_geneData.quadValue(index);
        //TODO a ref to the gene ptr should be member of the feature
        const auto gene =
                m_dataProxy->getGene(m_dataProxy->getSelectedDataset(), feature->gene());
        // do not select non-visible features or outside threshold
        if (refCount <= 0 || isFeatureOutsideRange(hits, value) || !gene->selected()) {
            continue;
        }
        // make the selection
        feature->selected(true);
        m_geneData.updateQuadSelected(index, true);
    }

    m_isDirty = true;
    emit selectionUpdated();
    emit updated();
}

GeneSelection::selectedItemsList GeneRendererGL::getSelectedIItems() const
{
    // get the features
    const auto& features =
            m_dataProxy->getFeatureList(m_dataProxy->getSelectedDataset());

    //aggregate all the selected features using SelectionType objects (aggregate by gene)
    QHash<QString, SelectionType> geneSelectionsMap;
    int mappedX = 0;
    int mappedY = 0;
    qreal totaReads = 0.0;
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());
        //assumes if a feature is selected, its gene is selected as well
        if (feature->selected()) {

            const QString geneName = feature->gene();

            //TODO a ref to the gene ptr should be member of the feature
            const auto gene =
                    m_dataProxy->getGene(m_dataProxy->getSelectedDataset(), geneName);
            //not include non selected genes
            if (!gene->selected()) {
                continue;
            }

            //TODO m_max is the 3rd quartile (not the total max) check this assumption is correct
            const int adjustedReads = std::min(feature->hits(), m_max);
            totaReads += adjustedReads;
            geneSelectionsMap[geneName].count++;
            geneSelectionsMap[geneName].reads += adjustedReads;
            //mapping points to image CS (would be faster to convert the image)
            transform().map(feature->x(), feature->y(), &mappedX, &mappedY);
            //qGray gives more weight to the green channel
            geneSelectionsMap[geneName].pixeIntensity += qGray(m_image.pixel(mappedX, mappedY));
            geneSelectionsMap[geneName].name = geneName;
        }
    }

    QHash<QString, SelectionType>::iterator it = geneSelectionsMap.begin();
    QHash<QString, SelectionType>::iterator end = geneSelectionsMap.end();
    //compute the normalization using the total sum of reads in the selection
    for ( ; it != end; ++it) {
        // normalization as reads per million described in literature
        it.value().normalizedReads = ((it.value().reads * 10e5) / totaReads) + 1;
    }

    return geneSelectionsMap.values();
}

void GeneRendererGL::setImage(const QImage &image)
{
    Q_ASSERT(!image.isNull());
    // stores a local copy of the tissue image in genes cordinate space
    // so we can obtain pixel intensity values when storing selections

    //TODO seems like doing the inverse transformation is not accurate
    //m_image = image.transformed(transform().inverted().toAffine());
    m_image = image;
}

void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // get selection area
    QRectF rect = event->path();
    QuadTreeAABB aabb(rect);

    // get selection mode
    const SelectionEvent::SelectionMode mode = event->mode();

    // if new selection clear the current selection
    if (mode == SelectionEvent::NewSelection) {
        clearSelection();
    }

    // get selected points from selection shape
    GeneInfoQuadTree::PointItemList pointList;
    m_geneInfoQuadTree.select(aabb, pointList);

    // iterate the points to get the features of each point and make
    // the selection
    foreach(GeneInfoQuadTree::PointItem point, pointList) {

        const int index = point.second;
        const int refCount = m_geneData.quadRefCount(index);
        const int value = m_geneData.quadValue(index);

        // do not select non-visible features
        if (refCount <= 0) {
            continue;
        }

        bool featuresWasSelected = false;
        const bool isSelected = mode == SelectionEvent::ExcludeSelection ? false : true;

        // iterate all the features in the position to select when possible
        const auto &featureList = m_geneInfoReverse.values(index);
        foreach(DataProxy::FeaturePtr feature, featureList) {

            //TODO a ref to the gene ptr should be member of the feature
            const auto gene =
                    m_dataProxy->getGene(m_dataProxy->getSelectedDataset(), feature->gene());

            // do not select features outside threshold or whose gene is not selected
            if (isFeatureOutsideRange(feature->hits(), value) || !gene->selected()) {
                continue;
            }

            featuresWasSelected = true;
            feature->selected(isSelected);
        }

        if (featuresWasSelected) {
            //update selection data if any feature was selected
            m_geneData.updateQuadSelected(index, isSelected);
        }
    }

    m_isDirty = true;
    emit selectionUpdated();
    emit updated();

    QApplication::restoreOverrideCursor();
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

bool GeneRendererGL::isFeatureOutsideRange(const int hits, const int totalValue)
{
    // check if the feature is outside the threshold range
    return ((m_visualMode == Globals::NormalMode
             && (hits < m_thresholdLower || hits > m_thresholdUpper))
            || (m_visualMode != Globals::NormalMode
            && (totalValue < m_thresholdLowerPooled || totalValue > m_thresholdUpperPooled)));
}

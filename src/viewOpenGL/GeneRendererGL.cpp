/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include <QGLShaderProgramEffect>
#include <QOpenGLShaderProgram>
#include <QGLAttributeValue>
#include <QImageReader>
#include <QApplication>

#include "dataModel/GeneSelection.h"

static const int INVALID_INDEX = -1;
static const qreal GENE_SIZE_DEFAULT = 0.5;
static const qreal GENE_INTENSITY_DEFAULT = 1.0;
static const GeneRendererGL::GeneShape DEFAULT_SHAPE_GENE = GeneRendererGL::GeneShape::Circle;

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
    m_geneInfoSelectedFeatures.clear();

    // lookup data
    m_geneInfoById.clear();
    m_geneInfoReverse.clear();

    // variables
    m_intensity = GENE_INTENSITY_DEFAULT;
    m_size = GENE_SIZE_DEFAULT;
    m_thresholdLower = 0;
    m_thresholdUpper = 1;
    m_shape = DEFAULT_SHAPE_GENE;

    // visual mode
    m_visualMode = NormalMode;
    // pooling mode
    m_poolingMode = PoolReadsCount;
    // color mode
    m_colorComputingMode = Globals::LinearColor;

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

void GeneRendererGL::setHitCount(const int min,
                                 const int max)
{
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

void GeneRendererGL::setUpperLimit(int limit)
{   
    if (m_thresholdUpper != limit) {
        m_thresholdUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setLowerLimit(int limit)
{
    if (m_thresholdLower != limit) {
        m_thresholdLower = limit;
        updateVisual();
    }
}

//TODO this can be optimized and run concurrently
void GeneRendererGL::generateData()
{
    const auto& features = m_dataProxy->getFeatureList();

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

//TODO this can be optimized and run concurrently
void GeneRendererGL::updateSize()
{
    Q_ASSERT(!m_geneNode.isNull() && m_geneData.isValid());

    GeneInfoByIdMap::const_iterator end = m_geneInfoById.end();
    for (GeneInfoByIdMap::const_iterator it = m_geneInfoById.begin(); it != end; ++it) {
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
    Q_ASSERT(m_geneData.isValid());

    if (geneList.empty()) {
        return;
    }

    //TODO Well, right now we call updateVisual() which will
    //update the data arrays from scratch, there should not be
    //any need to do that expensive operation when updating few genes
    //there seems to be problems with this function when turning genes on/off
    //mainly related to interpolation of colors. Once those problems
    //are fixed the following two lines should be removed. Other problems
    //are related to the threshold filtering.
    updateVisual();
}

void GeneRendererGL::updateVisible(DataProxy::GeneList geneList)
{
    Q_ASSERT(m_geneData.isValid());

    if (geneList.empty()) {
        return;
    }

    //TODO Well, right now we call updateVisual() which will
    //update the data arrays from scratch, there should not be
    //any need to do that expensive operation when updating few genes
    //there seems to be problems with this function when turning genes on/off
    //mainly related to interpolation of colors. Once those problems
    //are fixed the following two lines should be removed. Other problems
    //are related to the threshold filtering.
    updateVisual();
}

//TODO this can be optimized and run concurrently
void GeneRendererGL::updateVisual()
{
    Q_ASSERT(m_geneData.isValid());

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    m_localPooledMin = 10e6; //just a random high value
    m_localPooledMax = 0;

    const auto& features =
            m_dataProxy->getFeatureList();

    // reset ref count, selection and values when updating visuals
    m_geneData.resetRefCountSelectAndValues();
    m_geneInfoSelectedFeatures.clear();

    // iterate the features
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());

        // get the gene from the feature
        const auto gene = feature->geneObject();
        Q_ASSERT(!gene.isNull());

        const bool selected = gene->selected();
        // the key should always be present
        const int index = m_geneInfoById.value(feature);
        const int currentHits = feature->hits();

        // compute ref count
        const int oldRefCount = m_geneData.quadRefCount(index);
        const int newRefCount = oldRefCount + (selected ? 1 : 0);

        // we do not want to show features outside the threshold or not visible
        if (isFeatureOutsideRange(currentHits) && newRefCount != 0) {
            continue;
        }

        // update ref count data
        m_geneData.updateQuadRefCount(index, newRefCount);

        // compute and update values data
        const float oldValue = m_geneData.quadValue(index);
        const float newValue = m_poolingMode == PoolNumberGenes
                ? static_cast<float>(newRefCount)
                : (oldValue + (selected ? static_cast<float>(currentHits) : 0.0));
        m_geneData.updateQuadValue(index, newValue);

        // update feature color
        if (feature->color() != gene->color()) {
            feature->color(gene->color());
        }

        // update color data for visible features
        if (selected && newRefCount > 0) {
            QColor4ub dataColor = m_geneData.quadColor(index);
            const QColor4ub featureColor = feature->color();
            //color interpolation (adjusted by how expressed is the gene)
            //could use 1/refCount to adjust by drawing order
            if (featureColor != dataColor) {
                //const qreal adjustment = currentHits / m_max;
                //In order to adjust by the currentHits, we would need to
                //know the local max value of the selected genes in this feature...
                const qreal adjustment = 1.0 / static_cast<qreal>(newRefCount);
                dataColor = STMath::lerp(adjustment, dataColor, featureColor);
                m_geneData.updateQuadColor(index, dataColor);
            }

            //update local max min variables to be used as boundaries for color computing in the shaders
            m_localPooledMin = std::min(newValue, m_localPooledMin);
            m_localPooledMax = std::max(newValue, m_localPooledMax);
        }
    }

    m_isDirty = true;
    emit localPooledMinChanged(m_localPooledMin);
    emit localPooledMaxChanged(m_localPooledMax);
    emit selectionUpdated();
    emit updated();

    QGuiApplication::restoreOverrideCursor();
}

void GeneRendererGL::clearSelection()
{
    m_geneData.resetSelection(false);
    m_geneInfoSelectedFeatures.clear();
    m_isDirty = true;
    emit selectionUpdated();
    emit updated();
}

//TODO this can be optimized and run concurrently
void GeneRendererGL::selectGenes(const DataProxy::GeneList &genes)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    DataProxy::FeatureList aggregateFeatureList;
    foreach(DataProxy::GenePtr gene, genes) {
        Q_ASSERT(!gene.isNull());
        if (gene->selected()) {
            //The idea is to get the features that contain the gene
            //and then get all other features with other genes for the same position
            foreach(DataProxy::FeaturePtr feature, m_dataProxy->getGeneFeatureList(gene->name())) {
                const int featureIndex = m_geneInfoById.value(feature);
                aggregateFeatureList << m_geneInfoReverse.values(featureIndex);
            }
        }
    }

    selectFeatures(aggregateFeatureList);

    QGuiApplication::restoreOverrideCursor();
}

void GeneRendererGL::selectFeatures(const DataProxy::FeatureList &features)
{
    Q_ASSERT(m_geneData.isValid());

    // unselect previous selection
    m_geneData.resetSelection(false);
    m_geneInfoSelectedFeatures.clear();

    // iterate the features
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());
        const int index = m_geneInfoById.value(feature);
        const int refCount = m_geneData.quadRefCount(index);
        const int hits = feature->hits();
        // not filtering if the feature's gene is selected for now
        if (refCount <= 0 || isFeatureOutsideRange(hits)) {
            continue;
        }
        // update gene data and feature selected
        m_geneInfoSelectedFeatures.append(feature);
        m_geneData.updateQuadSelected(index, true);
    }

    m_isDirty = true;
    emit selectionUpdated();
    emit updated();
}

GeneSelection::selectedItemsList GeneRendererGL::getSelectedGenes() const
{
    //aggregate all the selected features using SelectionType objects (aggregate by gene)
    QHash<QString, SelectionType> geneSelectionsMap;
    int mappedX = 0;
    int mappedY = 0;
    foreach(DataProxy::FeaturePtr feature, m_geneInfoSelectedFeatures) {
        Q_ASSERT(!feature.isNull());
        //assumes if a feature is selected, its gene is selected as well
        const QString geneName = feature->gene();
        //TODO not filtering is the gene is selected
        const int adjustedReads = feature->hits();
        geneSelectionsMap[geneName].count++;
        geneSelectionsMap[geneName].reads += adjustedReads;
        //mapping points to image CS (would be faster to convert the image to the CS)
        transform().map(feature->x(), feature->y(), &mappedX, &mappedY);
        //qGray gives more weight to the green channel
        geneSelectionsMap[geneName].pixeIntensity += qGray(m_image.pixel(mappedX, mappedY));
        geneSelectionsMap[geneName].name = geneName;
    }

    return geneSelectionsMap.values();
}

const DataProxy::FeatureList& GeneRendererGL::getSelectedFeatures() const
{
    return m_geneInfoSelectedFeatures;
}


void GeneRendererGL::setImage(const QImage &image)
{
    Q_ASSERT(!image.isNull());
    m_image = image;
}

//TODO this can be optimized and run concurrently
void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    Q_ASSERT(m_geneData.isValid());

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // get selection area
    QRectF rect = event->path();
    QuadTreeAABB aabb(rect);

    // get selection mode
    const SelectionEvent::SelectionMode mode = event->mode();

    // if new selection clear the current selection
    if (mode == SelectionEvent::NewSelection) {
        // unselect previous selection
        m_geneData.resetSelection(false);
        m_geneInfoSelectedFeatures.clear();
    }

    // get selected points from selection shape
    GeneInfoQuadTree::PointItemList pointList;
    m_geneInfoQuadTree.select(aabb, pointList);

    // iterate the points to get the features of each point and make
    // the selection
    foreach(GeneInfoQuadTree::PointItem point, pointList) {

        const int index = point.second;
        const int refCount = m_geneData.quadRefCount(index);

        // do not select non-visible features
        if (refCount <= 0) {
            continue;
        }

        const bool isSelected = mode != SelectionEvent::ExcludeSelection;

        // iterate all the features in the position to select when possible
        const auto &featureList = m_geneInfoReverse.values(index);
        foreach(DataProxy::FeaturePtr feature, featureList) {
            // not checking if gene is selected or not for now
            const int hits = feature->hits();
            // do not select features outside threshold
            if (isFeatureOutsideRange(hits)) {
                continue;
            }

            //update gene data and feature selected
            m_geneData.updateQuadSelected(index, isSelected);
            if (isSelected) {
                m_geneInfoSelectedFeatures.append(feature);
            } else {
                m_geneInfoSelectedFeatures.removeOne(feature);
            }
        }
    }

    m_isDirty = true;
    emit selectionUpdated();
    emit updated();
    QGuiApplication::restoreOverrideCursor();
}

void GeneRendererGL::setVisualMode(const GeneVisualMode &mode)
{
    // update visual mode
    if (m_visualMode != mode) {
        m_visualMode = mode;
        updateVisual();
    }
}

void GeneRendererGL::setPoolingMode(const GenePooledMode &mode)
{
    // update pooling mode
    if (m_poolingMode != mode) {
        m_poolingMode = mode;
        if (m_visualMode != NormalMode) {
            updateVisual();
        }
    }
}

void GeneRendererGL::setColorComputingMode(const Globals::GeneColorMode &mode)
{
    // update color computing mode
    if (m_colorComputingMode != mode) {
        m_colorComputingMode = mode;
        if (m_visualMode != NormalMode) {
            updateVisual();
        }
    }
}

void GeneRendererGL::draw(QGLPainter *painter)
{   
    Q_ASSERT(!m_geneNode.isNull());

    if (m_isDirty) {
        m_isDirty = false;
        // add data to node
        m_geneNode->setGeometry(m_geneData);
        m_geneNode->setCount(m_geneData.indices().size());
    }

    // enable shader
    m_shaderProgram->setActive(painter, true);

    // add UNIFORM values to shader program
    int visualMode = m_shaderProgram->program()->uniformLocation("in_visualMode");
    m_shaderProgram->program()->setUniformValue(visualMode, static_cast<GLint>(m_visualMode));

    int colorMode = m_shaderProgram->program()->uniformLocation("in_colorMode");
    m_shaderProgram->program()->setUniformValue(colorMode, static_cast<GLint>(m_colorComputingMode));

    int upperLimit = m_shaderProgram->program()->uniformLocation("in_pooledUpper");
    m_shaderProgram->program()->setUniformValue(upperLimit, static_cast<GLfloat>(m_localPooledMax));

    int lowerLimit = m_shaderProgram->program()->uniformLocation("in_pooledLower");
    m_shaderProgram->program()->setUniformValue(lowerLimit, static_cast<GLfloat>(m_localPooledMin));

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

void GeneRendererGL::setShape(const GeneShape &shape)
{
    if (m_shape != shape) {
        m_shape = shape;
        m_isDirty = true;
        emit updated();
    }
}

bool GeneRendererGL::isFeatureOutsideRange(const int hits)
{
    // check if the feature is outside the threshold range
    return (hits < m_thresholdLower || hits > m_thresholdUpper);
}

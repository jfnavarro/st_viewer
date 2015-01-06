/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrent>
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
    m_geneInfoTotalReadsIndex.clear();

    // variables
    m_intensity = GENE_INTENSITY_DEFAULT;
    m_size = GENE_SIZE_DEFAULT;
    m_thresholdLower = 0;
    m_thresholdUpper = 1;
    m_shape = DEFAULT_SHAPE_GENE;
    m_localPooledMin = std::numeric_limits<float>::max();
    m_localPooledMax = std::numeric_limits<float>::min();

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
    m_isInitialized = false;
}

void GeneRendererGL::resetQuadTree(const QRectF rect)
{
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(rect);
}

void GeneRendererGL::setHitCount(const int min, const int max)
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

void GeneRendererGL::generateData()
{
    m_isDirty = false;
    m_isInitialized = false;
    QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>(this);
    QFuture<void> future = QtConcurrent::run(this, &GeneRendererGL::generateDataAsync);
    futureWatcher->setFuture(future);
    connect(futureWatcher, &QFutureWatcher<void>::finished, [=]{ m_isDirty = true;
        m_isInitialized = true; });
}

void GeneRendererGL::generateDataAsync()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    const auto& features = m_dataProxy->getFeatureList();

    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());

        // reset to default values
        feature->color(Globals::DEFAULT_COLOR_GENE);
        feature->geneObject()->color(Globals::DEFAULT_COLOR_GENE);

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
        m_geneInfoReverse.insert(index, feature); //multiple features per index
        // updated total reads per feature position
        m_geneInfoTotalReadsIndex[index] += feature->hits();

    } //endforeach

    QGuiApplication::restoreOverrideCursor();
}

//TODO this can be optimized and run concurrently
void GeneRendererGL::updateSize()
{
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    GeneInfoByIdMap::const_iterator end = m_geneInfoById.constEnd();
    GeneInfoByIdMap::const_iterator it = m_geneInfoById.constBegin();
    for (; it != end; ++it) {
        // update size of the quad for all features
        const int index = it.value();
        DataProxy::FeaturePtr feature = it.key();
        m_geneData.updateQuadSize(index, feature->x(), feature->y(), m_size);
    }

    QGuiApplication::restoreOverrideCursor();

    m_isDirty = true;
    emit updated();
}

void GeneRendererGL::updateColor(const DataProxy::GeneList &geneList)
{
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

void GeneRendererGL::updateVisible(const DataProxy::GeneList &geneList)
{
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

void GeneRendererGL::updateVisual()
{
    if (!m_isInitialized) {
        return;
    }

    QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>(this);
    QFuture<void> future = QtConcurrent::run(this, &GeneRendererGL::updateVisualAsync);
    futureWatcher->setFuture(future);
    connect(futureWatcher, &QFutureWatcher<void>::finished, [=]{
        m_isDirty = true;
        emit selectionUpdated();
        emit updated(); });
}

void GeneRendererGL::updateVisualAsync()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    //we want to get the max and min value of the reads that are going
    //to be rendered to pass these values to the shaders to compute normalized colors
    m_localPooledMin = std::numeric_limits<float>::max();
    m_localPooledMax = std::numeric_limits<float>::min();

    // reset selection array that contains the selected features
    m_geneInfoSelectedFeatures.clear();

    // iterate the index -> features container to compute the rendering data
    foreach(const int index, m_geneInfoReverse.uniqueKeys()) {
        float indexValue = 0.0;
        const bool indexSelected = false;
        QColor4ub indexColor = Globals::DEFAULT_COLOR_GENE;
        int indexRefCount = 0;

        // iterate the features to compute rendering data for an specific index (position)
        // QMultiHash iterator does not return the values only value (single)
        foreach(DataProxy::FeaturePtr feature, m_geneInfoReverse.values(index)) {
            // get feature's gene
            const auto gene = feature->geneObject();

            // update feature color
            if (feature->color() != gene->color()) {
                feature->color(gene->color());
            }

            // check if feature's gene is not selected or if feature is outside threshold
            const int currentHits = feature->hits();
            if (!gene->selected() || isFeatureOutsideRange(currentHits)) {
                continue;
            }

            //update ref count and total value
            indexRefCount++;
            if (m_poolingMode == PoolNumberGenes) {
                indexValue += static_cast<float>(indexRefCount);
            } else if (m_poolingMode == PoolReadsCount) {
                indexValue += static_cast<float>(currentHits);
            } else { //TPM value
                const int totalReads = m_geneInfoTotalReadsIndex.value(index);
                indexValue += (static_cast<float>(currentHits) / totalReads) * 10e5;
            }

            //when the color of the new feature is different than the color
            //in the feature's position we do linear interpolation adjusted
            //by the number of genes in the feature to obtain the new color
            const QColor4ub &featureColor = feature->color();
            if (indexColor != featureColor) {
                const qreal adjustment = 1.0 / static_cast<qreal>(indexRefCount);
                indexColor = STMath::lerp(adjustment, indexColor, featureColor);
            }

            //update local max min variables to be used as boundaries for color computing in the shaders
            m_localPooledMin = std::min(indexValue, m_localPooledMin);
            m_localPooledMax = std::max(indexValue, m_localPooledMax);
        }

        //update rendering data arrays
        m_geneData.updateQuadValue(index, indexValue);
        m_geneData.updateQuadSelected(index, indexSelected);
        m_geneData.updateQuadColor(index, indexColor);
    }

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
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    DataProxy::FeatureList aggregateFeatureList;
    foreach(DataProxy::GenePtr gene, genes) {
        Q_ASSERT(!gene.isNull());
        if (gene->selected()) {
            //The idea is to get the features that contains any gene present in the gene list
            //which is most likely obtained from a reg-exp search
            //m_dataProxy->getGeneFeatureList() will return only the features that contain
            //the gene name given as input, therefore we need to use the local containers
            //to obtain all the other features with other genes
            foreach(DataProxy::FeaturePtr feature, m_dataProxy->getGeneFeatureList(gene->name())) {
                const int featureIndex = m_geneInfoById.value(feature);
                aggregateFeatureList << m_geneInfoReverse.values(featureIndex);
            }
        }
    }

    QGuiApplication::restoreOverrideCursor();

    //now perform the real selection of the list of features
    selectFeatures(aggregateFeatureList);
}

void GeneRendererGL::selectFeatures(const DataProxy::FeatureList &features)
{
    if (!m_isInitialized) {
        return;
    }

    QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>(this);
    QFuture<void> future = QtConcurrent::run(this, &GeneRendererGL::selectFeaturesAsync, features);
    futureWatcher->setFuture(future);
    connect(futureWatcher, &QFutureWatcher<void>::finished, [=]{
        m_isDirty = true;
        emit selectionUpdated();
        emit updated(); });
}

void GeneRendererGL::selectFeaturesAsync(const DataProxy::FeatureList &features)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // unselect previous selection
    m_geneData.resetSelection(false);
    m_geneInfoSelectedFeatures.clear();

    // iterate the features
    foreach(DataProxy::FeaturePtr feature, features) {
        Q_ASSERT(!feature.isNull());
        const int index = m_geneInfoById.value(feature);
        // not filtering if the feature's gene is selected
        // as we want to include in the selection all the genes
        // of the feature regardless if they are selected or not
        // we only filter for visible (value == 0) or outside threshold
        if (m_geneData.quadValue(index) == 0.0 || isFeatureOutsideRange(feature->hits())) {
            continue;
        }
        // update gene data and feature selected
        m_geneInfoSelectedFeatures.append(feature);
        m_geneData.updateQuadSelected(index, true);
    }

    QGuiApplication::restoreOverrideCursor();
}

GeneSelection::selectedItemsList GeneRendererGL::getSelectedGenes() const
{
    //aggregate all the selected features using SelectionType objects (aggregate by gene)
    QHash<QString, SelectionType> geneSelectionsMap;
    foreach(DataProxy::FeaturePtr feature, m_geneInfoSelectedFeatures) {
        Q_ASSERT(!feature.isNull());
        //we include in the selection of the genes present in the selected feature
        //regardless if the feature was selected manually or by genes reg exp.
        //the reads are aggregated and a counter increased
        const QString geneName = feature->gene();
        geneSelectionsMap[geneName].count++;
        geneSelectionsMap[geneName].reads += feature->hits();
        geneSelectionsMap[geneName].name = geneName;
    }

    return geneSelectionsMap.values();
}

const DataProxy::FeatureList& GeneRendererGL::getSelectedFeatures() const
{
    return m_geneInfoSelectedFeatures;
}

void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    if (!m_isInitialized) {
        return;
    }

    QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>(this);
    QFuture<void> future = QtConcurrent::run(this, &GeneRendererGL::setSelectionAreaAsync, *event);
    futureWatcher->setFuture(future);
    connect(futureWatcher, &QFutureWatcher<void>::finished, [=]{
        m_isDirty = true;
        emit selectionUpdated();
        emit updated(); });
}

void GeneRendererGL::setSelectionAreaAsync(const SelectionEvent &event)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    qDebug() << "Selecting " << event.path();

    // get selection area
    const QuadTreeAABB aabb(event.path());

    // get selection mode
    const SelectionEvent::SelectionMode mode = event.mode();

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
        // get the position's index
        const int index = point.second;

        // do not select non-visible features
        if (m_geneData.quadValue(index) == 0.0) {
            continue;
        }

        // type of selection (add or remove)
        const bool isSelected = mode != SelectionEvent::ExcludeSelection;

        // iterate all the features in the position to select when possible
        foreach(DataProxy::FeaturePtr feature, m_geneInfoReverse.values(index)) {
            // not filtering if the feature's gene is selected
            // as we want to include in the selection all the genes
            // of the feature regardless if they are selected or not
            // we filter features outside the threshold
            if (isFeatureOutsideRange(feature->hits())) {
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

    QGuiApplication::restoreOverrideCursor();
}

void GeneRendererGL::setVisualMode(const GeneVisualMode mode)
{
    // update visual mode
    if (m_visualMode != mode) {
        m_visualMode = mode;
        updateVisual();
    }
}

void GeneRendererGL::setPoolingMode(const GenePooledMode mode)
{
    // update pooling mode
    if (m_poolingMode != mode) {
        m_poolingMode = mode;
        if (m_visualMode != NormalMode) {
            updateVisual();
        }
    }
}

void GeneRendererGL::setColorComputingMode(const Globals::GeneColorMode mode)
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
    if (!m_isInitialized) {
        return;
    }

    Q_ASSERT(!m_geneNode.isNull());

    if (m_isDirty) {
        m_isDirty = false;
        // add data to node
        m_geneNode->setGeometry(m_geneData);
        m_geneNode->setCount(m_geneData.indices().size());
    }

    qDebug() << "Drawing genes";

    // enable shader
    m_shaderProgram->setActive(painter, true);

    // add UNIFORM values to shader program
    int visualMode = m_shaderProgram->program()->uniformLocation("in_visualMode");
    m_shaderProgram->program()->setUniformValue(visualMode, static_cast<GLint>(m_visualMode));

    int colorMode = m_shaderProgram->program()->uniformLocation("in_colorMode");
    m_shaderProgram->program()->setUniformValue(colorMode, static_cast<GLint>(m_colorComputingMode));

    int poolingMode = m_shaderProgram->program()->uniformLocation("in_poolingMode");
    m_shaderProgram->program()->setUniformValue(poolingMode, static_cast<GLint>(m_poolingMode));

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
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(QuadTreeAABB(border));
}

const QRectF GeneRendererGL::boundingRect() const
{
    return m_border;
}

void GeneRendererGL::setShape(const GeneShape shape)
{
    if (m_shape != shape) {
        m_shape = shape;
        emit updated();
    }
}

bool GeneRendererGL::isFeatureOutsideRange(const int hits)
{
    // check if the feature is outside the threshold range
    return (hits < m_thresholdLower || hits > m_thresholdUpper);
}

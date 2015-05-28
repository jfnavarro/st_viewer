/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneRendererGL.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QOpenGLShaderProgram>
#include <QImageReader>
#include <QApplication>

#include "dataModel/GeneSelection.h"

static const int INVALID_INDEX = -1;
static const float GENE_SIZE_DEFAULT = 0.5;
static const float GENE_INTENSITY_DEFAULT = 1.0;
static const GeneRendererGL::GeneShape DEFAULT_SHAPE_GENE = GeneRendererGL::GeneShape::Circle;

GeneRendererGL::GeneRendererGL(QPointer<DataProxy> dataProxy, QObject *parent)
    : GraphicItemGL(parent),
      m_isDirtyStaticData(false),
      m_isDirtyDynamicData(false),
      m_isInitialized(false),
      m_dataProxy(dataProxy),
      m_vertexsBuffer(QOpenGLBuffer::VertexBuffer),
      m_indexesBuffer(QOpenGLBuffer::IndexBuffer),
      m_texturesBuffer(QOpenGLBuffer::VertexBuffer),
      m_colorsBuffer(QOpenGLBuffer::VertexBuffer),
      m_selectedBuffer(QOpenGLBuffer::VertexBuffer),
      m_visibleBuffer(QOpenGLBuffer::VertexBuffer),
      m_readsBuffer(QOpenGLBuffer::VertexBuffer)
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

}

void GeneRendererGL::clearData()
{
    // clear gene plot data
    m_geneData.clearData();

    //clear selection
    m_geneInfoSelectedFeatures.clear();

    // lookup data
    m_geneInfoByIndex.clear();
    m_geneInfoTotalReadsIndex.clear();
    m_geneIntoByGene.clear();
    m_geneInfoByFeature.clear();
    m_geneInfoByFeatureIndex.clear();

    // variables
    m_intensity = GENE_INTENSITY_DEFAULT;
    m_size = GENE_SIZE_DEFAULT;
    m_thresholdReadsLower = std::numeric_limits<int>::max();
    m_thresholdReadsUpper = std::numeric_limits<int>::min();
    m_thresholdGenesLower = std::numeric_limits<int>::max();
    m_thresholdGenesUpper = std::numeric_limits<int>::min();
    m_thresholdTPMLower = std::numeric_limits<int>::max();
    m_thresholdTPMUpper = std::numeric_limits<int>::min();
    m_shape = DEFAULT_SHAPE_GENE;
    m_localPooledMin = std::numeric_limits<int>::max();
    m_localPooledMax = std::numeric_limits<int>::min();

    // visual mode
    m_visualMode = NormalMode;
    // pooling mode
    m_poolingMode = PoolReadsCount;
    // color mode
    m_colorComputingMode = Globals::LinearColor;

    // set dirty and initialized to false
    m_isDirtyDynamicData = false;
    m_isDirtyStaticData = false;
    m_isInitialized = false;
}

void GeneRendererGL::resetQuadTree(const QRectF &rect)
{
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(rect);
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

void GeneRendererGL::setReadsUpperLimit(const int limit)
{   
    if (m_thresholdReadsUpper != limit) {
        m_thresholdReadsUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setReadsLowerLimit(const int limit)
{
    if (m_thresholdReadsLower != limit) {
        m_thresholdReadsLower = limit;
        updateVisual();
    }
}

void GeneRendererGL::setGenesUpperLimit(const int limit)
{
    if (m_thresholdGenesUpper != limit) {
        m_thresholdGenesUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setGenesLowerLimit(const int limit)
{
    if (m_thresholdGenesLower != limit) {
        m_thresholdGenesLower = limit;
        updateVisual();
    }
}

void GeneRendererGL::setTPMUpperLimit(const int limit)
{
    if (m_thresholdTPMUpper != limit) {
        m_thresholdTPMUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setTPMLowerLimit(const int limit)
{
    if (m_thresholdTPMLower != limit) {
        m_thresholdTPMLower = limit;
        updateVisual();
    }
}

void GeneRendererGL::generateData()
{
    clearData();

    //update shader
    setupShaders();

    //generate data
    generateDataAsync();

    m_isDirtyStaticData = true;
    m_isDirtyDynamicData = true;
    m_isInitialized = true;
}

void GeneRendererGL::generateDataAsync()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    //temp hack to get the max,min number of genes per feature, next API release this info
    //will come in the dataset
    QHash<int,unsigned> indexGenesCount;
    foreach(DataProxy::FeaturePtr feature, m_dataProxy->getFeatureList()) {
        Q_ASSERT(!feature.isNull());

        // reset to default value the gene color
        feature->geneObject()->color(Globals::DEFAULT_COLOR_GENE);
        feature->geneObject()->selected(false);

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
        m_geneInfoByIndex.insert(index, feature); // multiple features per index
        m_geneIntoByGene.insert(feature->geneObject(), index); // multiple indexes per gene
        //TODO not used at the moment but they might be part of a new approach
        //to compute rendering data
        //m_geneInfoByFeature.insert(feature->geneObject(), feature); // multiple features per gene
        //m_geneInfoByFeatureIndex.insert(feature, index); // one index per feature

        // updated total reads per feature position
        m_geneInfoTotalReadsIndex[index] += feature->hits();

        // update thresholds (next API will contain this information)
        ++indexGenesCount[index];
        const int num_genes_feature = indexGenesCount.value(index);
        const int feature_reads = feature->hits();
        //m_thresholdGenesLower = std::min(num_genes_feature, m_thresholdGenesLower);
        m_thresholdGenesUpper = std::max(num_genes_feature, m_thresholdGenesUpper);
        m_thresholdReadsLower = std::min(feature_reads, m_thresholdReadsLower);
        m_thresholdReadsUpper = std::max(feature_reads, m_thresholdReadsUpper);

    } //endforeach

    m_thresholdGenesLower = 1;

    QGuiApplication::restoreOverrideCursor();
}

void GeneRendererGL::initBasicBuffers()
{
    if (!m_vao.isCreated()) {
        m_vao.create();
    }
    m_vao.bind();

    m_shader_program.bind();

    //Vertices buffer
    if (!m_vertexsBuffer.isCreated()) {
        m_vertexsBuffer.create();
        m_vertexsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_vertexsBuffer.bind();
    m_vertexsBuffer.allocate(m_geneData.m_vertices.constData(),
                             m_geneData.m_vertices.size() * 3 * sizeof(float));
    m_shader_program.enableAttributeArray("vertexAttr");
    m_shader_program.setAttributeBuffer("vertexAttr", GL_FLOAT, 0, 3);
    m_vertexsBuffer.release();

    //Indexes buffer
    if (!m_indexesBuffer.isCreated()) {
        m_indexesBuffer.create();
        m_indexesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_indexesBuffer.bind();
    m_indexesBuffer.allocate(m_geneData.m_indexes.constData(),
                             m_geneData.m_indexes.size() * 1 * sizeof(int));
    m_indexesBuffer.release();

    //Textures buffer
    if (!m_texturesBuffer.isCreated()) {
        m_texturesBuffer.create();
        m_texturesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_texturesBuffer.bind();
    m_texturesBuffer.allocate(m_geneData.m_textures.constData(),
                              m_geneData.m_textures.size() * 2 * sizeof(float));
    m_shader_program.enableAttributeArray("textureAttr");
    m_shader_program.setAttributeBuffer("textureAttr", GL_FLOAT, 0, 2);
    m_texturesBuffer.release();

    m_vao.release();
    m_shader_program.release();

    m_isDirtyStaticData = false;
}

void GeneRendererGL::initDynamicBuffers()
{
    if (!m_vao.isCreated()) {
        m_vao.create();
    }
    m_vao.bind();

    m_shader_program.bind();

    //Color buffer
    if (!m_colorsBuffer.isCreated()) {
        m_colorsBuffer.create();
        m_colorsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_colorsBuffer.bind();
    m_colorsBuffer.allocate(m_geneData.m_colors.constData(),
                            m_geneData.m_colors.size() * 4 * sizeof(float));
    m_shader_program.enableAttributeArray("colorAttr");
    m_shader_program.setAttributeBuffer("colorAttr", GL_FLOAT, 0, 4);
    m_colorsBuffer.release();

    //Selected buffer
    if (!m_selectedBuffer.isCreated()) {
        m_selectedBuffer.create();
        m_selectedBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_selectedBuffer.bind();
    m_selectedBuffer.allocate(m_geneData.m_selected.constData(),
                              m_geneData.m_selected.size() * 1 * sizeof(float));
    m_shader_program.enableAttributeArray("selectedAttr");
    m_shader_program.setAttributeBuffer("selectedAttr", GL_FLOAT, 0, 1);
    m_selectedBuffer.release();

    //Visible buffer
    if (!m_visibleBuffer.isCreated()) {
        m_visibleBuffer.create();
        m_visibleBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_visibleBuffer.bind();
    m_visibleBuffer.allocate(m_geneData.m_visible.constData(),
                             m_geneData.m_visible.size() * 1 * sizeof(float));
    m_shader_program.enableAttributeArray("visibleAttr");
    m_shader_program.setAttributeBuffer("visibleAttr", GL_FLOAT, 0, 1);
    m_visibleBuffer.release();

    //Reads buffer
    if (!m_readsBuffer.isCreated()) {
        m_readsBuffer.create();
        m_readsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_readsBuffer.bind();
    m_readsBuffer.allocate(m_geneData.m_reads.constData(),
                           m_geneData.m_reads.size() * 1 * sizeof(float));
    m_shader_program.enableAttributeArray("readsAttr");
    m_shader_program.setAttributeBuffer("readsAttr", GL_FLOAT, 0, 1);
    m_readsBuffer.release();

    m_vao.release();
    m_shader_program.release();

    m_isDirtyDynamicData = false;
}

int GeneRendererGL::getMinReadsThreshold() const
{
    return m_thresholdReadsLower;
}

int GeneRendererGL::getMaxReadsThreshold() const
{
    return m_thresholdReadsUpper;
}

int GeneRendererGL::getMinGenesThreshold() const
{
    return m_thresholdGenesLower;
}

int GeneRendererGL::getMaxGenesThreshold() const
{
    return m_thresholdGenesUpper;
}

int GeneRendererGL::getMinTPMThreshold() const
{
    return m_thresholdTPMLower;
}

int GeneRendererGL::getMaxTPMThreshold() const
{
    return m_thresholdTPMUpper;
}

void GeneRendererGL::updateSize()
{
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    foreach(const int index, m_geneInfoByIndex.uniqueKeys()) {
        // update size of the quad for only one feature
        // (all features of same index should have same coordinates)
        DataProxy::FeaturePtr feature = m_geneInfoByIndex.value(index);
        m_geneData.updateQuadSize(index, feature->x(), feature->y(), m_size);
    }

    QGuiApplication::restoreOverrideCursor();
    m_isDirtyStaticData = true;
    m_isDirtyDynamicData = true;
    emit updated();
}

void GeneRendererGL::updateColor(const DataProxy::GeneList &geneList)
{
    if (geneList.empty()) {
        return;
    }

    updateVisual(geneList);
}

void GeneRendererGL::updateVisible(const DataProxy::GeneList &geneList)
{
    if (geneList.empty()) {
        return;
    }

    updateVisual(geneList);
}

void GeneRendererGL::updateVisual()
{
    updateVisual(m_geneInfoByIndex.uniqueKeys());
}

void GeneRendererGL::updateVisual(const DataProxy::GeneList &geneList, const bool forceSelection)
{
    QSet<int> indexes;
    foreach(DataProxy::GenePtr gene, geneList) {
        GeneInfoByGeneMap::const_iterator it = m_geneIntoByGene.find(gene);
        GeneInfoByGeneMap::const_iterator end = m_geneIntoByGene.end();
        for (; it != end && it.key() == gene; ++it){
            indexes.insert(it.value());
        }
    }

    updateVisual(indexes.toList(), forceSelection);
}

void GeneRendererGL::updateVisual(const QList<int> &indexes, const bool forceSelection)
{
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    //we want to get the max and min value of the reads that are going
    //to be rendered to pass these values to the shaders to compute normalized colors
    m_localPooledMin = std::numeric_limits<int>::max();
    m_localPooledMax = std::numeric_limits<int>::min();

    // reset selection array that contains the selected features
    m_geneInfoSelectedFeatures.clear();

    // declare temp variables for storing the index's value and color
    int indexValue = 0;
    QColor indexColor = Globals::DEFAULT_COLOR_GENE;
    const bool pooling_genes = m_poolingMode == PoolNumberGenes;
    const bool pooling_tpm = m_poolingMode == PoolTPMs;
    const bool isPooled = m_visualMode == DynamicRangeMode || m_visualMode == HeatMapMode;

    // iterate the index -> features container to compute the rendering data
    foreach(const int index, indexes) {

        // temp local variables to store the genes/reads/tpm/color of each feature
        int indexValueReads = 0;
        int indexValueGenes = 0;
        int indexValueTotalGenes = 0;
        indexColor = Globals::DEFAULT_COLOR_GENE;

        // iterate the features to compute rendering data for an specific index (position)
        GeneInfoByIndexMap::const_iterator it = m_geneInfoByIndex.constFind(index);
        GeneInfoByIndexMap::const_iterator end = m_geneInfoByIndex.constEnd();
        for (; it != end && it.key() == index; ++it) {
            DataProxy::FeaturePtr feature = it.value();
            Q_ASSERT(feature);

            //increase the gene counter always
            ++indexValueTotalGenes;

            const int currentHits = feature->hits();
            //check if the reads are outside the threshold
            if (featureReadsOutsideRange(currentHits)) {
                continue;
            }

            //if we want to enforce the selection we add the feature to the container
            if (forceSelection) {
                m_geneInfoSelectedFeatures.append(feature);
            }

            // get feature's gene
            const auto gene = feature->geneObject();
            //check if gene is selected to visualize
            if (!gene->selected()) {
                continue;
            }

            //update local variables for number of reads and genes
            indexValueReads += currentHits;
            ++indexValueGenes;

            //when the color of the new feature is different than the color
            //in the feature's index we do linear interpolation adjusted
            //by the number of genes in the feature to obtain the new color
            const QColor &featureColor = gene->color();
            if (indexColor != featureColor) {
                const qreal adjustment = 1.0 / indexValueGenes;
                indexColor = STMath::lerp(adjustment, indexColor, featureColor);
            }
        }

        //we filter out features by its gene count regardles if genes are visible or not
        const bool visible = indexValueGenes != 0 && !featureGenesOutsideRange(indexValueTotalGenes);

        //update pooled min-max to compute colors
        indexValue = indexValueReads;
        if (isPooled && visible) {
            if (pooling_genes) {
                indexValue = indexValueGenes;
            } else if (pooling_tpm) {
                indexValue = STMath::tpmNormalization<int>(indexValueReads,
                                                      m_geneInfoTotalReadsIndex.value(index));
            }
            m_localPooledMin = std::min(indexValue, m_localPooledMin);
            m_localPooledMax = std::max(indexValue, m_localPooledMax);
        }

        //update rendering data arrays
        m_geneData.updateQuadReads(index, indexValue);
        m_geneData.updateQuadSelected(index, visible && forceSelection);
        m_geneData.updateQuadVisible(index, visible);
        m_geneData.updateQuadColor(index, indexColor);
    }

    QGuiApplication::restoreOverrideCursor();

    m_isDirtyDynamicData = true;
    emit selectionUpdated();
    emit updated();
}

void GeneRendererGL::clearSelection()
{
    m_geneData.clearSelectionArray();
    m_geneInfoSelectedFeatures.clear();
    m_isDirtyDynamicData = true;
    emit selectionUpdated();
    emit updated();
}

void GeneRendererGL::selectGenes(const DataProxy::GeneList &genes)
{
    updateVisual(genes, true);
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
        ++geneSelectionsMap[geneName].count;
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

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // get selection area
    const QuadTreeAABB aabb(event->path());

    // get selection mode
    const SelectionEvent::SelectionMode mode = event->mode();

    // if new selection clear the current selection
    if (mode == SelectionEvent::NewSelection) {
        // unselect previous selection
        m_geneData.clearSelectionArray();
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
        //NOTE no point to filter by genes/TPM as discarged indexes by genes/TPM
        //will not be visible
        if (!m_geneData.quadVisible(index)) {
            continue;
        }

        // type of selection (add or remove)
        const bool isSelected = mode != SelectionEvent::ExcludeSelection;

        // iterate all the features in the position to select when possible
        foreach(DataProxy::FeaturePtr feature, m_geneInfoByIndex.values(index)) {
            // not filtering if the feature's gene is selected
            // as we want to include in the selection all the genes
            // of the feature regardless if they are selected or not
            // we just filter features outside the threshold
            if (featureReadsOutsideRange(feature->hits())) {
                continue;
            }

            // update gene data and feature selected (only needed one time)
            if (m_geneData.quadSelected(index) != isSelected) {
                m_geneData.updateQuadSelected(index, isSelected);
            }

            // update the container with selected features
            if (isSelected) {
                m_geneInfoSelectedFeatures.append(feature);
            } else {
                m_geneInfoSelectedFeatures.removeOne(feature);
            }
        }
    }

    QGuiApplication::restoreOverrideCursor();
    m_isDirtyDynamicData = true;
    emit selectionUpdated();
    emit updated();
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

void GeneRendererGL::draw(QOpenGLFunctionsVersion *m_qopengl_functions)
{
    if (!m_isInitialized) {
        return;
    }

    if (m_isDirtyStaticData) {
        initBasicBuffers();
    }

    if (m_isDirtyDynamicData) {
        initDynamicBuffers();
    }

    QMatrix4x4 projectionModelViewMatrix = getProjection() * getModelView();

    int visualMode = m_shader_program.uniformLocation("in_visualMode");
    int colorMode = m_shader_program.uniformLocation("in_colorMode");
    int poolingMode = m_shader_program.uniformLocation("in_poolingMode");
    int upperLimit = m_shader_program.uniformLocation("in_pooledUpper");
    int lowerLimit = m_shader_program.uniformLocation("in_pooledLower");
    int intensity = m_shader_program.uniformLocation("in_intensity");
    int shape = m_shader_program.uniformLocation("in_shape");
    int projMatrix = m_shader_program.uniformLocation("in_ModelViewProjectionMatrix");

    m_shader_program.bind();

    // add UNIFORM values to shader program
    m_shader_program.setUniformValue(visualMode, static_cast<GLint>(m_visualMode));
    m_shader_program.setUniformValue(colorMode, static_cast<GLint>(m_colorComputingMode));
    m_shader_program.setUniformValue(poolingMode, static_cast<GLint>(m_poolingMode));
    m_shader_program.setUniformValue(upperLimit, static_cast<GLint>(m_localPooledMax));
    m_shader_program.setUniformValue(lowerLimit, static_cast<GLint>(m_localPooledMin));
    m_shader_program.setUniformValue(intensity, static_cast<GLfloat>(m_intensity));
    m_shader_program.setUniformValue(shape, static_cast<GLint>(m_shape));
    m_shader_program.setUniformValue(projMatrix, projectionModelViewMatrix);

    m_vao.bind();
    m_vertexsBuffer.bind();
    m_indexesBuffer.bind();
    m_texturesBuffer.bind();
    m_colorsBuffer.bind();
    m_selectedBuffer.bind();
    m_visibleBuffer.bind();
    m_readsBuffer.bind();

    m_qopengl_functions->glDrawElements(GL_TRIANGLES, m_geneData.m_indexes.size(), GL_UNSIGNED_INT, 0);

    m_vertexsBuffer.release();
    m_indexesBuffer.release();
    m_texturesBuffer.release();
    m_colorsBuffer.release();
    m_selectedBuffer.release();
    m_visibleBuffer.release();
    m_readsBuffer.release();
    m_vao.release();

    m_shader_program.release();
}

void GeneRendererGL::setupShaders()
{
    if (m_shader_program.isLinked()) {
        return;
    }

    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile(":shader/geneShader.vert");

    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile(":shader/geneShader.frag");

    m_shader_program.addShader(&vShader);
    m_shader_program.addShader(&fShader);

    if (!m_shader_program.link()) {
        qDebug() << "GeneRendererGL: unable to link a shader program." + m_shader_program.log();
        QApplication::exit();
    }
}

void GeneRendererGL::setDimensions(const QRectF &border)
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

bool GeneRendererGL::featureReadsOutsideRange(const int value)
{
    return (value < m_thresholdReadsLower || value > m_thresholdReadsUpper);
}

bool GeneRendererGL::featureGenesOutsideRange(const int value)
{
    return (value < m_thresholdGenesLower || value > m_thresholdGenesUpper);
}

bool GeneRendererGL::featureTPMOutsideRange(const int value)
{
    return (value < m_thresholdTPMLower || value > m_thresholdTPMUpper);
}

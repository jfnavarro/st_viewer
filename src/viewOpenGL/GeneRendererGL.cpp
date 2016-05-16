#include "GeneRendererGL.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QOpenGLShaderProgram>
#include <QImageReader>
#include <QApplication>

#include "dataModel/UserSelection.h"
#include "dataModel/Feature.h"
#include "dataModel/Gene.h"
#include "SettingsVisual.h"

static const int INVALID_INDEX = -1;
static const float GENE_SIZE_DEFAULT = 0.5;
static const float GENE_INTENSITY_DEFAULT = 1.0;
static const GeneRendererGL::GeneShape DEFAULT_SHAPE_GENE = GeneRendererGL::GeneShape::Circle;

GeneRendererGL::GeneRendererGL(QSharedPointer<DataProxy> dataProxy, QObject *parent)
    : GraphicItemGL(parent)
    , m_isDirtyStaticData(false)
    , m_isDirtyDynamicData(false)
    , m_isInitialized(false)
    , m_dataProxy(dataProxy)
    , m_vertexsBuffer(QOpenGLBuffer::VertexBuffer)
    , m_indexesBuffer(QOpenGLBuffer::IndexBuffer)
    , m_texturesBuffer(QOpenGLBuffer::VertexBuffer)
    , m_colorsBuffer(QOpenGLBuffer::VertexBuffer)
    , m_selectedBuffer(QOpenGLBuffer::VertexBuffer)
    , m_visibleBuffer(QOpenGLBuffer::VertexBuffer)
    , m_readsBuffer(QOpenGLBuffer::VertexBuffer)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, true);

    // initialize variables
    clearData();
}

GeneRendererGL::~GeneRendererGL()
{
}

void GeneRendererGL::clearData()
{
    // clear gene plot data
    m_geneData.clearData();

    // clear selection
    m_geneInfoSelectedFeatures.clear();

    // lookup data
    m_geneInfoByIndex.clear();
    m_geneInfoTotalReadsIndex.clear();
    m_geneInfoTotalGenesIndex.clear();
    m_geneInfoByGene.clear();
    m_geneInfoByFeatureIndex.clear();
    m_indexes.clear();

    // variables
    m_intensity = GENE_INTENSITY_DEFAULT;
    m_size = GENE_SIZE_DEFAULT;
    m_thresholdReadsLower = std::numeric_limits<unsigned>::max();
    m_thresholdReadsUpper = std::numeric_limits<unsigned>::min();
    m_thresholdGenesLower = std::numeric_limits<unsigned>::max();
    m_thresholdGenesUpper = std::numeric_limits<unsigned>::min();
    m_thresholdTotalReadsLower = std::numeric_limits<unsigned>::max();
    m_thresholdTotalReadsUpper = std::numeric_limits<unsigned>::min();
    m_shape = DEFAULT_SHAPE_GENE;
    m_localPooledMin = std::numeric_limits<unsigned>::max();
    m_localPooledMax = std::numeric_limits<unsigned>::min();
    m_genes_cutoff = true;

    // visual mode
    m_visualMode = NormalMode;
    // pooling mode
    m_poolingMode = PoolReadsCount;
    // color mode
    m_colorComputingMode = Visual::LinearColor;

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

void GeneRendererGL::setIntensity(float intensity)
{
    if (m_intensity != intensity) {
        m_intensity = intensity;
        emit updated();
    }
}

void GeneRendererGL::setSize(float size)
{
    if (m_size != size) {
        m_size = size;
        updateSize();
    }
}

void GeneRendererGL::setReadsUpperLimit(const unsigned limit)
{
    if (m_thresholdReadsUpper != limit) {
        m_thresholdReadsUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setReadsLowerLimit(const unsigned limit)
{
    if (m_thresholdReadsLower != limit) {
        m_thresholdReadsLower = limit;
        updateVisual();
    }
}

void GeneRendererGL::setGenesUpperLimit(const unsigned limit)
{
    if (m_thresholdGenesUpper != limit) {
        m_thresholdGenesUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setGenesLowerLimit(const unsigned limit)
{
    if (m_thresholdGenesLower != limit) {
        m_thresholdGenesLower = limit;
        updateVisual();
    }
}

void GeneRendererGL::setTotalReadsUpperLimit(const unsigned limit)
{
    if (m_thresholdTotalReadsUpper != limit) {
        m_thresholdTotalReadsUpper = limit;
        updateVisual();
    }
}

void GeneRendererGL::setTotalReadsLowerLimit(const unsigned limit)
{
    if (m_thresholdTotalReadsLower != limit) {
        m_thresholdTotalReadsLower = limit;
        updateVisual();
    }
}

void GeneRendererGL::generateData()
{
    clearData();

    // update shader
    setupShaders();

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    for (const auto &feature : m_dataProxy->getFeatureList()) {
        Q_ASSERT(feature);
        // Get the feature's gene
        auto gene = m_dataProxy->geneGeneObject(feature->gene());
        Q_ASSERT(gene);

        // feature cordinates
        const QPointF point(feature->x(), feature->y());

        // test if point already exists (quad tree)
        GeneInfoQuadTree::PointItem item(point, INVALID_INDEX);
        m_geneInfoQuadTree.select(point, item);

        // index corresponds to the index in the array of vertices for the OpenGL data
        int index = item.second;

        // if it does not exists, create a quad and store the index
        if (item.second == INVALID_INDEX) {
            index = m_geneData.addQuad(feature->x(),
                                       feature->y(),
                                       m_size,
                                       Visual::DEFAULT_COLOR_GENE);
            // update look up container for the quad tree
            m_geneInfoQuadTree.insert(point, index);
            // add to list of indexes
            m_indexes.insert(index);
        }

        // update look up container for the features and indexes
        // multiple features per index
        m_geneInfoByIndex.insert(std::make_pair(index, feature));
        // multiple indexes per gene
        m_geneInfoByGene.insert(std::make_pair(gene, index));
        // one index per feature
        m_geneInfoByFeatureIndex.insert(std::make_pair(feature, index));

        // updated total reads/genes per spot/index
        const unsigned feature_reads = feature->count();
        const unsigned num_genes_spot = ++m_geneInfoTotalGenesIndex[index];
        const unsigned num_reads_spot = m_geneInfoTotalReadsIndex[index] += feature_reads;

        // update thresholds (TODO next API will contain this information so no need for this)
        m_thresholdGenesLower = std::min(num_genes_spot, m_thresholdGenesLower);
        m_thresholdGenesUpper = std::max(num_genes_spot, m_thresholdGenesUpper);
        m_thresholdReadsLower = std::min(feature_reads, m_thresholdReadsLower);
        m_thresholdReadsUpper = std::max(feature_reads, m_thresholdReadsUpper);
        m_thresholdTotalReadsLower = std::min(num_reads_spot, m_thresholdTotalReadsLower);
        m_thresholdTotalReadsUpper = std::max(num_reads_spot, m_thresholdTotalReadsUpper);

    } // endforeach

    // compute gene's cut off
    compuateGenesCutoff();

    QGuiApplication::restoreOverrideCursor();

    m_isDirtyStaticData = true;
    m_isDirtyDynamicData = true;
    m_isInitialized = true;
}

void GeneRendererGL::compuateGenesCutoff()
{
    const unsigned minseglen = 2;
    for (auto gene : m_dataProxy->getGeneList()) {
        Q_ASSERT(gene);
        auto gene_indexes = m_geneInfoByGene.equal_range(gene);
        // get all the features of the spots that contain that gene
        DataProxy::FeatureList feature_list;
        for (auto it = gene_indexes.first; it != gene_indexes.second; ++it) {
            auto features = m_geneInfoByIndex.equal_range(it->second);
            std::transform(features.first,
                           features.second,
                           std::back_inserter(feature_list),
                           [](const GeneInfoByIndexMap::value_type &ele) { return ele.second; });
        }
        // create a list of the counts from the list of features
        std::vector<unsigned> counts(feature_list.size());
        std::transform(feature_list.begin(),
                       feature_list.end(),
                       std::back_inserter(counts),
                       [](const DataProxy::FeaturePtr &ele) { return ele->count(); });
        const size_t num_features = counts.size();
        // if too little counts or if all the counts are the same cut off is the min count present
        if (num_features < minseglen + 1
            || std::equal(counts.begin() + 1, counts.end(), counts.begin())) {
            const unsigned cutoff = *std::min_element(counts.begin(), counts.end());
            gene->cut_off(cutoff);
            continue;
        }
        // sort the counts and compute a list of their squared sum
        std::sort(counts.begin(), counts.end());
        std::vector<unsigned> squared_summed_counts(counts);
        std::transform(squared_summed_counts.begin(),
                       squared_summed_counts.end(),
                       squared_summed_counts.begin(),
                       squared_summed_counts.begin(),
                       std::multiplies<unsigned>());
        std::partial_sum(squared_summed_counts.begin(),
                         squared_summed_counts.end(),
                         squared_summed_counts.begin());
        squared_summed_counts.insert(squared_summed_counts.begin(), 0);
        // generate a vector taustar with indexes of the counts
        std::vector<unsigned> taustar;
        unsigned n = minseglen;
        std::generate_n(std::back_inserter(taustar),
                        num_features + minseglen - 2 - 2,
                        [n]() mutable { return n++; });
        std::vector<float> tmp1;
        std::vector<float> tmp2;
        std::vector<float> tmp3;
        const float last_count = static_cast<float>(squared_summed_counts.back());
        // perform tmp3 = (squared_summed_counts / last_count) - (taustar / num_counts)
        std::transform(squared_summed_counts.begin() + 2,
                       squared_summed_counts.end() - 1,
                       std::back_inserter(tmp1),
                       [=](unsigned count) { return count / last_count; });
        std::transform(taustar.begin(),
                       taustar.end(),
                       std::back_inserter(tmp2),
                       [=](unsigned tau_value)
        { return tau_value / static_cast<float>(num_features); });
        std::transform(tmp1.begin(),
                       tmp1.end(),
                       tmp2.begin(),
                       std::back_inserter(tmp3),
                       [](float a, float b) { return std::fabs(a - b); });
        // tau is the distance to the max element in tmp3 which is an index
        const auto tau = std::distance(tmp3.begin(), std::max_element(tmp3.begin(), tmp3.end()));
        // get the read count of the tau index and that is the gene cut off
        const unsigned est_readcount
            = *std::upper_bound(counts.begin(), counts.end(), counts.at(tau));
        gene->cut_off(est_readcount);
    }
}

void GeneRendererGL::initBasicBuffers()
{
    if (!m_vao.isCreated()) {
        m_vao.create();
    }
    m_vao.bind();

    m_shader_program.bind();

    // vertices buffer
    if (!m_vertexsBuffer.isCreated()) {
        m_vertexsBuffer.create();
        m_vertexsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_vertexsBuffer.bind();
    m_vertexsBuffer.allocate(m_geneData.m_vertices.constData(),
                             m_geneData.m_vertices.size() * 3 * sizeof(float));
    m_shader_program.enableAttributeArray("vertexAttr");
    m_shader_program.setAttributeBuffer("vertexAttr", GL_FLOAT, 0, 3);

    // indexes buffer
    if (!m_indexesBuffer.isCreated()) {
        m_indexesBuffer.create();
        m_indexesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_indexesBuffer.bind();
    m_indexesBuffer.allocate(m_geneData.m_indexes.constData(),
                             m_geneData.m_indexes.size() * 1 * sizeof(unsigned int));

    // textures buffer
    if (!m_texturesBuffer.isCreated()) {
        m_texturesBuffer.create();
        m_texturesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_texturesBuffer.bind();
    m_texturesBuffer.allocate(m_geneData.m_textures.constData(),
                              m_geneData.m_textures.size() * 2 * sizeof(float));
    m_shader_program.enableAttributeArray("textureAttr");
    m_shader_program.setAttributeBuffer("textureAttr", GL_FLOAT, 0, 2);

    m_vao.release();
    m_shader_program.release();

    m_vertexsBuffer.release();
    m_indexesBuffer.release();
    m_texturesBuffer.release();

    m_isDirtyStaticData = false;
}

void GeneRendererGL::initDynamicBuffers()
{
    if (!m_vao.isCreated()) {
        m_vao.create();
    }
    m_vao.bind();

    m_shader_program.bind();

    // color buffer
    if (!m_colorsBuffer.isCreated()) {
        m_colorsBuffer.create();
        m_colorsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_colorsBuffer.bind();
    m_colorsBuffer.allocate(m_geneData.m_colors.constData(),
                            m_geneData.m_colors.size() * 4 * sizeof(float));
    m_shader_program.enableAttributeArray("colorAttr");
    m_shader_program.setAttributeBuffer("colorAttr", GL_FLOAT, 0, 4);

    // selected buffer
    if (!m_selectedBuffer.isCreated()) {
        m_selectedBuffer.create();
        m_selectedBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_selectedBuffer.bind();
    m_selectedBuffer.allocate(m_geneData.m_selected.constData(),
                              m_geneData.m_selected.size() * 1 * sizeof(unsigned));
    m_shader_program.enableAttributeArray("selectedAttr");
    m_shader_program.setAttributeBuffer("selectedAttr", GL_UNSIGNED_INT, 0, 1);

    // visible buffer
    if (!m_visibleBuffer.isCreated()) {
        m_visibleBuffer.create();
        m_visibleBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_visibleBuffer.bind();
    m_visibleBuffer.allocate(m_geneData.m_visible.constData(),
                             m_geneData.m_visible.size() * 1 * sizeof(unsigned));
    m_shader_program.enableAttributeArray("visibleAttr");
    m_shader_program.setAttributeBuffer("visibleAttr", GL_UNSIGNED_INT, 0, 1);

    // reads buffer
    if (!m_readsBuffer.isCreated()) {
        m_readsBuffer.create();
        m_readsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }
    m_readsBuffer.bind();
    m_readsBuffer.allocate(m_geneData.m_reads.constData(),
                           m_geneData.m_reads.size() * 1 * sizeof(unsigned));
    m_shader_program.enableAttributeArray("readsAttr");
    m_shader_program.setAttributeBuffer("readsAttr", GL_UNSIGNED_INT, 0, 1);

    m_vao.release();
    m_shader_program.release();

    m_colorsBuffer.release();
    m_selectedBuffer.release();
    m_visibleBuffer.release();
    m_readsBuffer.release();

    m_isDirtyDynamicData = false;
}

unsigned GeneRendererGL::getMinReadsThreshold() const
{
    return m_thresholdReadsLower;
}

unsigned GeneRendererGL::getMaxReadsThreshold() const
{
    return m_thresholdReadsUpper;
}

unsigned GeneRendererGL::getMinGenesThreshold() const
{
    return m_thresholdGenesLower;
}

unsigned GeneRendererGL::getMaxGenesThreshold() const
{
    return m_thresholdGenesUpper;
}

unsigned GeneRendererGL::getMinTotalReadsThreshold() const
{
    return m_thresholdTotalReadsLower;
}

unsigned GeneRendererGL::getMaxTotalReadsThreshold() const
{
    return m_thresholdTotalReadsUpper;
}

void GeneRendererGL::updateSize()
{
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    for (const unsigned index : m_indexes) {
        // update size of the quad for only one feature
        // (all features of same index have same coordinates)
        const auto feature = m_geneInfoByIndex.find(index);
        Q_ASSERT(feature != m_geneInfoByIndex.end() && feature->second);
        m_geneData.updateQuadSize(index, feature->second->x(), feature->second->y(), m_size);
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

void GeneRendererGL::updateGene(const DataProxy::GenePtr gene)
{
    if (!gene) {
        return;
    }
    // get unique indexes from the gene
    IndexesList unique_indexes;
    auto range = m_geneInfoByGene.equal_range(gene);
    std::transform(range.first,
                   range.second,
                   std::inserter(unique_indexes, unique_indexes.end()),
                   [](GeneInfoByGeneMap::value_type &ele) { return ele.second; });
    updateVisual();
}

void GeneRendererGL::updateVisual()
{
    // call updateVisual with all the genes
    updateVisual(m_indexes);
}

void GeneRendererGL::updateVisual(const DataProxy::GeneList &geneList)
{
    // get unique indexes from the list of genes
    IndexesList unique_indexes;
    for (const auto &gene : geneList) {
        auto range = m_geneInfoByGene.equal_range(gene);
        std::transform(range.first,
                       range.second,
                       std::inserter(unique_indexes, unique_indexes.end()),
                       [](GeneInfoByGeneMap::value_type &ele) { return ele.second; });
    }

    // compute the rendering information for the selected genes
    updateVisual(unique_indexes);
}

void GeneRendererGL::updateVisual(const IndexesList &indexes)
{
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // we want to get the max and min value of the reads that are going
    // to be rendered to pass these values to the shaders to compute normalized colors
    m_localPooledMin = std::numeric_limits<unsigned>::max();
    m_localPooledMax = std::numeric_limits<unsigned>::min();

    // reset selection array that contains the selected features
    m_geneInfoSelectedFeatures.clear();

    // some visualization options
    const bool pooling_genes = m_poolingMode == PoolNumberGenes;
    const bool pooling_tpm = m_poolingMode == PoolTPMs;
    const bool isPooled = m_visualMode == DynamicRangeMode || m_visualMode == HeatMapMode;

    // iterate the indexes (spots) to compute the visual data by going trough all the
    // features (gene counts) in each spot
    foreach (const auto &index, indexes) {

        // check if spot's total reads/genes are inside the total reads/genes thresholds
        const unsigned total_reads_feature = m_geneInfoTotalReadsIndex.find(index)->second;
        const unsigned total_genes_feature = m_geneInfoTotalGenesIndex.find(index)->second;
        if (featureGenesOutsideRange(total_genes_feature)
            || featureTotalReadsOutsideRange(total_reads_feature)) {
            // set spot to not visible
            m_geneData.updateQuadVisible(index, false);
            continue;
        }

        // temp local variables to store the color of the spot
        QColor indexColor = Visual::DEFAULT_COLOR_GENE;
        unsigned indexValue = 0;
        unsigned indexValueGenes = 0;

        // iterate the genes in the spot to compute rendering data for an specific index (spot)
        auto range = m_geneInfoByIndex.equal_range(index);
        for (auto it = range.first; it != range.second; ++it) {
            const auto feature = it->second;
            Q_ASSERT(feature);
            // get the feature's gene
            auto gene = m_dataProxy->geneGeneObject(feature->gene());
            Q_ASSERT(gene);

            // get the gene status and the count
            const bool isSelected = gene->selected();
            const unsigned geneCutOff = gene->cut_off();
            const unsigned currentHits = feature->count();

            // check if the reads count of the gene in this spot are outside the threshold
            // or the gene is not selected
            if (featureReadsOutsideRange(currentHits)
                || (m_genes_cutoff && currentHits < geneCutOff) || !isSelected) {
                continue;
            }

            // update local variables for number of reads and genes
            indexValue += currentHits;
            ++indexValueGenes;

            // when the color of the new feature is different than the color
            // in the feature's index we do linear interpolation adjusted
            // by the number of genes in the feature to obtain the new color
            const QColor &featureColor = gene->color();
            if (indexColor != featureColor) {
                const float adjustment = 1.0 / indexValueGenes;
                indexColor = Math::lerp(adjustment, indexColor, featureColor);
            }
        }

        // we only show indexes where there is at least one gene-feature activated
        const bool visible = indexValueGenes > 0;

        // update pooled min-max to compute colors if applies
        if (isPooled && visible) {
            if (pooling_genes) {
                indexValue = indexValueGenes;
            } else if (pooling_tpm) {
                indexValue = Math::tpmNormalization<unsigned>(indexValue, total_reads_feature);
            }
            // only update the boundaries for color computation in pooled mode
            m_localPooledMin = std::min(indexValue, m_localPooledMin);
            m_localPooledMax = std::max(indexValue, m_localPooledMax);
        }

        // update rendering data arrays
        m_geneData.updateQuadReads(index, indexValue);
        m_geneData.updateQuadSelected(index, false);
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
    // Well, we have some duplicated code here but the problem
    // is that this function is invoked from the reg-exp selection tool.
    // We want to make the spots visible that contain genes present in the
    // search and we also want to select those spots
    IndexesList unique_indexes;
    for (const auto &gene : genes) {
        auto range = m_geneInfoByGene.equal_range(gene);
        std::transform(range.first,
                       range.second,
                       std::inserter(unique_indexes, unique_indexes.end()),
                       [](const GeneInfoByGeneMap::value_type &ele) { return ele.second; });
    }
    // we update the rendering data
    updateVisual(genes);
    // we select the spots that contain the genes
    selectSpots(unique_indexes, SelectionEvent::NewSelection);
}

void GeneRendererGL::setSelectionArea(const SelectionEvent *event)
{
    // get selection area
    const QuadTreeAABB aabb(event->path());

    // get selection mode
    const SelectionEvent::SelectionMode mode = event->mode();

    // get selected points from selection shape
    GeneInfoQuadTree::PointItemList pointList;
    m_geneInfoQuadTree.select(aabb, pointList);

    // create a list of indexes from the quadtree' points.
    IndexesList indexes;
    std::transform(pointList.begin(),
                   pointList.end(),
                   std::inserter(indexes, indexes.end()),
                   [](GeneInfoQuadTree::PointItemList::value_type &ele) { return ele.second; });

    // make the selection
    selectSpots(indexes, mode);
}

const DataProxy::FeatureList &GeneRendererGL::getSelectedFeatures() const
{
    return m_geneInfoSelectedFeatures;
}

void GeneRendererGL::selectSpots(const IndexesList &indexes,
                                 const SelectionEvent::SelectionMode &mode)
{
    if (!m_isInitialized) {
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // if new selection clear the current selection
    if (mode == SelectionEvent::NewSelection) {
        // unselect previous selection
        m_geneData.clearSelectionArray();
        m_geneInfoSelectedFeatures.clear();
    }

    // type of selection (add or remove)
    const bool remove_selection = (mode == SelectionEvent::ExcludeSelection);

    // iterate the points to get the features of each point and make
    // the selection
    for (const auto &index : indexes) {

        // do not select non-visible spots or spots that are already selected in ADD mode
        if (!m_geneData.quadVisible(index)
            || (m_geneData.quadSelected(index) && !remove_selection)) {
            continue;
        }

        // iterate all the features in the position to select when possible
        bool no_feature_selected = true;
        auto range = m_geneInfoByIndex.equal_range(index);
        for (auto it = range.first; it != range.second; ++it) {
            // not filtering if the feature's gene is selected
            // as we want to include in the selection all the genes
            // of the feature regardless if they are selected or not
            // we just filter features outside the threshold
            const auto feature = it->second;
            Q_ASSERT(feature);
            // get the feature's gene
            auto gene = m_dataProxy->geneGeneObject(feature->gene());
            Q_ASSERT(gene);
            const unsigned geneCutOff = gene->cut_off();
            const unsigned currentHits = feature->count();
            if (featureReadsOutsideRange(currentHits)
                || (m_genes_cutoff && currentHits < geneCutOff)) {
                continue;
            }

            // this means that at least one feature was selected
            no_feature_selected = false;

            // update the container with selected features
            if (!remove_selection) {
                m_geneInfoSelectedFeatures.push_back(feature);
            } else {
                m_geneInfoSelectedFeatures.erase(std::remove(m_geneInfoSelectedFeatures.begin(),
                                                             m_geneInfoSelectedFeatures.end(),
                                                             feature),
                                                 m_geneInfoSelectedFeatures.end());
            }
        }

        // update gene data to selected or not selected (spot)
        m_geneData.updateQuadSelected(index, !no_feature_selected && !remove_selection);
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

void GeneRendererGL::setColorComputingMode(const Visual::GeneColorMode mode)
{
    // update color computing mode
    if (m_colorComputingMode != mode) {
        m_colorComputingMode = mode;
        if (m_visualMode != NormalMode) {
            updateVisual();
        }
    }
}

void GeneRendererGL::slotSetGenesCutOff(bool enable)
{
    if (m_genes_cutoff != enable) {
        m_genes_cutoff = enable;
        updateVisual();
    }
}

void GeneRendererGL::draw(QOpenGLFunctionsVersion &qopengl_functions)
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

    qopengl_functions.glDrawElements(GL_TRIANGLES, m_geneData.m_indexes.size(), GL_UNSIGNED_INT, 0);

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

bool GeneRendererGL::featureReadsOutsideRange(const unsigned value)
{
    return (value < m_thresholdReadsLower || value > m_thresholdReadsUpper);
}

bool GeneRendererGL::featureGenesOutsideRange(const unsigned value)
{
    return (value < m_thresholdGenesLower || value > m_thresholdGenesUpper);
}

bool GeneRendererGL::featureTotalReadsOutsideRange(const unsigned value)
{
    return (value < m_thresholdTotalReadsLower || value > m_thresholdTotalReadsUpper);
}

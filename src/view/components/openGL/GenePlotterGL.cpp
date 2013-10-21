/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include "utils/MathExtended.h"
#include "utils/Utils.h"

#include <QSettings>

#include "GLScope.h"
#include "data/GLElementShapeFactory.h"
#include "data/GLElementRender.h"
#include "image/GLHeatMap.h"
#include "math/GLFloat.h"
#include "math/GLMatrix.h"
#include "shader/GLShaderRender.h"

#include "view/pages/CellViewPage.h"

#include "GenePlotterGL.h"

GenePlotterGL::GenePlotterGL(QGraphicsItem* parent)
    : QGraphicsObject(parent), m_geneProgram(0),
      m_colorScheme(0), m_visualMode(NormalMode),
      m_hitCountMin(0), m_hitCountMax(0), m_hitCountSum(0)
{
    reset();

    setCacheMode(QGraphicsItem::NoCache);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
}

GenePlotterGL::~GenePlotterGL()
{
    if (m_colorScheme)
    {
        delete m_colorScheme;
        m_colorScheme = 0;
    }
}

void GenePlotterGL::initGL()
{
    // load shaders
    if (!m_geneProgramList.contains(Globals::Circle))
    {
        loadShader(Globals::Circle, ":shader/geneCircle.vert", ":shader/geneCircle.frag");
    }
    if (!m_geneProgramList.contains(Globals::Cross))
    {
        loadShader(Globals::Cross, ":shader/geneCross.vert", ":shader/geneCross.frag");
    }
    if (!m_geneProgramList.contains(Globals::Square))
    {
        loadShader(Globals::Square, ":shader/geneSquare.vert", ":shader/geneSquare.frag");
    }
    // set default shader
    if (m_geneProgram == 0)
    {
        m_geneProgram = m_geneProgramList.value(Globals::Circle, 0);
    }
}

void GenePlotterGL::loadShader(const uint id, const QString &vertexPath, const QString &fragmentPath)
{
    QObject memoryGuard;

    QGLShaderProgram *program = new QGLShaderProgram(&memoryGuard);

    // compile shader, abort on error
    bool result;
    
    result = program->addShaderFromSourceFile(QGLShader::Vertex, vertexPath);
    
    if (!result)
    {
        qDebug() << QString("Error: Unable to compile vertex shader (%1)\n").arg(vertexPath) << program->log();
        return;
    }
    
    result = program->addShaderFromSourceFile(QGLShader::Fragment, fragmentPath);
    
    if (!result)
    {
        qDebug() << QString("Error: Unable to compile fragment shader (%1)\n").arg(fragmentPath) << program->log();
        return;
    }

    result = program->link();
    
    if (!result)
    {
        qDebug() << QString("Error: Unable to link shaders (%1 <-> %2)\n").arg(vertexPath).arg(fragmentPath) << program->log();
        return;
    }

    // store program
    program->setParent(this);
    m_geneProgramList[id] = program;
}

void GenePlotterGL::clear()
{
    // chip grid stuff
    setBoundingRect(QRectF());
    m_rect = QRectF();

    // grid data
    m_gridData.clear();

    // gene plot data
    m_geneData.clear();
    
    // selected genes data
    m_geneInfoSelection.clear();
    
    // lookup data
    m_geneInfo.clear();
    m_geneInfoById.clear();
    m_geneInfoReverse.clear();
    m_geneInfoByIdx.clear();
    m_geneInfoQuadTree.clear();
}

void GenePlotterGL::reset()
{
    // clear rendering data
    clear();
    
    // grid data
    m_gridLineSpace = Globals::grid_line_space;
    m_gridLineSize = Globals::grid_line_size;
    m_gridVisible = false;
    m_gridColor = Globals::color_grid;
    m_gridBorderColor = Globals::color_grid_border;
    
    // gene plot data
    m_geneVisible = true;
    m_geneShape = Globals::gene_shape;
    m_geneIntensity = static_cast<qreal>(Globals::gene_intensity / 10);
    m_geneSize = static_cast<qreal>(Globals::gene_size / 10);
    m_geneLimit = Globals::gene_limit;

    m_geneNameVisible = false;
    m_geneNameFont = QFont();
    m_geneNumberVisible = false;

    // allocate color scheme
    setGeneVisualMode(NormalMode);
}

QPainterPath GenePlotterGL::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool GenePlotterGL::contains(const QPointF& point) const
{
    return QGraphicsObject::contains(point);
}

void GenePlotterGL::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (painter->paintEngine()->type() != QPaintEngine::OpenGL &&
        painter->paintEngine()->type() != QPaintEngine::OpenGL2)
    {
        qDebug() << "GenePlotterGL: I need a QGLWidget to be set as viewport on the graphics view";
        return;
    }

    GL::GLElementRender simpleRenderer;
    GL::GLShaderRender shaderRenderer;
    shaderRenderer.shader(m_geneProgram);

    painter->beginNativePainting();
    {
        GL::GLscope glBlendScope(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        if (m_gridVisible)
        {
            GL::GLscope glLineSmooth(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glLineWidth(m_gridLineSize);

            simpleRenderer.render(m_gridData);
        }
        if (m_geneVisible)
        {
            // render using shader if valid
            if (m_geneProgram)
            {
                m_geneProgram->bind();
                shaderRenderer.render(m_geneData);
                m_geneProgram->release();
            }
            // or fall back to simple render
            else
            {
                simpleRenderer.render(m_geneData);
            }
        }

        glPopMatrix();
    }
    painter->endNativePainting();
}

bool GenePlotterGL::isObscuredBy(const QGraphicsItem* item) const
{
    return QGraphicsObject::isObscuredBy(item);
}

QPainterPath GenePlotterGL::opaqueArea() const
{
    return QGraphicsObject::opaqueArea();
}

void GenePlotterGL::setHitCount(int min, int max, int sum)
{
    m_hitCountMin = min;
    m_hitCountMax = max;
    m_hitCountSum = sum;
}

void GenePlotterGL::setSelectionArea(const SelectionEvent *event)
{
    GL::GLElementRectangleFactory factory(m_geneData);

    QRectF rect = event->path().boundingRect();
    GL::GLaabb aabb = GL::toGLaabb(rect);

    SelectionEvent::SelectionMode mode = event->mode();
    if (mode == SelectionEvent::NewSelection)
    {
        // unselect previous selecetion
        foreach(GeneInfoList::size_type index, m_geneInfoSelection)
        {
            const GL::GLindex vdi = m_geneInfo[index].vertexDataIndex;
            const GL::GLoption option = factory.getOption(vdi);
            factory.setOption(vdi, GL::bitClear(option, 0));
        }
        m_geneInfoSelection.clear();
    }

    GeneInfoQuadTree::PointItemList list;
    m_geneInfoQuadTree.select(aabb, list);

    DataProxy *dataProxy = DataProxy::getInstance();

    // select new selecetion
    GeneInfoQuadTree::PointItemList::const_iterator it, end = list.end();
    for (it = list.begin(); it != end; ++it)
    {
        const GeneInfoList::size_type index = it->second;
        const GL::GLindex vdi = m_geneInfo[index].vertexDataIndex;
        const int refCount = m_geneInfo[index].refCount;
        const GL::GLoption option = factory.getOption(vdi);

        // do not select non-visible features
        if (refCount <= 0)
        {
            continue;
        }

        if (mode == SelectionEvent::ExcludeSelection)
        {
            m_geneInfoSelection.remove(index);
            factory.setOption(vdi, GL::bitClear(option, 0));
        }
        else
        {
            m_geneInfoSelection.insert(index);
            factory.setOption(vdi, GL::bitSet(option, 0));
        }
    }
}

void GenePlotterGL::updateGeneColor(DataProxy::GenePtr gene)
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name());
    
    GL::GLElementRectangleFactory factory(m_geneData);

    // easy access
    const bool selected = gene->selected();

    foreach (DataProxy::FeaturePtr feature, (*features))
    {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature->id());
        Q_ASSERT(it != m_geneInfoById.end());
        LookupData *lookup = &(m_geneInfo[it.value()]);

        // easy access
        const GL::GLindex vdi = lookup->vertexDataIndex;

        // feature update color
        const QColor oldQColor = m_colorScheme->getColor(feature);
        const QColor geneQColor = gene->color();
        if (feature->color() != geneQColor)
        {
            feature->color(geneQColor);
        }
        const QColor newQColor = m_colorScheme->getColor(feature);

        const int refCount = (lookup->refCount);
        const GL::GLcolor oldColor = GL::toGLcolor(oldQColor);
        const GL::GLcolor newColor = GL::toGLcolor(newQColor);

        if (selected && (refCount > 0))
        {
            GL::GLcolor color = factory.getColor(vdi);
            if (refCount > 1)
            {
                color = GL::invlerp((1.0f / GLfloat(refCount)), color, oldColor);
            }
            color = GL::lerp((1.0f / GLfloat(refCount)), color, newColor);
            color.alpha = color.alpha
                * (feature->hits() < m_geneLimit ? 0.1f : 1.0f) // fade out features below hit count threshold
                * m_geneIntensity; // add alpha
            factory.setColor(vdi, color);
        }
    }

    update(boundingRect());
}
void GenePlotterGL::updateGeneSelection(DataProxy::GenePtr gene)
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name());
    
    GL::GLElementRectangleFactory factory(m_geneData);

    // easy access
    const bool selected = gene->selected();

    foreach (DataProxy::FeaturePtr feature, (*features))
    {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature->id());
        Q_ASSERT(it != m_geneInfoById.end());
        LookupData *lookup = &(m_geneInfo[it.value()]);

        // easy access
        const GL::GLindex vdi = lookup->vertexDataIndex;
        const GL::GLindex idi = lookup->indexDataIndex;

        // bump ref count
        const int oldRefCount = (lookup->refCount);
        const int newRefCount = (lookup->refCount += (selected ? 1 : -1));

        // update color
        GL::GLcolor color(GL::White);
        if (newRefCount != 0)
        {
            //NOTE in order to derive color we do a weighted addition or
            //     subtraction of feature color
            const GL::GLcolor featureColor = GL::toGLcolor(m_colorScheme->getColor(feature));
            color = factory.getColor(vdi);
            color = (selected) ?
                GL::lerp((1.0f / GLfloat(newRefCount)), color, featureColor) :
                GL::invlerp((1.0f / GLfloat(oldRefCount)), color, featureColor);
            color.alpha = color.alpha
                * (feature->hits() < m_geneLimit ? 0.1f : 1.0f) // fade out features below hit count threshold
                * m_geneIntensity; // add alpha
        }
        factory.setColor(vdi, color);

        // toggle visible
        if (selected && (newRefCount == 1))
        {
            // add index
            GL::GLindex index = factory.connect(vdi);

            // update lookup
            lookup->indexDataIndex = index;
            m_geneInfoByIdx.insert(GeneInfoByIdxKey(LookupData::IndexIndex, index), it.value());
        }
        // if toggle invisible
        else if (!selected && (newRefCount == 0))
        {
            factory.deconnect(idi);
            // update lookup
            m_geneInfoByIdx.remove(GeneInfoByIdxKey(LookupData::IndexIndex, idi));
            lookup->indexDataIndex = GL::INVALID_INDEX;

            // element data remove uses swap & pop, reflect change in lookup map
            const GL::GLindex lastTail = factory.size();
            if (idi != lastTail)
            {
                const GeneInfoByIdxKey key(LookupData::IndexIndex, lastTail);
                Q_ASSERT(m_geneInfoByIdx.contains(key));
                const GeneInfoList::size_type listIndex = m_geneInfoByIdx[key];
                LookupData& tailData = m_geneInfo[listIndex];
                m_geneInfoByIdx.insert(GeneInfoByIdxKey(LookupData::IndexIndex, idi), 
                                       m_geneInfoByIdx.take(GeneInfoByIdxKey(LookupData::IndexIndex, tailData.indexDataIndex)));
                tailData.indexDataIndex = idi;
            }
        }
    }

    update(boundingRect());
}

void GenePlotterGL::updateGeneData()
{
    // update rendering data
    rebuildGeneData();
    update(boundingRect());
}

void GenePlotterGL::updateChipSize()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    const QString chipId = (dataset != 0) ? dataset->chipId() : QString();
    DataProxy::ChipPtr currentChip = dataProxy->getChip(chipId);

    // early out
    if (currentChip.isNull())
    {
        return;
    }
    
    prepareGeometryChange();

    // resize grid according to chip size
    m_rect = QRectF(
        QPointF(currentChip->x1(), currentChip->y1()),
        QPointF(currentChip->x2(), currentChip->y2())
    );
    m_border = QRectF(
        QPointF(currentChip->x1Border(), currentChip->y1Border()),
        QPointF(currentChip->x2Border(), currentChip->y2Border())
    );

    // reflect bounds to quad tree
    m_geneInfoQuadTree.clear();
    m_geneInfoQuadTree = GeneInfoQuadTree(GL::toGLaabb(m_border));

    // update rendering data
    rebuildGridData();

    update(boundingRect());
}

void GenePlotterGL::updateTransformation()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());

    // early out
    if (dataset.isNull())
    {
        return;
    }

    //NOTE should this always be done?
    setTransform(dataset->alignment());
}

const QList<QString> GenePlotterGL::selectedFeatureList() const
{
    QList<QString> featureList;

    foreach(const GeneInfoList::size_type index, m_geneInfoSelection)
    {
        GeneInfoReverseMap::const_iterator it, end = m_geneInfoReverse.end();
        // hash map represent one to many connection. iterate over all matches.
        for (it = m_geneInfoReverse.find(index); (it != m_geneInfoReverse.end()) && (it.key() == index); ++it)
        {
            featureList << (it.value());
        }
    }
    return featureList;
}

void GenePlotterGL::selectAll(const DataProxy::GeneList &geneList)
{
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::FeatureList aggregateFeatureList;
    foreach (DataProxy::GenePtr gene, geneList)
    {
        aggregateFeatureList << *(dataProxy->getGeneFeatureList(dataProxy->getSelectedDataset(), gene->name()));
    }
    selectAll(aggregateFeatureList);
}
void GenePlotterGL::selectAll(const DataProxy::FeatureList &featureList)
{
    DataProxy *dataProxy = DataProxy::getInstance();

    GL::GLElementRectangleFactory factory(m_geneData);

    // unselect previous selecetion
    foreach(GeneInfoList::size_type index, m_geneInfoSelection)
    {
        const GL::GLindex vdi = m_geneInfo[index].vertexDataIndex;
        const GL::GLoption option = factory.getOption(vdi);
        factory.setOption(vdi, GL::bitClear(option, 0));
    }
    m_geneInfoSelection.clear();

    foreach(const DataProxy::FeaturePtr feature, featureList)
    {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature->id()), end = m_geneInfoById.end();
        if (it == end)
        {
                continue;
        }

        const GeneInfoList::size_type index = it.value();
        const GL::GLindex vdi = m_geneInfo[index].vertexDataIndex;
        const int refCount = m_geneInfo[index].refCount;
        const GL::GLoption option = factory.getOption(vdi);

        // do not select non-visible features
        if (refCount <= 0)
        {
            continue;
        }

        m_geneInfoSelection.insert(index);
        factory.setOption(vdi, GL::bitSet(option, 0));
    }
}

void GenePlotterGL::rebuildGridData()
{
    // clear data
    m_gridData.clear();
    m_gridData.setMode(GL_LINES);

    // build data
    generateGridData();
}

void GenePlotterGL::generateGridData()
{
    // generate borders
    const GL::GLcolor gridBorderColor = 0.5f * GL::toGLcolor(m_gridBorderColor);
    for (qreal y = m_border.top(); y <= m_border.bottom(); y += 1.0)
    {
        if (m_rect.top() <= y && y <= m_rect.bottom())
        {
            m_gridData
                .addPoint(GL::toGLpoint(m_border.left(), y))
                .addPoint(GL::toGLpoint(m_rect.left(), y))
                .addColor(GL::GLlinecolor(gridBorderColor))
                .connect(GL::GLlineindex())
                .addPoint(GL::toGLpoint(m_rect.right(), y))
                .addPoint(GL::toGLpoint(m_border.right(), y))
                .addColor(GL::GLlinecolor(gridBorderColor))
                .connect(GL::GLlineindex());
        }
        else
        {
            m_gridData
                .addPoint(GL::toGLpoint(m_border.left(), y))
                .addPoint(GL::toGLpoint(m_border.right(), y))
                .addColor(GL::GLlinecolor(gridBorderColor))
                .connect(GL::GLlineindex());
        }
    }
    for (qreal x = m_border.left(); x <= m_border.right(); x += 1.0)
    {
        if (m_rect.left() <= x && x <= m_rect.right())
        {
            m_gridData
                .addPoint(GL::toGLpoint(x, m_border.top()))
                .addPoint(GL::toGLpoint(x, m_rect.top()))
                .addColor(GL::GLlinecolor(gridBorderColor))
                .connect(GL::GLlineindex())
                .addPoint(GL::toGLpoint(x, m_rect.bottom()))
                .addPoint(GL::toGLpoint(x, m_border.bottom()))
                .addColor(GL::GLlinecolor(gridBorderColor))
                .connect(GL::GLlineindex());
        }
        else
        {
            m_gridData
                .addPoint(GL::toGLpoint(x, m_border.top()))
                .addPoint(GL::toGLpoint(x, m_border.bottom()))
                .addColor(GL::GLlinecolor(gridBorderColor))
                .connect(GL::GLlineindex());
        }
    }

    // generate grid
    const GL::GLcolor gridColor = 0.5f * GL::toGLcolor(m_gridColor);
    for (qreal y = m_rect.top(); y <= m_rect.bottom(); y += m_gridLineSpace)
    {
        m_gridData
            .addPoint(GL::toGLpoint(m_rect.left(),  y))
            .addPoint(GL::toGLpoint(m_rect.right(), y))
            .addColor(GL::GLlinecolor(gridColor))
            .connect(GL::GLlineindex());
    }
    for (qreal x = m_rect.left(); x <= m_rect.right(); x += m_gridLineSpace)
    {
        m_gridData
            .addPoint(GL::toGLpoint(x, m_rect.top()))
            .addPoint(GL::toGLpoint(x, m_rect.bottom()))
            .addColor(GL::GLlinecolor(gridColor))
            .connect(GL::GLlineindex());
    }
    if (!qFuzzyCompare(QtExt::qMod(m_rect.bottom() - m_rect.top(), m_gridLineSpace), 0.0))
    {
        m_gridData
            .addPoint(GL::toGLpoint(m_rect.left(), m_rect.bottom()))
            .addPoint(GL::toGLpoint(m_rect.right(), m_rect.bottom()))
            .addColor(GL::GLlinecolor(gridColor))
            .connect(GL::GLlineindex());
    }
    if (!qFuzzyCompare(QtExt::qMod(m_rect.right() - m_rect.left(), m_gridLineSpace), 0.0))
    {
        m_gridData
            .addPoint(GL::toGLpoint(m_rect.right(), m_rect.top()))
            .addPoint(GL::toGLpoint(m_rect.right(), m_rect.bottom()))
            .addColor(GL::GLlinecolor(gridColor))
            .connect(GL::GLlineindex());
    }
}

void GenePlotterGL::generateGeneData()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());

    const GL::GLflag flags =
        GL::GLElementShapeFactory::AutoAddColor |
        GL::GLElementShapeFactory::AutoAddTexture |
        GL::GLElementShapeFactory::AutoAddOption;
    GL::GLElementRectangleFactory factory(m_geneData, flags);
    
    factory.setSize((GLfloat) m_geneSize);

    // generate geometry
    foreach (const DataProxy::FeaturePtr feature, (*features))
    {
        const GL::GLpoint point = GL::toGLpoint(feature->x(), feature->y());

        LookupData *lookup = 0;

        // test if point already exists
        GeneInfoQuadTree::PointItem item = {point, (-1)};
        m_geneInfoQuadTree.select(point, item);
        if (item.second != -1)
        {
            lookup = &m_geneInfo[item.second];
            m_geneInfoById.insert(feature->id(), item.second);
            m_geneInfoReverse.insertMulti(item.second, feature->id());
        }
        // else insert point and create the link
        else
        {
            GL::GLindex vertexDataIndex = factory.addShape(point).index();

            GeneInfoList::size_type index = m_geneInfo.size();
            m_geneInfo.append(LookupData(vertexDataIndex, GL::INVALID_INDEX));

            m_geneInfoById.insert(feature->id(), index);
            m_geneInfoReverse.insert(index, feature->id());
            m_geneInfoByIdx.insert(GeneInfoByIdxKey(LookupData::DataIndex, vertexDataIndex), index);
            m_geneInfoQuadTree.insert(point, index);

            lookup = &m_geneInfo.back();
        }

        // bump ref count
        int featureCount = (++(lookup->featureCount));
        if (featureCount > 1)
        {
            // set complex option
            const GL::GLoption option = factory.getOption(item.second);
            factory.setOption(item.second, GL::bitSet(option, 1));
        }

        // set default color
        factory.setColor(lookup->vertexDataIndex, GL::GLcolor(GL::White));
    }
}

void GenePlotterGL::updateGeneVisualData()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());

    GL::GLElementRectangleFactory factory(m_geneData);

    // reset ref count
    GeneInfoList::iterator it, end = m_geneInfo.end();
    for (it = m_geneInfo.begin(); it != end; ++it)
    {
        it->refCount = 0;
    }

    // update features
    foreach (const DataProxy::FeaturePtr feature, (*features))
    {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature->id());
        Q_ASSERT(it != m_geneInfoById.end());
        LookupData *lookup = &(m_geneInfo[it.value()]);

        // easy access
        const GL::GLindex vdi = lookup->vertexDataIndex;
        const GL::GLindex idi = lookup->indexDataIndex;

        DataProxy::GenePtr gene = dataProxy->getGene(dataProxy->getSelectedDataset(), feature->gene());
        const bool selected = gene->selected();

        // bump ref count
        const int refCount = (lookup->refCount += (selected ? 1 : 0));

        // update color
        if (selected && (refCount > 0))
        {
            //NOTE in order to derive color we do a weighted addition or
            //     subtraction of feature color
            const GL::GLcolor featureColor = GL::toGLcolor(m_colorScheme->getColor(feature));
            GL::GLcolor color = factory.getColor(vdi);
            color = GL::lerp((1.0f / GLfloat(refCount)), color, featureColor);
            color.alpha = color.alpha
                * (feature->hits() < m_geneLimit ? 0.1f : 1.0f) // fade out features below hit count threshold
                * m_geneIntensity; // add alpha
            factory.setColor(vdi, color);
        }
    }
}

void GenePlotterGL::updateGeneSizeData()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::FeatureListPtr features = dataProxy->getFeatureList(dataProxy->getSelectedDataset());

    GL::GLElementRectangleFactory factory(m_geneData);

    // update features
    foreach (const DataProxy::FeaturePtr feature, (*features))
    {
        GeneInfoByIdMap::iterator it = m_geneInfoById.find(feature->id());
        Q_ASSERT(it != m_geneInfoById.end());
        LookupData *lookup = &(m_geneInfo[it.value()]);

        // easy access
        const GL::GLindex vdi = lookup->vertexDataIndex;

        const GLfloat x = feature->x(), y = feature->y();
        factory.setShape(vdi, GL::GLrectangle(x, y, m_geneSize));
    }
}

void GenePlotterGL::rebuildGeneData()
{
    // clear data
    m_geneData.clear(GL::GLElementData::Arrays & ~GL::GLElementData::IndexArray);
    m_geneData.setMode(GL_QUADS);
    
    generateGeneData();
}

GenePlotterGL::FeatureColor::FeatureColor() { }
QColor GenePlotterGL::FeatureColor::getColor(DataProxy::FeaturePtr feature) const
{
    return feature->color();
}

GenePlotterGL::DynamicRangeColor::DynamicRangeColor(int minHits, int maxHits)
    : m_minHits(minHits), m_maxHits(maxHits)
{ }

QColor GenePlotterGL::DynamicRangeColor::getColor(DataProxy::FeaturePtr feature) const
{
    const qreal v = qreal(feature->hits()), min = qreal(m_minHits), max = qreal(m_maxHits);
    const qreal nv = qSqrt(GL::norm<qreal,qreal>(v, min, max));

    QColor color = feature->color();
    color.setAlphaF(nv);
    return color;
}

GenePlotterGL::HeatMapColor::HeatMapColor(int minHits, int maxHits)
    : m_minHits(minHits), m_maxHits(maxHits)
{ 
    
}

QColor GenePlotterGL::HeatMapColor::getColor(DataProxy::FeaturePtr feature) const
{
    const GLfloat v = GLfloat(feature->hits()), min = GLfloat(m_minHits), max = GLfloat(m_maxHits);
    const GLfloat nv = GL::norm<GLfloat,GLfloat>(v, min, max);
    const GLfloat waveLength = GL::GLheatmap::generateHeatMapWavelength(nv, GL::GLheatmap::SpectrumExp);
    return GL::toQColor(GL::GLheatmap::createHeatMapColor(waveLength));
}

void GenePlotterGL::setGeneVisible(bool geneVisible)
{
    if (m_geneVisible != geneVisible)
    {
        m_geneVisible = geneVisible;
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneShape(int geneShape)
{
    const Globals::Shape shape = static_cast<Globals::Shape>(geneShape);
    if (m_geneShape != shape)
    {
        m_geneShape = shape;
        m_geneProgram = m_geneProgramList.value(shape, 0);
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneLimit(int geneLimit)
{
    if (m_geneLimit != geneLimit)
    {
        m_geneLimit = geneLimit;
        updateGeneVisualData();
        update(boundingRect());
    }
}

//input is expected to be >= 1 and <= 10
void GenePlotterGL::setGeneIntensity(int geneIntensity)
{
    qreal decimal = (qreal)(geneIntensity) / 10;
    if (!qFuzzyCompare(m_geneIntensity, decimal))
    {
        m_geneIntensity = decimal;
        updateGeneVisualData();
        update(boundingRect());
    }
}

//input is expected to be >= 10 and <= 30
void GenePlotterGL::setGeneSize(int geneSize)
{   
    qreal decimal = (qreal)(geneSize) / 10;
    if (!qFuzzyCompare(m_geneSize, decimal))
    {
        m_geneSize = decimal;
        updateGeneSizeData();
        update(boundingRect());
    }
}


void GenePlotterGL::setGeneVisualMode(const VisualMode mode)
{
    // early out
    if (m_visualMode == mode && m_colorScheme != 0)
    {
        return;
    }
    m_visualMode = mode;

    // set new color scheme deleting old if needed
    if (m_colorScheme)
    {
        delete m_colorScheme;
    }

    switch (m_visualMode)
    {
    case DynamicRangeMode:
        m_colorScheme = new DynamicRangeColor(m_hitCountMin, m_hitCountMax);
        break;
    case HeatMapMode:
        m_colorScheme = new HeatMapColor(m_hitCountMin, m_hitCountMax);
        break;
    case NormalMode:
    default:
        m_colorScheme = new FeatureColor();
        break;
    }

    // update visual data
    updateGeneVisualData();
    update(boundingRect());
}

void GenePlotterGL::setGridVisible(bool gridVisible)
{
    if (m_gridVisible != gridVisible)
    {
        m_gridVisible = gridVisible;
        update(boundingRect());
    }
}
void GenePlotterGL::setGridColor(const QColor& gridColor)
{   
    if (m_gridColor != gridColor)
    {
        m_gridColor = gridColor;
        rebuildGridData();
        update(boundingRect());
    }
}
void GenePlotterGL::setGridBorderColor(const QColor& gridBorderColor)
{
    if (m_gridBorderColor != gridBorderColor)
    {
        m_gridBorderColor = gridBorderColor;
        rebuildGridData();
        update(boundingRect());
    }
}
void GenePlotterGL::setGridLineSpace(qreal gridLineSpace)
{
    if (!qFuzzyCompare(m_gridLineSpace, gridLineSpace))
    {
        m_gridLineSpace = gridLineSpace;
        rebuildGridData();
        update(boundingRect());
    }
}
void GenePlotterGL::setGridLineSize(qreal gridLineSize)
{
    if (!qFuzzyCompare(m_gridLineSize, gridLineSize))
    {
        m_gridLineSize = gridLineSize;
        update(boundingRect());
    }
}

void GenePlotterGL::setBoundingRect(const QRectF& rect)
{
    if (m_border != rect)
    {
        prepareGeometryChange();
        m_border = rect;
    }
}

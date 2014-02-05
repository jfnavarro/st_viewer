/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GenePlotterGL.h"

#include <QDebug>
#include "utils/DebugHelper.h"
#include <QSettings>

#include "GLScope.h"
#include "data/GLElementShapeFactory.h"
#include "render/GLElementRender.h"
#include "render/GLShaderRender.h"
#include "QGLShaderProgram"
#include "GLCommon.h"

#include "viewPages/CellViewPage.h"

#include "GeneRendererGL.h"
#include "GridRendererGL.h"

GenePlotterGL::GenePlotterGL(QGraphicsItem* parent)
    : QGraphicsObject(parent), m_geneProgram(0)
{
    m_geneRenderer.reset(new GeneRendererGL());
    m_chipRenderer.reset(new GridRendererGL());
    reset();
    setCacheMode(QGraphicsItem::NoCache);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
}

GenePlotterGL::~GenePlotterGL()
{

}

void GenePlotterGL::initGL()
{
    // load shaders
    if (!m_geneProgramList.contains(Globals::Circle)) {
        loadShader(Globals::Circle, ":shader/geneCircle.vert", ":shader/geneCircle.frag");
    }
    if (!m_geneProgramList.contains(Globals::Cross)) {
        loadShader(Globals::Cross, ":shader/geneCross.vert", ":shader/geneCross.frag");
    }
    if (!m_geneProgramList.contains(Globals::Square)) {
        loadShader(Globals::Square, ":shader/geneSquare.vert", ":shader/geneSquare.frag");
    }
    // set default shader
    if (m_geneProgram == 0) {
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
    if (!result) {
        qDebug() << QString("Error: Unable to compile vertex shader (%1)\n").arg(vertexPath) << program->log();
        return;
    }
    result = program->addShaderFromSourceFile(QGLShader::Fragment, fragmentPath);
    if (!result) {
        qDebug() << QString("Error: Unable to compile fragment shader (%1)\n").arg(fragmentPath) << program->log();
        return;
    }
    result = program->link();
    if (!result) {
        qDebug() << QString("Error: Unable to link shaders (%1 <-> %2)\n").arg(vertexPath).arg(fragmentPath) << program->log();
        return;
    }
    // store program
    program->setParent(this);
    m_geneProgramList[id] = program;
}

void GenePlotterGL::clear()
{
    m_geneRenderer->clearData();
    m_chipRenderer->clearData();
}

void GenePlotterGL::reset()
{
    // clear rendering data
    clear();

    // grid data
    m_gridVisible = false;
    
    // gene plot data
    m_geneVisible = true;
    m_geneShape = Globals::gene_shape;
}

QPainterPath GenePlotterGL::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void GenePlotterGL::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (painter->paintEngine()->type() != QPaintEngine::OpenGL &&
        painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qDebug() << "GenePlotterGL: I need a QGLWidget to be set as viewport on the graphics view";
        return;
    }
    
    GL::GLElementRender simpleRenderer;
    GL::GLShaderRender shaderRenderer;
    shaderRenderer.shader(m_geneProgram);
    
    painter->beginNativePainting();
    {
        GL::GLscope glBlendScope(GL_BLEND);
        
        //alpha blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        
        if (m_gridVisible) {
            // use simple renderer for the grid
            //const GL::GLElementData &griddata = m_chipRenderer->getData();
            //const GL::GLElementRenderQueue &queue = m_chipRenderer->getCmds();
            //GL::GLscope glLineSmooth(GL_LINE_SMOOTH);
            //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            //glLineWidth(Globals::grid_line_size);
            //simpleRenderer.render(griddata,queue);
        }
        if (m_geneVisible) {
            // render using shader the genes
            const GL::GLElementDataGene &genedata = m_geneRenderer->getData();
            if (m_geneProgram && !genedata.isEmpty()) {
                m_geneProgram->bind();
                shaderRenderer.render(genedata);
                m_geneProgram->release();
            }
            else {
                qDebug() << "GenePlotterGL: Shader program is not valid!!";
            }
        }
        glPopMatrix();
    }
    painter->endNativePainting();
}

void GenePlotterGL::clearSelectionArea()
{
    //clear selected genes
    m_geneRenderer->clearSelection();
    update(boundingRect());
    //TOFIX expensive, refactor
    DataProxy::FeatureListPtr selectedFeatures = m_geneRenderer->getSelectedFeatures();
    emit featuresSelected(selectedFeatures);
}

void GenePlotterGL::setSelectionArea(const SelectionEvent *event)
{
    //select an area
    m_geneRenderer->setSelectionArea(event);
    update(boundingRect());
    DataProxy::FeatureListPtr selectedFeatures = m_geneRenderer->getSelectedFeatures();
    emit featuresSelected(selectedFeatures);
}

void GenePlotterGL::selectGenes(const DataProxy::GeneList &geneList)
{
    //select a list of genes
    m_geneRenderer->selectGenes(geneList);
    update(boundingRect());
    DataProxy::FeatureListPtr selectedFeatures = m_geneRenderer->getSelectedFeatures();
    emit featuresSelected(selectedFeatures);
}

DataProxy::FeatureListPtr GenePlotterGL::getSelectedFeatures()
{
    DataProxy::FeatureListPtr selectedFeatures = m_geneRenderer->getSelectedFeatures();
    return selectedFeatures;
}

void GenePlotterGL::updateGeneColor(DataProxy::GenePtr gene)
{
    m_geneRenderer->updateGene(gene, GeneRendererGL::geneColor);
    update(boundingRect());
}

void GenePlotterGL::updateGeneSelection(DataProxy::GenePtr gene)
{
    m_geneRenderer->updateGene(gene, GeneRendererGL::geneSelection);
    update(boundingRect());
}

void GenePlotterGL::setGeneVisible(bool geneVisible)
{
    if (m_geneVisible != geneVisible) {
        m_geneVisible = geneVisible;
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneShape(const Globals::Shape& shape)
{
    if (m_geneShape != shape) {
        m_geneShape = shape;
        m_geneProgram = m_geneProgramList.value(shape, 0);
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneLowerLimit(int geneLimit)
{
    if (m_geneRenderer->lowerLimit() != geneLimit) {
        m_geneRenderer->setLowerLimit(geneLimit);
        m_geneRenderer->updateData(GeneRendererGL::geneThreshold);
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneUpperLimit(int geneLimit)
{
    if(m_geneRenderer->upperLimit() != geneLimit) {
        m_geneRenderer->setUpperLimit(geneLimit);
        m_geneRenderer->updateData(GeneRendererGL::geneThreshold);
        update(boundingRect());
    }
}

void GenePlotterGL::setHitCountLimits(int min, int max, int sum)
{
    m_geneRenderer->setHitCount(min,max,sum);
    //m_geneRenderer->updateData(GeneRendererGL::All);
    //update(boundingRect());
}

void GenePlotterGL::setGeneIntensity(qreal geneIntensity)
{
    if (!qFuzzyCompare(m_geneRenderer->intensity(),geneIntensity)) {
        m_geneRenderer->setIntensity(geneIntensity);
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneSize(qreal geneSize)
{
    if (!qFuzzyCompare(m_geneRenderer->size(),geneSize)) {
        m_geneRenderer->setSize(geneSize);
        m_geneRenderer->updateData(GeneRendererGL::geneSize);
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneVisualMode(const Globals::VisualMode &mode)
{
    if (m_geneRenderer->visualMode() != mode) {
        m_geneRenderer->setVisualMode(mode);
        m_geneRenderer->updateData(GeneRendererGL::geneVisual);
        update(boundingRect());
    }
}

void GenePlotterGL::setGeneThresholdMode(const Globals::ThresholdMode &mode)
{
    if (m_geneRenderer->thresholdMode() != mode) {
        m_geneRenderer->setThresholdMode(mode);
        m_geneRenderer->updateData(GeneRendererGL::geneThreshold);
        update(boundingRect());
    }
}

void GenePlotterGL::setGridVisible(bool gridVisible)
{
    if (m_gridVisible != gridVisible) {
        m_gridVisible = gridVisible;
        update(boundingRect());
    }
}

void GenePlotterGL::setGridColor(const QColor& gridColor)
{
    if (m_chipRenderer->color() != gridColor) {
        m_chipRenderer->setColor(gridColor);
        //m_chipRenderer->updateData();
        update(boundingRect());
    }
}

QRectF GenePlotterGL::boundingRect() const
{
    return m_chipRenderer.isNull() ? QRectF() : m_chipRenderer->border();
}

void GenePlotterGL::updateChipSize()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    const QString chipId = (!dataset.isNull()) ? dataset->chipId() : QString();
    DataProxy::ChipPtr currentChip = dataProxy->getChip(chipId);
    
    // early out
    if (currentChip.isNull()) {
        return;
    }
    
    prepareGeometryChange();
    // resize grid according to chip size
    QRectF m_rect = QRectF(
                 QPointF(currentChip->x1(), currentChip->y1()),
                 QPointF(currentChip->x2(), currentChip->y2())
             );
    QRectF m_border = QRectF(
                   QPointF(currentChip->x1Border(), currentChip->y1Border()),
                   QPointF(currentChip->x2Border(), currentChip->y2Border())
               );
    
    // reflect bounds to quad tree
    m_geneRenderer->resetQuadTree(m_border);
    
    // update rendering data
    m_chipRenderer->clearData();
    m_chipRenderer->setDimensions(m_border,m_rect);
    m_chipRenderer->generateData();
    
    // update canvas
    update(boundingRect());
}

void GenePlotterGL::updateGeneData()
{
    m_geneRenderer->clearData();
    m_geneRenderer->generateData();
}

void GenePlotterGL::updateTransformation()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    // early out
    if (dataset.isNull()) {
        return;
    }
    //NOTE should this always be done?
    setTransform(dataset->alignment());
}

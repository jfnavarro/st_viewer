#include "GeneRendererGL.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QOpenGLShaderProgram>
#include <QImageReader>
#include <QApplication>

GeneRendererGL::GeneRendererGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent)
    : GraphicItemGL(parent)
    , m_rendering_settings(rendering_settings)
    , m_initialized(false)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, true);
    setAnchor(GraphicItemGL::Anchor::None);

    // initialize variables
    clearData();
}

GeneRendererGL::~GeneRendererGL()
{
}

void GeneRendererGL::clearData()
{
    m_initialized = false;
}

void GeneRendererGL::slotUpdate()
{
    if (m_initialized) {
        m_geneData->computeRenderingData();
        emit updated();
    }
}

void GeneRendererGL::attachData(QSharedPointer<STData> data)
{
    // update shader
    setupShaders();

    m_geneData = data;
    m_geneData->initRenderingData();
    m_initialized = true;
    m_border = m_geneData->getBorder();
}

// we update the rendering data
void GeneRendererGL::draw(QOpenGLFunctionsVersion &qopengl_functions)
{
    if (!m_initialized) {
        return;
    }

    m_shader_program.bind();

    const QMatrix4x4 projectionModelViewMatrix = getProjection() * getModelView();
    int projMatrix = m_shader_program.uniformLocation("in_ModelViewProjectionMatrix");
    int vertex = m_shader_program.attributeLocation("vertexAttr");
    int color = m_shader_program.attributeLocation("colorAttr");
    int texture = m_shader_program.attributeLocation("textureAttr");
    int selected = m_shader_program.attributeLocation("selected");

    // add UNIFORM values to shader program
    m_shader_program.setUniformValue(projMatrix, projectionModelViewMatrix);
    m_shader_program.setUniformValueArray(selected,
                                          reinterpret_cast<const GLuint*>(
                                              m_geneData->renderingSelected().constData()), 1);

    // Add arrays to the shader program
    m_shader_program.setAttributeArray(vertex,
                                       m_geneData->renderingVertices().constData());
    m_shader_program.setAttributeArray(color,
                                       m_geneData->renderingColors().constData());
    m_shader_program.setAttributeArray(texture, m_geneData->renderingTextures().constData());
    m_shader_program.enableAttributeArray(vertex);
    m_shader_program.enableAttributeArray(color);
    m_shader_program.enableAttributeArray(texture);

    qopengl_functions.glDrawElements(GL_TRIANGLES, m_geneData->renderingIndexes().size(),
                                     GL_UNSIGNED_INT,
                                     reinterpret_cast<const GLuint*>(
                                         m_geneData->renderingIndexes().constData()));

    m_shader_program.disableAttributeArray(vertex);
    m_shader_program.disableAttributeArray(color);
    m_shader_program.disableAttributeArray(texture);

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

const QRectF GeneRendererGL::boundingRect() const
{
    return m_border;
}

void GeneRendererGL::setSelectionArea(const SelectionEvent &event)
{
    m_geneData->selectSpots(event);
    slotUpdate();
}

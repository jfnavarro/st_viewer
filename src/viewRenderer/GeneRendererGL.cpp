#include "GeneRendererGL.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QOpenGLShaderProgram>
#include <QImageReader>
#include <QApplication>

GeneRendererGL::GeneRendererGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent)
    : GraphicItemGL(parent)
    , m_rendering_settings(rendering_settings)
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

}

//void GeneRendererGL::resetQuadTree(const QRectF &rect)
//{
//    m_geneInfoQuadTree.clear();
//    m_geneInfoQuadTree = GeneInfoQuadTree(rect);
//}

void GeneRendererGL::attachData(QSharedPointer<STData> data)
{
    clearData();

    // update shader
    setupShaders();

    m_geneData = data;
}

// we update the rendering data
void GeneRendererGL::draw(QOpenGLFunctionsVersion &qopengl_functions)
{
    Q_UNUSED(qopengl_functions)
    /*
    if (!m_isInitialized) {
        return;
    }

    m_shader_program.bind();

    const QMatrix4x4 projectionModelViewMatrix = getProjection() * getModelView();
    int visualMode = m_shader_program.uniformLocation("in_visualMode");
    int colorMode = m_shader_program.uniformLocation("in_colorMode");
    int poolingMode = m_shader_program.uniformLocation("in_poolingMode");
    int upperLimit = m_shader_program.uniformLocation("in_pooledUpper");
    int lowerLimit = m_shader_program.uniformLocation("in_pooledLower");
    int intensity = m_shader_program.uniformLocation("in_intensity");
    int shape = m_shader_program.uniformLocation("in_shape");
    int projMatrix = m_shader_program.uniformLocation("in_ModelViewProjectionMatrix");
    int counts = m_shader_program.attributeLocation("countAttr");
    int selected = m_shader_program.attributeLocation("selectedAttr");
    int visible = m_shader_program.attributeLocation("visibleAttr");
    int vertex = m_shader_program.attributeLocation("vertexAttr");
    int color = m_shader_program.attributeLocation("colorAttr");
    int texture = m_shader_program.attributeLocation("textureAttr");

    // add UNIFORM values to shader program
    m_shader_program.setUniformValue(visualMode, static_cast<GLint>(m_visualMode));
    m_shader_program.setUniformValue(colorMode, static_cast<GLint>(m_colorComputingMode));
    m_shader_program.setUniformValue(poolingMode, static_cast<GLint>(m_poolingMode));
    m_shader_program.setUniformValue(upperLimit, static_cast<GLint>(m_localPooledMax));
    m_shader_program.setUniformValue(lowerLimit, static_cast<GLint>(m_localPooledMin));
    m_shader_program.setUniformValue(intensity, static_cast<GLfloat>(m_intensity));
    m_shader_program.setUniformValue(shape, static_cast<GLint>(m_shape));
    m_shader_program.setUniformValue(projMatrix, projectionModelViewMatrix);

    // Add arrays to the shader program
    m_shader_program.setAttributeArray(counts,
                                       reinterpret_cast<const GLfloat*>(m_geneData.m_reads.constData()),
                                       1);
    m_shader_program.setAttributeArray(selected,
                                       reinterpret_cast<const GLfloat*>(m_geneData.m_selected.constData()),
                                       1);
    m_shader_program.setAttributeArray(visible,
                                       reinterpret_cast<const GLfloat*>(m_geneData.m_visible.constData()),
                                       1);
    m_shader_program.setAttributeArray(vertex,
                                       m_geneData.m_vertices.constData());
    m_shader_program.setAttributeArray(color,
                                       m_geneData.m_colors.constData());
    m_shader_program.setAttributeArray(texture, m_geneData.m_textures.constData());

    m_shader_program.enableAttributeArray(vertex);
    m_shader_program.enableAttributeArray(color);
    m_shader_program.enableAttributeArray(texture);
    m_shader_program.enableAttributeArray(counts);
    m_shader_program.enableAttributeArray(selected);
    m_shader_program.enableAttributeArray(visible);

    qopengl_functions.glDrawElements(GL_TRIANGLES, m_geneData.m_indexes.size(),
                                     GL_UNSIGNED_INT,
                                     reinterpret_cast<const GLuint*>(m_geneData.m_indexes.constData()));

    m_shader_program.disableAttributeArray(vertex);
    m_shader_program.disableAttributeArray(color);
    m_shader_program.disableAttributeArray(texture);
    m_shader_program.disableAttributeArray(counts);
    m_shader_program.disableAttributeArray(selected);
    m_shader_program.disableAttributeArray(visible);
    m_shader_program.release();
    */
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

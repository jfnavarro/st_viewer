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
/*
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
        m_geneInfoByIndex.insert(index, feature);
        // multiple indexes per gene
        m_geneInfoByGene.insert(gene, index);
        // mutiple count per gene
        m_geneInfoByGeneFeatures[gene].push_back(feature->count());

        // updated total reads/genes per spot/index
        const int feature_reads = feature->count();
        const int num_genes_spot = ++m_geneInfoTotalGenesIndex[index];
        const int num_reads_spot = m_geneInfoTotalReadsIndex[index] += feature_reads;

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
    m_isInitialized = true;
    */
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

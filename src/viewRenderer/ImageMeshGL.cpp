#include "ImageMeshGL.h"


#include <QByteArray>
#include <QApplication>
#include <QOpenGLFunctions>
#include <QFile>
#include <QFileInfo>

ImageMeshGL::ImageMeshGL()
    : m_indexBuf(QOpenGLBuffer::IndexBuffer)
    , m_posBuf(QOpenGLBuffer::VertexBuffer)
    , m_normBuf(QOpenGLBuffer::VertexBuffer)
    , m_program(nullptr)
    , m_num_triangles(0)
    , m_isInitialized(false)
{

}

ImageMeshGL::~ImageMeshGL()
{
    clearData();
    delete m_program;
    m_program = nullptr;
}

void ImageMeshGL::init()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();

    // Compile shaders
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/meshShader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/meshShader.frag");
    m_program->link();
}

void ImageMeshGL::clearData()
{
    m_vao.destroy();
    m_posBuf.destroy();
    m_indexBuf.destroy();
    m_normBuf.destroy();
    m_isInitialized = false;
    m_num_triangles = 0;
}

void ImageMeshGL::draw(const QMatrix4x4 &mvp_matrx)
{
    if (!m_isInitialized) {
        return;
    }

    m_program->bind();
    m_program->setUniformValue("mvp_matrix", mvp_matrx);
    m_program->setUniformValue("color", QVector4D(0.9f, 0.9f, 0.9f, 0.5f));
    {
        m_vao.bind();
        //draw
        glDrawElements(GL_TRIANGLES, m_num_triangles, GL_UNSIGNED_INT, 0);
        m_vao.release();
    }
    m_program->release();
}

void ImageMeshGL::loadMesh(const QString &meshFile)
{
    qDebug() << "Loading mesh " << meshFile;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normal;
    QVector<GLuint> indices;
    QFile file(meshFile);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            if (!line.startsWith("#")) {
                fields = line.split(" ");
                if (fields.at(0) == "v") {
                    vertices.append(QVector3D(fields.at(1).toFloat(),
                                              fields.at(2).toFloat(),
                                              fields.at(3).toFloat()));
                } else if (fields.at(0) == "vn") {
                    normal.append(QVector3D(fields.at(1).toFloat(),
                                            fields.at(2).toFloat(),
                                            fields.at(3).toFloat()));

                } else if (fields.at(0) == "f") {
                    indices.append(fields.at(1).split("//").at(0).toInt());
                    indices.append(fields.at(2).split("//").at(0).toInt());
                    indices.append(fields.at(3).split("//").at(0).toInt());
                } else {
                    //Ignoring other types
                }
            }
        }
    }
    m_num_triangles = indices.size();
    qDebug() << "Loaded mesh with  " << vertices.size() / 3
             << " triangles and " << m_num_triangles << " faces";

    m_program->bind();

    m_vao.create();
    m_vao.bind();

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuf.allocate(indices.data(), indices.size() *sizeof(GLuint));

    // Transfer vertex data to VBO 0
    m_posBuf.create();
    m_posBuf.bind();
    m_posBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_posBuf.allocate(vertices.constData(), vertices.count() * sizeof(QVector3D));
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    // Transfer normal data to VBO 1
    m_normBuf.create();
    m_normBuf.bind();
    m_normBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_normBuf.allocate(normal.constData(), normal.count() * sizeof(QVector3D));
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

    m_posBuf.release();
    m_normBuf.release();
    m_vao.release();
    m_program->release();

    m_isInitialized = true;
}

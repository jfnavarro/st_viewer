#include "ImageTextureGL.h"

#include <QOpenGLTexture>
#include <QImage>
#include <QtConcurrent>
#include <QFuture>
#include <QByteArray>
#include <QBuffer>
#include <QApplication>
#include <QImageReader>
#include <QOpenGLFunctions>

#include <cmath>


ImageTextureGL::ImageTextureGL()
    : m_program(nullptr)
    , m_isInitialized(false)
{

}

ImageTextureGL::~ImageTextureGL()
{
    clearData();
    delete m_program;
}

void ImageTextureGL::init()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();

    // Compile shaders
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/imageShader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/imageShader.frag");
    m_program->link();
}

void ImageTextureGL::clearData()
{
    clearTextures();
    m_vao.destroy();
    m_textures_pos.clear();
    m_texture_coords.clear();
    m_coordBuf.destroy();
    m_posBuf.destroy();
    m_isInitialized = false;
}

void ImageTextureGL::clearTextures()
{
    for (QOpenGLTexture *texture : m_textures) {
        if (texture != nullptr) {
            texture->destroy();
        }
        texture = nullptr;
    }
    m_textures.clear();
}

void ImageTextureGL::draw(const QMatrix4x4 &mvp_matrx)
{
    if (!m_isInitialized) {
        return;
    }

    m_program->bind();
    m_program->setUniformValue("mvp_matrix", mvp_matrx);
    m_program->setUniformValue("tex", 0);
    {
        m_vao.bind();
        for (int i = 0; i < m_textures.size(); ++i) {
            QOpenGLTexture *texture = m_textures[i];
            Q_ASSERT(texture != nullptr);
            texture->bind(0);
            glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
        }
        m_vao.release();
    }
    m_program->release();
}

void ImageTextureGL::createTiles(const QVector<QPair<QImage, QPoint> > &tiles)
{
    for (const auto &tile : tiles) {
        // create texture from the tile
        addTexture(tile.first, tile.second.x(), tile.second.y());
    }

    m_program->bind();

    m_vao.create();
    m_vao.bind();

    // Transfer vertex data to VBO 0
    m_posBuf.create();
    m_posBuf.bind();
    m_posBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_posBuf.allocate(m_textures_pos.constData(), m_textures_pos.size() * sizeof(QVector3D));
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    // Transfer texture data to VBO 1
    m_coordBuf.create();
    m_coordBuf.bind();
    m_coordBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_coordBuf.allocate(m_texture_coords.constData(), m_texture_coords.size() * sizeof(QVector2D));
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

    m_posBuf.release();
    m_coordBuf.release();
    m_vao.release();
    m_program->release();

    m_isInitialized = true;
}

void ImageTextureGL::addTexture(const QImage &image, const int x, const int y)
{
    const float width = static_cast<float>(image.width());
    const float height = static_cast<float>(image.height());

    m_textures_pos.append(QVector3D(x + width, y + height, 0.0));
    m_textures_pos.append(QVector3D(x + width, y, 0.0));
    m_textures_pos.append(QVector3D(x, y + height, 0.0));
    m_textures_pos.append(QVector3D(x, y, 0.0));

    m_texture_coords.append(QVector2D(1.0, 1.0));
    m_texture_coords.append(QVector2D(1.0, 0.0));
    m_texture_coords.append(QVector2D(0.0, 1.0));
    m_texture_coords.append(QVector2D(0.0, 0.0));

    QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setData(image);
    m_textures.append(texture);
}

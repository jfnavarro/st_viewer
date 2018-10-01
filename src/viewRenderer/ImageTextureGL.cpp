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

static const int tile_width = 256;
static const int tile_height = 256;

ImageTextureGL::ImageTextureGL()
    : m_program(nullptr)
    , m_isInitialized(false)
    , m_iscaled(false)
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
    m_iscaled = false;
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

bool ImageTextureGL::createTiles(const QString &imagefile)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    // image buffer reader
    QImageReader imageReader(imagefile);
    // scale image to half for big images
    QSize imageSize = imageReader.size();
    if (imageSize.width() >= 10000 || imageSize.height() >= 10000) {
        imageSize /= 2;
        imageReader.setScaledSize(imageSize);
        m_iscaled = true;
    } else {
        m_iscaled = false;
    }
    // parse the image
    QImage image;
    const bool read_ok = imageReader.read(&image);
    if (!read_ok) {
        qDebug() << "Tissue image cannot be opened/read" << imageReader.errorString();
        QGuiApplication::restoreOverrideCursor();
        return false;
    }

    m_bounds = image.rect();

    // compute tiles size and numbers
    const int width = image.width();
    const int height = image.height();
    const int xCount = std::ceil(width / static_cast<double>(tile_width));
    const int yCount = std::ceil(height / static_cast<double>(tile_height));
    const int count = xCount * yCount;

    // create tiles and their textures
    for (int i = 0; i < count; ++i) {

        // texture sizes
        const int x = tile_width * (i % xCount);
        const int y = tile_height * (i / xCount);
        const int texture_width = std::min(width - x, tile_width);
        const int texture_height = std::min(height - y, tile_height);

        // create sub image and add texture
        // TODO an ideal solution would  be to extract the clip rect part of the image
        // from the imageReader to avoid loading the whole image into memory
        // but the setClipRect option would only work one time, after calling read()
        // the buffer is cleaned
        addTexture(image.copy(x, y, texture_width, texture_height), x, y);
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

    QGuiApplication::restoreOverrideCursor();
    m_isInitialized = true;
    return true;
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

QRectF ImageTextureGL::boundingRect() const
{
    return m_bounds;
}

bool ImageTextureGL::scaled() const
{
    return m_iscaled;
}

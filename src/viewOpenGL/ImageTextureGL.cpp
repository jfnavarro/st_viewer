#include "ImageTextureGL.h"

#include <QOpenGLTexture>
#include <QImage>
#include <QtConcurrent>
#include <QFuture>
#include <QByteArray>
#include <QBuffer>
#include <QApplication>
#include <QImageReader>
#include <cmath>

static const int tile_width = 512;
static const int tile_height = 512;

ImageTextureGL::ImageTextureGL(QObject* parent)
    : GraphicItemGL(parent)
    , m_intensity(1.0)
    , m_isInitialized(false)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, false);
}

ImageTextureGL::~ImageTextureGL()
{
    clearData();
}

void ImageTextureGL::clearData()
{
    clearTextures();
    m_textures_indices.clear();
    m_texture_coords.clear();
    m_isInitialized = false;
}

void ImageTextureGL::clearTextures()
{
    foreach (QOpenGLTexture* texture, m_textures) {
        if (texture != nullptr) {
            texture->destroy();
        }
        texture = nullptr;
    }

    m_textures.clear();
}

void ImageTextureGL::doDraw(QOpenGLFunctionsVersion& qopengl_functions)
{
    if (!m_isInitialized) {
        return;
    }

    qopengl_functions.glEnable(GL_TEXTURE_2D);
    {
        qopengl_functions.glVertexPointer(2, GL_FLOAT, 0, m_textures_indices.constData());
        qopengl_functions.glTexCoordPointer(2, GL_FLOAT, 0, m_texture_coords.constData());
        qopengl_functions.glEnableClientState(GL_VERTEX_ARRAY);
        qopengl_functions.glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        for (int i = 0; i < m_textures.size(); ++i) {
            QOpenGLTexture* texture = m_textures[i];
            Q_ASSERT(texture != nullptr);
            texture->bind();
            qopengl_functions.glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
            texture->release();
        }

        qopengl_functions.glDisableClientState(GL_VERTEX_ARRAY);
        qopengl_functions.glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    qopengl_functions.glDisable(GL_TEXTURE_2D);
}

void ImageTextureGL::setSelectionArea(const SelectionEvent*)
{
}

QFuture<void> ImageTextureGL::createTexture(const QByteArray& imageByteArray)
{
    // clear memory
    clearData();
    return QtConcurrent::run(this, &ImageTextureGL::createTiles, imageByteArray);
}

void ImageTextureGL::createTiles(QByteArray imageByteArray)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    // extract image from byte array
    QBuffer imageBuffer(&imageByteArray);
    if (!imageBuffer.open(QIODevice::ReadOnly)) {
        qDebug() << "[ImageTextureGL] Image decoding buffer error:" << imageBuffer.errorString();
        return;
    }

    // create image from byte array
    QImageReader imageReader;
    imageReader.setDevice(&imageBuffer);
    QImage image;
    const bool readOk = imageReader.read(&image);
    imageBuffer.close();
    if (!readOk || image.isNull()) {
        qDebug() << "[ImageTextureGL] Opening image failed";
        return;
    }

    // get size and bounds
    const QSize imageSize = image.size();
    m_bounds = image.rect();

    // compute tiles size and numbers
    const int width = imageSize.width();
    const int height = imageSize.height();
    const int xCount = std::ceil(qreal(width) / qreal(tile_width));
    const int yCount = std::ceil(qreal(height) / qreal(tile_height));
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

    m_isInitialized = true;
    QGuiApplication::restoreOverrideCursor();
}

void ImageTextureGL::addTexture(const QImage& image, const int x, const int y)
{
    const qreal width = static_cast<qreal>(image.width());
    const qreal height = static_cast<qreal>(image.height());

    m_textures_indices.append(QVector2D(x, y));
    m_textures_indices.append(QVector2D(x + width, y));
    m_textures_indices.append(QVector2D(x + width, y + height));
    m_textures_indices.append(QVector2D(x, y + height));

    m_texture_coords.append(QVector2D(0.0, 0.0));
    m_texture_coords.append(QVector2D(1.0, 0.0));
    m_texture_coords.append(QVector2D(1.0, 1.0));
    m_texture_coords.append(QVector2D(0.0, 1.0));

    QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setData(image);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textures.append(texture);
}

const QRectF ImageTextureGL::boundingRect() const
{
    return m_bounds;
}

void ImageTextureGL::setIntensity(qreal intensity)
{
    if (m_intensity != intensity) {
        m_intensity = intensity;
        emit updated();
    }
}

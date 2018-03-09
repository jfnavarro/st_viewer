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

ImageTextureGL::ImageTextureGL(QObject *parent)
    : GraphicItemGL(parent)
    , m_isInitialized(false)
    , m_iscaled(false)
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
    for (QOpenGLTexture *texture : m_textures) {
        if (texture != nullptr) {
            texture->destroy();
        }
        texture = nullptr;
    }
    m_textures.clear();
}

void ImageTextureGL::draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter)
{
    Q_UNUSED(painter);

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
            QOpenGLTexture *texture = m_textures[i];
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

QFuture<void> ImageTextureGL::createTextures(const QString &imagefile)
{
    return QtConcurrent::run(this, &ImageTextureGL::createTiles, imagefile);
}

void ImageTextureGL::createGrid(const QImage &image, const int offset)
{
    const QImage gray_scale = image.convertToFormat(QImage::Format_Grayscale8);
    const int x_pixels = gray_scale.width();
    const int y_pixels = gray_scale.height();
    m_grid_points.clear();
    for (int x = 0; x < x_pixels; x+=offset) {
        for (int y = 0; y < y_pixels; y+=offset) {
            const float value = gray_scale.pixelColor(x,y).valueF();
            if (value > 0.5) {
               m_grid_points.append(QPointF(x,y));
            }
        }
    }
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
    const int width = imageSize.width();
    const int height = imageSize.height();
    const int xCount = std::ceil(width / static_cast<float>(tile_width));
    const int yCount = std::ceil(height / static_cast<float>(tile_height));
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

    QGuiApplication::restoreOverrideCursor();
    m_isInitialized = true;
    return true;
}

void ImageTextureGL::addTexture(const QImage &image, const int x, const int y)
{
    const float width = static_cast<float>(image.width());
    const float height = static_cast<float>(image.height());

    m_textures_indices.append(QVector2D(x, y));
    m_textures_indices.append(QVector2D(x + width, y));
    m_textures_indices.append(QVector2D(x + width, y + height));
    m_textures_indices.append(QVector2D(x, y + height));

    m_texture_coords.append(QVector2D(0.0, 0.0));
    m_texture_coords.append(QVector2D(1.0, 0.0));
    m_texture_coords.append(QVector2D(1.0, 1.0));
    m_texture_coords.append(QVector2D(0.0, 1.0));

    QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setData(image);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_textures.append(texture);
}

const QList<QPointF>& ImageTextureGL::getGrid() const
{
    return m_grid_points;
}

const QRectF ImageTextureGL::boundingRect() const
{
    return m_bounds;
}

void ImageTextureGL::setSelectionArea(const SelectionEvent &event)
{
    Q_UNUSED(event)
}

bool ImageTextureGL::scaled() const
{
    return m_iscaled;
}

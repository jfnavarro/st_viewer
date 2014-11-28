/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ImageTextureGL.h"

#include <QGLBuilder>
#include <QGeometryData>
#include <QGLMaterial>
#include <QImage>
#include <QGLPainter>
#include <QtConcurrent>
#include <QFuture>
#include <QByteArray>
#include <QBuffer>

#include "utils/QJpegImageReader.h"

#include <cmath>

ImageTextureGL::ImageTextureGL(QObject *parent) :
    GraphicItemGL(parent), m_intensity(1.0)
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
    clearNodes();
}

void ImageTextureGL::clearTextures()
{
    foreach(QGLTexture2D *texture, m_textures) {
        if (texture) {
            texture->cleanupResources();
            texture->release();
            texture->clearImage();
            texture->deleteLater();
        }
        texture = nullptr;
    }

    m_textures.clear();
}

void ImageTextureGL::clearNodes()
{
    foreach(QGLSceneNode *node, m_nodes) {
        if (node) {
            node->deleteLater();
        }
        node = nullptr;
    }

    m_nodes.clear();
}

void ImageTextureGL::draw(QGLPainter *painter)
{
    glEnable(GL_TEXTURE_2D);
    {
        foreach(QGLSceneNode *node, m_nodes ) {
            if (node && node->material() && node->material()->texture()) {
                node->material()->texture()->bind();
                QColor texture_color = Qt::black;
                texture_color.setAlphaF(m_intensity);
                node->material()->setColor(texture_color);
                node->draw(painter);
                node->material()->texture()->release();
            }
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void ImageTextureGL::setSelectionArea(const SelectionEvent *)
{

}

QFuture<void> ImageTextureGL::createTexture(const QByteArray &imageByteArray)
{
    //clear memory
    clearData();
    return QtConcurrent::run(this, &ImageTextureGL::createTiles, imageByteArray);
}

void ImageTextureGL::createTiles(QByteArray imageByteArray)
{
    static const int tile_width = 1024;
    static const int tile_height = 1024;

    //extract image from byte array
    QBuffer imageBuffer(&imageByteArray);
    if (!imageBuffer.open(QIODevice::ReadOnly)) {
        qDebug() << "[ImageTextureGL] Image decoding buffer error:" << imageBuffer.errorString();
        return;
    }

    //create image from byte array
    QJpegImageReader imageReader;
    imageReader.setDevice(&imageBuffer);
    QImage image;
    const bool readOk = imageReader.read(&image);
    if (!readOk || image.isNull()) {
        qDebug() << "[ImageTextureGL] Created image failed ";
        return;
    }
    imageBuffer.close();

    //get size and bounds
    const QSize imageSize = image.size();
    m_bounds = image.rect();

    //compute tiles size and numbers
    const int width = imageSize.width();
    const int height = imageSize.height();
    const int xCount = std::ceil(qreal(width) / qreal(tile_width));
    const int yCount = std::ceil(qreal(height) / qreal(tile_height));
    const int count = xCount * yCount;

    QImage sub_image;
    QRect clip_rect;
    for (int i = 0; i < count; ++i) {

        // texture sizes
        const int x = tile_width * (i % xCount);
        const int y = tile_height * (i / xCount);
        const int texture_width = std::min(width -  x, tile_width);
        const int texture_height = std::min(height -  y, tile_height);

        // create sub image
        clip_rect.setRect(x, y, texture_width, texture_height);
        //TODO an ideal solution would  be to extract the clip rect part of the image
        //from the imageReader to avoid loading the whole image into memory
        //but the setClipRect option would only work one time, after calling read()
        //the buffer is cleaned
        sub_image = image.copy(clip_rect);

        // add texture
        addTexture(sub_image, x, y);
    }
}

void ImageTextureGL::addTexture(const QImage& image, const int x, const int y)
{
    const qreal width = qreal(image.width());
    const qreal height = qreal(image.height());

    QGLBuilder builder;
    QGeometryData data;

    QVector2D a(x, y);
    QVector2D b(x + width, y);
    QVector2D c(x + width, y + height);
    QVector2D d(x, y + height);
    QVector2D ta(0.0, 0.0);
    QVector2D tb(1.0, 0.0);
    QVector2D tc(1.0, 1.0);
    QVector2D td(0.0, 1.0);

    data.appendVertex(a, b, c, d);
    data.appendTexCoord(ta, tb, tc, td);

    builder.addQuads(data);
    QGLSceneNode *node  = builder.finalizedSceneNode();

    QGLTexture2D *m_texture = new QGLTexture2D();
    m_texture->setImage(image);
    m_texture->setVerticalWrap(QGL::ClampToEdge);
    m_texture->setHorizontalWrap(QGL::ClampToEdge);
    m_texture->setBindOptions(QGLTexture2D::NoBindOption);
    m_texture->setSize(QSize(width, height));

    QGLMaterial *mat = new QGLMaterial();
    mat->setColor(Qt::black);
    mat->setTexture(m_texture);
    node->setMaterial(mat);
    node->setEffect(QGL::LitDecalTexture2D);
    m_nodes.append(node);
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

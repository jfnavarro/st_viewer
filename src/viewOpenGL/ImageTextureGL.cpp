/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include "ImageTextureGL.h"
#include "TexturedQuads.h"
#include "ColoredLines.h"
#include "Renderer.h"

#include <QOpenGLTexture>
#include <QImage>
#include <QtConcurrent>
#include <QFuture>
#include <QByteArray>
#include <QBuffer>
#include <QApplication>
#include <algorithm>
#include <cmath>

namespace
{
// Returns the name to use for a tile at point p (x,y).
inline QString getTileName(const QPoint& p)
{
    return QString("%1_%2t").arg(p.x(), p.y());
}
}

ImageTextureGL::ImageTextureGL(QObject* parent)
    : GraphicItemGL(parent)
    , m_bounds()
    , m_intensity(1.0)
    , m_outlineTextureTiles(false)
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
}

void ImageTextureGL::setImage(Renderer& renderer, const QImage& image)
{
    resetToNoImage(renderer);
    storeImageTiles(renderer, image);
}

void ImageTextureGL::resetToNoImage(Renderer& renderer)
{
    auto removeNamedTiles
        = [&renderer](QRect tileRect) { renderer.removeTexture(getTileName(tileRect.topLeft())); };

    forEachTile(m_bounds.size(), removeNamedTiles);

    m_bounds = QRect();
}

void ImageTextureGL::doDraw(Renderer& renderer)
{
    const int grey = static_cast<int>(255.0 * m_intensity);
    const QColor intensity(grey, grey, grey);

    auto drawTile = [&renderer, intensity, this](QRect tileRect) {
        TexturedQuads quad;
        quad.addQuad(tileRect, TexturedQuads::defaultTextureCoords(), intensity);
        drawTexturedQuads(renderer, quad, getTileName(tileRect.topLeft()));

        // Set m_outlineTextureTiles to true to display the tiles.
        if (m_outlineTextureTiles) {
            ColoredLines outline;
            outline.addRectOutline(tileRect, Qt::yellow);
            drawLines(renderer, outline);
        }
    };

    forEachTile(m_bounds.size(), drawTile);
}

// Subdivides the image into tiles, and stores each sub image as a named texture in the renderer.
void ImageTextureGL::storeImageTiles(Renderer& renderer, const QImage& image)
{
    m_bounds = image.rect();

    auto makeSubImageForTile = [&renderer, &image, this](QRect tileRect) {
        const QImage subImage = image.copy(tileRect);
        const QString name = getTileName(tileRect.topLeft());
        renderer.addTexture(name, subImage.mirrored(), false);
    };

    forEachTile(image.size(), makeSubImageForTile);
}

void ImageTextureGL::forEachTile(const QSize& imageSize, std::function<void(QRect)> tileFn)
{
    const int tileMaxWidth = ImageTextureGL::TextureTileWidth;
    const int tileMaxHeight = ImageTextureGL::TextureTileHeight;

    const int imgWidth = imageSize.width();
    const int imgHeight = imageSize.height();

    const int tilesInX = std::ceil(qreal(imgWidth) / tileMaxWidth);
    const int tilesInY = std::ceil(qreal(imgHeight) / tileMaxHeight);

    for (int ty = 0; ty < tilesInY; ++ty) {
        for (int tx = 0; tx < tilesInX; ++tx) {

            const int tileX = tx * tileMaxWidth;
            const int tileY = ty * tileMaxHeight;

            const int tileWidth = std::min(imgWidth - tileX, tileMaxHeight);
            const int tileHeight = std::min(imgHeight - tileY, tileMaxHeight);

            tileFn(QRect(tileX, tileY, tileWidth, tileHeight));
        }
    }
}

void ImageTextureGL::setSelectionArea(const SelectionEvent*)
{
}

QRectF ImageTextureGL::boundingRect() const
{
    return m_bounds;
}

void ImageTextureGL::setIntensity(const qreal intensity)
{
    if (intensity >= 0.0 && intensity < 1.0) {
        if (m_intensity != intensity) {
            m_intensity = intensity;
            emit updated();
        }
    }
}

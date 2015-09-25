/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef IMAGETEXTUREGL_H
#define IMAGETEXTUREGL_H

#include "GraphicItemGL.h"
#include <QVector2D>
#include <QFuture>
#include <functional>

class Renderer;
class QImage;
class QOpenGLTexture;
class QByteArray;

// This class represents a tiled image to be rendered using textures. This class
// is used to render the cell tissue image which has a high resolution
// The tiling and creation of the textures is performed concurrently
// TODO: Clarify how an empty object behaves.
class ImageTextureGL : public GraphicItemGL
{
    Q_OBJECT

public:
    explicit ImageTextureGL(QObject* parent = 0);
    virtual ~ImageTextureGL();

    // The object will display this image and update its bounding rect to the image's size.
    void setImage(Renderer& renderer, const QImage& image);

    // Resets the object to the state where it has no image. It will render nothing and have
    // an empty bounding rect.
    void resetToNoImage(Renderer& renderer);

    // Returns the size of the set image, if any, else the empty rect.
    QRectF boundingRect() const override;

public slots:

    // Set a multiplier to apply to the image color. Valid ranges are 0 (darkest) to 1 (brightest)
    // inclusive. Ranges outside of this will be ignored.
    void setIntensity(const qreal intensity);

protected:
    void setSelectionArea(const SelectionEvent*) override;

private:
    static const int TextureTileWidth = 512;
    static const int TextureTileHeight = 512;

    void doDraw(Renderer& renderer) override;

    void storeImageTiles(Renderer& renderer, const QImage& image);

    void drawImageTiles();

    // The number and dimensions of each tile are entirely decided by the size of the image to be
    // partitioned and by the size of the tile. This algorithm will call the function tileFn once
    // for every tile that an image of imageSize would need, and tileFn will be passed the
    // tile rectangle: its location and size on the image.
    static void forEachTile(const QSize& imageSize, std::function<void(QRect)> tileFn);

    // The screen rectangle occupied by the image.
    QRect m_bounds;

    // How bright the image is, 0 being darkest, 1 being brightest.
    qreal m_intensity;

    // Intended as a debug feature, when true the textures will be rendered with a yellow border.
    bool m_outlineTextureTiles;

    Q_DISABLE_COPY(ImageTextureGL)
};

#endif // IMAGETEXTUREGL_H

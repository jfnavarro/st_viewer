#ifndef IMAGETEXTUREGL_H
#define IMAGETEXTUREGL_H

#include "GraphicItemGL.h"
#include <QVector2D>
#include <QFuture>

class QImage;
class QOpenGLTexture;
class QByteArray;

// This class represents a tiled image to be rendered using textures. This class
// is used to render the cell tissue image which has a high resolution
// The tiling and creation of the textures is performed concurrently
class ImageTextureGL : public GraphicItemGL
{
    Q_OBJECT

public:
    explicit ImageTextureGL(QObject *parent = 0);
    virtual ~ImageTextureGL();

    // will split the image into small textures of fixed size in an asynchronous
    // way
    // using createTiles and returning the future object
    QFuture<void> createTexture(const QByteArray &imageByteArray);

    // will remove and destroy all textures
    void clearData();

    // return the total size of the image as a QRectF
    const QRectF boundingRect() const override;

    // will split the images into small textures of fixed size
    void createTiles(QByteArray imageByteArray);

public slots:

    // to adjust intensity of the textures
    void setIntensity(float intensity);

protected:
    void setSelectionArea(const SelectionEvent *) override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // internal functions to create a texture from an image and add it to the
    // rendering list
    void addTexture(const QImage &image, const unsigned x = 0, const unsigned y = 0);

    // internal function to remove and clean textures
    void clearTextures();
    void clearNodes();

    QVector<QOpenGLTexture *> m_textures;
    QVector<QVector2D> m_textures_indices;
    QVector<QVector2D> m_texture_coords;
    float m_intensity;
    QRectF m_bounds;
    bool m_isInitialized;

    Q_DISABLE_COPY(ImageTextureGL)
};

#endif // IMAGETEXTUREGL_H

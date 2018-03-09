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

    // this function will split the image into small textures of fixed size in an asynchronous way
    // using createTiles and returning the future object
    QFuture<void> createTextures(const QString &imagefile);

    // will remove and destroy all textures
    void clearData();

    // return the total size of the image as a QRectF
    const QRectF boundingRect() const override;

    // will split the image given as input into small textures of fixed size
    // returns true if the parsing and creation of tiles was correct
    bool createTiles(const QString &imagefile);

    // return a grid of points computed from the image (inside the tissue)
    const QList<QPointF>& getGrid() const;

    // true if the image has been scaled down
    bool scaled() const;

public slots:

protected:

    void draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter) override;
    void setSelectionArea(const SelectionEvent &event);

private:

    // internal function to create a grid of of the image (inside tissue)
    void createGrid(const QImage &image, const int offset);

    // internal functions to create a texture from an image and add it to the
    // rendering list
    void addTexture(const QImage &image, const int x = 0, const int y = 0);

    // internal function to remove and clean textures
    void clearTextures();
    void clearNodes();

    QVector<QOpenGLTexture *> m_textures;
    QVector<QVector2D> m_textures_indices;
    QVector<QVector2D> m_texture_coords;
    QRectF m_bounds;
    bool m_isInitialized;
    QList<QPointF> m_grid_points;
    bool m_iscaled;

    Q_DISABLE_COPY(ImageTextureGL)
};

#endif // IMAGETEXTUREGL_H

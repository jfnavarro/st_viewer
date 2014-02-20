#ifndef IMAGETEXTUREGL_H
#define IMAGETEXTUREGL_H

#include "GraphicItemGL.h"

class QGLPainter;
class QImage;
class QGLTexture2D;

class ImageTextureGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit ImageTextureGL(QObject *parent = 0);
    virtual ~ImageTextureGL();

    void createTexture(const QImage& image);
    void clearTextures();

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    const QRectF boundingRect() const;

    void createTiles(const QImage &image);
    void addTexture(const QImage &image, const int x = 0, const int y = 0);

private:

     static const QImage createSubImage(const QImage &image, const QRect & rect);

     QList<QGLTexture2D *> m_textures;
};

#endif // IMAGETEXTUREGL_H

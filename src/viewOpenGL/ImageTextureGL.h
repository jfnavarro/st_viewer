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

public slots:

    void setIntensity(qreal intensity);

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    const QRectF boundingRect() const;

    void createTiles(const QImage &image);
    void addTexture(const QImage &image, const int x = 0, const int y = 0);

private:

     QList<QGLTexture2D *> m_textures;

     qreal m_intensity = 1.0f;
};

#endif // IMAGETEXTUREGL_H

#ifndef IMAGETEXTUREGL_H
#define IMAGETEXTUREGL_H

#include <QGLSceneNode>
#include <QTransform>

class QGLPainter;
class QImage;
class QGLTexture2D;

class ImageTextureGL : public QGLSceneNode
{

    Q_OBJECT

public:

    explicit ImageTextureGL(QObject *parent = 0);
    virtual ~ImageTextureGL();

    void createTexture(const QImage& image);
    void createTiles(const QImage &image);
    void addTexture(const QImage &image, const int x = 0, const int y = 0);
    void clearTextures();

    static const QImage createSubImage(const QImage &image, const QRect & rect);

    bool visible() const;

public slots:

    void setVisible(bool);

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

private:

     QList<QGLTexture2D *> m_textures;

     bool m_visible;
};

#endif // IMAGETEXTUREGL_H

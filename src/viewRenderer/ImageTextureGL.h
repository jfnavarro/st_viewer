#ifndef IMAGETEXTUREGL_H
#define IMAGETEXTUREGL_H

#include <QVector2D>
#include <QOpenGLFunctions>
#include <QRectF>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

class QImage;

// This class represents a tiled image to be rendered using textures. This class
// is used to render the cell tissue image which has a high resolution
class ImageTextureGL : public QOpenGLFunctions
{

public:
    ImageTextureGL();
    ~ImageTextureGL();

    // initialize the OpenGL context and the shaders
    void init();

    // will remove and destroy all textures
    void clearData();

    // return the total size of the image as a QRectF
    QRectF boundingRect() const;

    // will split the image given as input into small textures of fixed size
    // returns true if the parsing and creation of tiles was correct
    bool createTiles(const QString &imagefile);

    // true if the image has been scaled down
    bool scaled() const;

    // draw the image
    void draw(const QMatrix4x4 &mvp_matrx);

private:

    // internal functions to create a texture from an image and add it to the
    // rendering list
    void addTexture(const QImage &image, const int x = 0, const int y = 0);

    // internal function to remove and clean textures
    void clearTextures();
    void clearNodes();

    // OpenGL rendering data and buffers
    QVector<QOpenGLTexture *> m_textures;
    QVector<QVector3D> m_textures_pos;
    QVector<QVector2D> m_texture_coords;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_posBuf;
    QOpenGLBuffer m_coordBuf;
    QOpenGLShaderProgram *m_program;

    QRectF m_bounds;
    bool m_isInitialized;
    bool m_iscaled;

    Q_DISABLE_COPY(ImageTextureGL)
};

#endif // IMAGETEXTUREGL_H

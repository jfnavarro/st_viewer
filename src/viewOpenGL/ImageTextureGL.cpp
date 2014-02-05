
#include "ImageTextureGL.h"

#include <QGLBuilder>
#include <QGeometryData>
#include <QGLMaterial>
#include <QImage>
#include <QGLPainter>

#include <cmath>

static GLint maxTextureSize() {
    GLint texSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
    return texSize;
}

ImageTextureGL::ImageTextureGL(QObject *parent) :
    QGLSceneNode(parent)
{

}

ImageTextureGL::~ImageTextureGL()
{
    clearTextures();
}

void ImageTextureGL::clearTextures()
{
    foreach( QGLTexture2D *texture, m_textures) {
        texture->cleanupResources();
    }
    // check this, nodes are part of the tree and should be removed automatically
    // removeNodes(allChildren());
}

void ImageTextureGL::draw(QGLPainter *painter)
{
    glEnable(GL_TEXTURE_2D);
    {
        //painter->modelViewMatrix().push();
        //painter->projectionMatrix().push();

        //foreach(QGLTexture2D *texture, m_textures) {
        //    texture->bind(); //do not think I need to do this, it is slow
        //}

        //painter->modelViewMatrix().scale(0.01f);

        foreach(QGLSceneNode *node, allChildren() ) {
            node->draw(painter);
        }

        //painter->modelViewMatrix().pop();
        //painter->projectionMatrix().pop();
    }
    glDisable(GL_TEXTURE_2D);

}

void ImageTextureGL::drawGeometry(QGLPainter *painter)
{
    QGLSceneNode::drawGeometry(painter);
}

void ImageTextureGL::createTexture(const QImage& image)
{
    const int maxSize = static_cast<int>(maxTextureSize());

    if (image.width() > maxSize || image.height() > maxSize) {
        // cut image into smaller textures
        createTiles(image);
    }
    else {
        // create one big texture for the image
        addTexture(image);
    }
}

const QImage ImageTextureGL::createSubImage(const QImage &image, const QRect & rect) {
    size_t offset = rect.x() * image.depth() / 8
                    + rect.y() * image.bytesPerLine();
    return QImage(image.bits() + offset, rect.width(), rect.height(),
                  image.bytesPerLine(), image.format());
}

void ImageTextureGL::createTiles(const QImage &image)
{
    static const int tile_width = 512;
    static const int tile_height = 512;
    const int width = image.width();
    const int height = image.height();
    const int xCount = std::ceil( qreal(width) / qreal(tile_width) );
    const int yCount = std::ceil( qreal(height) / qreal(tile_height) );
    const int count = xCount * yCount;

    for (int i = 0; i < count; ++i) {

         // texture sizes
        const int x = tile_width * (i % xCount);
        const int y = tile_height * (i / xCount);
        const int texture_width = std::min(width -  x, tile_width);
        const int texture_height = std::min(height -  y, tile_height);

        // create sub image
        QImage sub_image = ImageTextureGL::createSubImage(image,
                                                          QRect(x, y, texture_width, texture_height));

        // add texture
        addTexture(sub_image, x, y);
    }
}

void ImageTextureGL::addTexture(const QImage& image, const int x, const int y)
{

    const qreal width = qreal(image.width());
    const qreal height = qreal(image.height());
    const qreal aspect_ratio = 1.0f; //height / width

    QGLBuilder builder;
    QGeometryData data;

    QVector2D a(x, y);
    QVector2D b(x + width, y);
    QVector2D c(x + width, y + height);
    QVector2D d(x, y + height);
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, aspect_ratio);
    QVector2D td(0.0f, aspect_ratio);

    data.appendVertex(a, b, c, d);
    data.appendTexCoord(ta, tb, tc, td);

    builder.addQuads(data);
    QGLSceneNode *node  = builder.finalizedSceneNode();

    QGLTexture2D *m_texture = new QGLTexture2D(this);
    m_texture->setImage(image);
    m_texture->setVerticalWrap(QGL::ClampToEdge);
    m_texture->setHorizontalWrap(QGL::ClampToEdge);
    m_texture->setBindOptions(QGLTexture2D::DefaultBindOption);
    m_texture->setSize(QSize(width, height));

    QGLMaterial *mat = new QGLMaterial(this);
    mat->setColor(Qt::black);
    mat->setTexture(m_texture);
    if (node->palette() == 0) {
        node->setMaterial(mat);
    }
    else {
        int canMat = node->palette()->addMaterial(mat);
        node->setMaterialIndex(canMat);
    }
    node->setEffect(QGL::LitDecalTexture2D);
    addNode(node);
}

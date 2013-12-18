/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ImageItemGL.h"

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QStyleOption>
#include <QImageReader>
#include <QImage>

#include "GLQt.h"
#include "GLScope.h"
#include "math/GLMatrix.h"
#include "data/GLTextureRender.h"
#include "data/GLTextureCutter.h"

static GLint maxTextureSize() {
    GLint texSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
    return texSize;
}

ImageItemGL::ImageItemGL(QGraphicsItem* parent)
    : QGraphicsObject(parent),
      m_image(),
      m_rect(0.0f, 0.0f, 0.0f, 0.0f)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
}

ImageItemGL::ImageItemGL(const QImage& image, QGraphicsItem* parent)
    : QGraphicsObject(parent),
      m_image(),
      m_rect(0.0f, 0.0f, 0.0f, 0.0f)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
    setImage(image);
}

ImageItemGL::~ImageItemGL()
{

}

void ImageItemGL::clear()
{
    // bounds stuff
    setBoundingRect(QRectF());
    // image data
    m_texture.deallocate();
    m_image = QImage();
}

void ImageItemGL::reset()
{
    // clear rendering data
    clear();
}

void ImageItemGL::rebuildTextureData()
{
    // clear and generate new
    m_texture.deallocate();
    generateTextureData();
}

void ImageItemGL::generateTextureData()
{
    const int maxSize = static_cast<int>(maxTextureSize());
    if (m_image.width() > maxSize || m_image.height() > maxSize) {

        GL::GLTextureCutter cutter; // default size
        // cut image into smaller textures
        cutter.cut(m_image.width(), m_image.height(), m_image.bits(), m_texture);
    }
    else {
        // get dimensions
        const GLint width = m_image.width();
        const GLint height = m_image.height();
        const GLint x = 0;
        const GLint y = 0;

        // create texture
        QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture->setMinificationFilter(QOpenGLTexture::Linear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
        texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
        texture->setData(m_image); //this adds format and size

        m_texture.addTexture(texture);

        // need to create the 4 vertex for the texture (we draw textures as GL_QUADS
        // TODO this could be done automatically in addTexture
        m_texture.addVertex( x, y + height);
        m_texture.addVertex( x, y );
        m_texture.addVertex( x + width, y );
        m_texture.addVertex( x + width, y + height );
    }
}

void ImageItemGL::setImage(const QImage& image)
{
    if (m_image != image) {
        m_image = image;
        rebuildTextureData();
        setBoundingRect(QRectF(QPointF(0.0f, 0.0f), QSizeF(image.size())));
        update();
    }
}

QRectF ImageItemGL::boundingRect() const
{
    if (m_rect.isNull()) {
        m_rect = QRectF(m_image.rect());
    }
    return m_rect;
}

QPainterPath ImageItemGL::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool ImageItemGL::contains(const QPointF& point) const
{
    return QGraphicsItem::contains(point);
}

void ImageItemGL::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (painter->paintEngine()->type() != QPaintEngine::OpenGL &&
        painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qDebug() << "ImageItemGL: drawBackground needs a QGLWidget to be set as viewport on the graphics view";
        return;
    }

    GL::GLTextureRender renderer;
    painter->beginNativePainting();
    {
        renderer.render(m_texture);
    }
    painter->endNativePainting();
}

bool ImageItemGL::isObscuredBy(const QGraphicsItem *item) const
{
    return QGraphicsItem::isObscuredBy(item);
}

QPainterPath ImageItemGL::opaqueArea() const
{
    return QGraphicsItem::opaqueArea();
}

void ImageItemGL::visible(bool visible)
{
    setVisible(visible);
}

void ImageItemGL::setBoundingRect(const QRectF& rect)
{
    if (m_rect != rect) {
        prepareGeometryChange();
        m_rect = rect;
    }
}

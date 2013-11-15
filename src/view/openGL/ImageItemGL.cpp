/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QStyleOption>
#include <QImageReader>

#include <GLQt.h>
#include <GLScope.h>
#include <math/GLMatrix.h>
#include <data/GLTextureRender.h>
#include <data/GLTextureCutter.h>

#include "ImageItemGL.h"

ImageItemGL::ImageItemGL(QGraphicsItem* parent)
    : QGraphicsObject(parent), m_image(), m_rect(0.0f, 0.0f, 0.0f, 0.0f)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
}

ImageItemGL::ImageItemGL(const QImage& image, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_image(), m_rect(0.0f, 0.0f, 0.0f, 0.0f)
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
    GL::GLTextureCutter cutter; // default size
    // cut image into smaller textures
    const QImage image = m_image;
    cutter.cut(image.width(), image.height(), image.bits(), m_texture);
}

void ImageItemGL::setImage(const QImage& image)
{
    if (m_image != image)
    {
        m_image = image;
        rebuildTextureData();
        setBoundingRect(QRectF(QPointF(0.0f, 0.0f), QSizeF(image.size())));
        update();
    }
}

QRectF ImageItemGL::boundingRect() const
{
    if (m_rect.isNull())
    {
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
    if (painter->paintEngine()->type() != QPaintEngine::OpenGL &&
            painter->paintEngine()->type() != QPaintEngine::OpenGL2)
    {
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
    if (m_rect != rect)
    {
        prepareGeometryChange();
        m_rect = rect;
    }
}

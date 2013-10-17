/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef IMAGEITEMGL_H
#define IMAGEITEMGL_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGLWidget>
#include <QPointer>

#include <data/GLTextureData.h>

// Specialized graphical object class used to visualize the cell tissue image
// in the cell view. Contains all necessary data to configure and display the
// high resolution image.
class ImageItemGL : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    explicit ImageItemGL(QGraphicsItem* parent = 0);
    explicit ImageItemGL(const QImage& image, QGraphicsItem* parent = 0);
    virtual ~ImageItemGL();

    void clear();
    void reset();
    
    inline const QImage image() const { return m_image; }
    void setImage(const QImage& image);

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual bool contains(const QPointF& point) const;

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    virtual bool isObscuredBy(const QGraphicsItem* item) const;
    virtual QPainterPath opaqueArea() const;

    enum { Type = QGraphicsItem::UserType + 1 };
    inline virtual int type() const { return Type; }

public slots:
    
    void visible(bool);

private:
    
    void setBoundingRect(const QRectF& rect);
    mutable QRectF m_rect;

    // OpenGL image: only width, height and bits are valid functions.
    QImage m_image;
    GL::GLTextureData m_texture;

    void rebuildTextureData();
    void generateTextureData();
};

#endif // IMAGEITEMGL_H

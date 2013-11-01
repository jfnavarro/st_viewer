/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef VIEWITEMGL_H
#define VIEWITEMGL_H

#include <QObject>
#include <QTransform>
#include <QPointF>
#include <QMouseEvent>

// ViewItemGL is a simple abstract class that represents a static GUI item to
// rendered in the associated view port. Convenient methods are provided to
// anchor the item in predefined points in the view port (eg. north, center, etc.).
// Simple means for responding to mouse events is included as well. 
class ViewItemGL : public QObject
{
    Q_OBJECT
    Q_ENUMS(Anchor)
    
public:
    
    enum Anchor {Center, North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};
    
    explicit ViewItemGL(QObject* parent = 0);
    virtual ~ViewItemGL();
    virtual void render(QPainter* painter) = 0;
    virtual const QRectF boundingRect() const = 0;
    virtual const bool contains(const QPointF& point) const;
    // default "do nothing" event functions
    virtual const bool mouseMoveEvent(QMouseEvent* event);
    virtual const bool mousePressEvent(QMouseEvent* event);
    virtual const bool mouseReleaseEvent(QMouseEvent* event);

public slots:

    const Anchor anchor() const;
    void setAnchor(Anchor anchor);
    const QTransform transform() const;
    void setTransform(const QTransform& transform);
    int zValue() const;
    void setZValue(int z);
    bool visible() const;
    void setVisible(bool visible);

private:
    
    const QTransform adjustForAnchor(const QTransform& transform) const;
    
    QTransform m_transform;
    int m_z;
    bool m_visible;
    Anchor m_anchor;
};

#endif // VIEWITEMGL //

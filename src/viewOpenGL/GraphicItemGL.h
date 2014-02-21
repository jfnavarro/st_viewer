#ifndef GRAPHICITEMGL_H
#define GRAPHICITEMGL_H
/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QGLSceneNode>
#include <QTransform>

class QGLPainter;
class QRectF;
class QMouseEvent;

class GraphicItemGL : public QGLSceneNode
{

    Q_OBJECT
    Q_ENUMS(Anchor VisualOption)
    Q_FLAGS(VisualOptions)

public:

    enum Anchor
    {
        Center = 1,
        North = 2,
        NorthEast = 4,
        East = 8,
        SouthEast = 16,
        South = 32,
        SouthWest = 64,
        West = 128,
        NorthWest = 256
    };

    enum VisualOption
    {
        Visible = 1,
        Selectable = 2,
        Transformable = 4,
        Yinverted = 8,
        Xinverted = 16
    };
    Q_DECLARE_FLAGS(VisualOptions, VisualOption)

    explicit GraphicItemGL(QObject *parent = 0);
    virtual ~GraphicItemGL();

    Anchor anchor() const;
    void setAnchor(Anchor anchor);

    // transformation matrix of the element
    const QTransform transform() const;
    void setTransform(const QTransform& transform);

    bool visible() const;
    bool selectable() const;
    bool transformable() const;
    bool invertedY() const;
    bool invertedX() const;

    GraphicItemGL::VisualOptions visualOptions() const;
    void setVisualOptions(GraphicItemGL::VisualOptions visualOptions);
    void setVisualOption(GraphicItemGL::VisualOption visualOption, bool value);

    virtual void draw(QGLPainter *painter) = 0;
    virtual void drawGeometry (QGLPainter * painter) = 0;

    // geometry of the graphic element
    virtual const QRectF boundingRect() const = 0;
    virtual bool contains(const QPointF& point) const;

    // graphic elements can be sent mouse events
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

public slots:

    void setVisible(bool);

protected:

    const QTransform adjustForAnchor(const QTransform& transform) const;

    QTransform m_transform;
    Anchor m_anchor = Anchor::NorthWest;
    GraphicItemGL::VisualOptions m_visualOptions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicItemGL::VisualOptions)

#endif // GRAPHICITEMGL_H

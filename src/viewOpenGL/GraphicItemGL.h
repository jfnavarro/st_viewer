#ifndef GRAPHICITEMGL_H
#define GRAPHICITEMGL_H

#include <QGLSceneNode>
#include <QTransform>

class QGLPainter;
class QRectF;

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
        NorthEast = 3,
        East = 4,
        SouthEast = 5,
        South = 6,
        SouthWest = 7,
        West = 8,
        NorthWest
    };

    enum VisualOption
    {
        Visible = 0x0001,
        Selectable = 0x0002,
        Transformable = 0x0004,
        Yinverted = 0x0008,
        Xinverted = 0x0012
    };
    Q_DECLARE_FLAGS(VisualOptions, VisualOption)

    explicit GraphicItemGL(QObject *parent = 0);
    virtual ~GraphicItemGL();

    Anchor anchor() const;
    void setAnchor(Anchor anchor);

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

    virtual const QRectF boundingRect() const = 0;
    virtual bool contains(const QPointF& point) const;

public slots:

    void setVisible(bool);

protected:

    const QTransform adjustForAnchor(const QTransform& transform) const;

    QTransform m_transform;
    Anchor m_anchor = Anchor::NorthWest;
    QSizeF m_bounds;
    GraphicItemGL::VisualOptions m_visualOptions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicItemGL::VisualOptions)

#endif // GRAPHICITEMGL_H

#ifndef GRAPHICITEMGL_H
#define GRAPHICITEMGL_H

#include <QTransform>
#include <QMatrix4x4>
#include <QOpenGLFunctions_2_0>

class QRectF;
class QMouseEvent;
class SelectionEvent;

// Base class for rendering nodes used in CellGLView
// it contains some basic functionalities and options
// For any OpenGL/QPainter based graphical object that needs
// to be rendered in the CellGLView, the object
// must has this class as a base class
class GraphicItemGL : public QObject
{

    Q_OBJECT
    Q_FLAGS(VisualOptions)

public:
    using QOpenGLFunctionsVersion = QOpenGLFunctions_2_0;

    enum Anchor {
        Center = 1,
        North = 2,
        NorthEast = 3,
        East = 4,
        SouthEast = 5,
        South = 6,
        SouthWest = 7,
        West = 8,
        NorthWest = 9,
        None = 10
    };

    enum VisualOption {
        Visible = 1,
        Selectable = 2,
        Transformable = 4,
        Yinverted = 8,
        Xinverted = 16,
        RubberBandable = 32
    };
    Q_DECLARE_FLAGS(VisualOptions, VisualOption)

    explicit GraphicItemGL(QObject *parent = 0);
    virtual ~GraphicItemGL();

    // The anchor defines where the object will be placed
    // with respect to the rendering canvas
    Anchor anchor() const;
    void setAnchor(Anchor anchor);

    // transformation matrix of the object
    // (local transformation with respect to the rendering canvas)
    const QTransform transform() const;
    void setTransform(const QTransform &transform);

    // properties stored in the flags
    bool visible() const;
    bool selectable() const;
    bool transformable() const;
    bool invertedY() const;
    bool invertedX() const;
    bool rubberBandable() const;

    // Methods to change the settings/properties
    GraphicItemGL::VisualOptions visualOptions() const;
    void setVisualOptions(GraphicItemGL::VisualOptions visualOptions);
    void setVisualOption(GraphicItemGL::VisualOption visualOption, bool value);

    // The drawing method, must be overriden in every drawing object
    virtual void draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter) = 0;

    // geometry of the graphic element
    virtual const QRectF boundingRect() const = 0;

    // must be implemented in the node to support selection events (mouse selection)
    virtual void setSelectionArea(const SelectionEvent &event) = 0;

    // bounding rect boundaries check
    bool contains(const QPointF &point) const;
    bool contains(const QRectF &point) const;

public slots:
    // TODO should prepend "slot"
    void setVisible(bool);

signals:
    // TODO should prepend "signal"
    // to notify the rendering OpenGL canvas that a render is needed
    void updated();

protected:
    // This is used to adjust the position of the element according to an anchor
    // option
    // returns the local transformation matrix adjusted for the anchor position
    const QTransform adjustForAnchor(const QTransform &transform) const;

    // local transformation matrix (for the object)
    QTransform m_transform;
    // anchor position of object with respect to the screen
    Anchor m_anchor;
    // object's rendering settings
    GraphicItemGL::VisualOptions m_visualOptions;

    Q_DISABLE_COPY(GraphicItemGL)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicItemGL::VisualOptions)

#endif // GRAPHICITEMGL_H

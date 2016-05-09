#ifndef GRAPHICITEMGL_H
#define GRAPHICITEMGL_H

#include <QTransform>
#include <QMatrix4x4>
#include <QOpenGLFunctions_2_0>

#include "SettingsVisual.h"

class QRectF;
class QMouseEvent;
class SelectionEvent;

// Base class for rendering nodes used in CellGLView
// it contains some basic functionalities and options
class GraphicItemGL : public QObject
{

    Q_OBJECT
    Q_FLAGS(VisualOptions)

public:
    using QOpenGLFunctionsVersion = QOpenGLFunctions_2_0;

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
    Visual::Anchor anchor() const;
    void setAnchor(Visual::Anchor anchor);

    // transformation matrix of the element
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

    // Drawing method, calls the virtual method. Asserts that the OpenGL
    // state is error free before and after calling the virtual method.
    void draw(QOpenGLFunctionsVersion &qpengl_functions);

    // geometry of the graphic element
    virtual const QRectF boundingRect() const = 0;

    // must be implemented in the node supports selection events
    virtual void setSelectionArea(const SelectionEvent *event) = 0;

    // bounding rect boundaries check
    bool contains(const QPointF &point) const;
    bool contains(const QRectF &point) const;

    // mouse events can be sent to the node
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    // drawing functions
    // we pass the QOpenGLFunctions_2_0 functions
    void drawBorderRect(const QRectF &rect,
                        const QColor &color,
                        QOpenGLFunctionsVersion &qopengl_functions);

    // to set/get the projection and model view matrices of the rendering object
    // these matrices will be used when rendering
    void setProjection(const QMatrix4x4 &projection);
    void setModelView(const QMatrix4x4 &modelview);
    const QMatrix4x4 getProjection() const;
    const QMatrix4x4 getModelView() const;

private:
    // Override this to draw the item using OpenGL.
    virtual void doDraw(QOpenGLFunctionsVersion &qpengl_functions) = 0;

public slots:
    // TODO should prepend "slot"
    void setVisible(bool);

signals:
    // TODO should prepend "signal"
    // tells the view the rendering is needed
    void updated();

protected:
    // This is used to adjust the position of the element according to an anchor
    // option
    // returns local transform adjusted for the anchor position
    const QTransform adjustForAnchor(const QTransform &transform) const;

    // some local variables to store rendering properties
    QTransform m_transform;
    Visual::Anchor m_anchor;
    GraphicItemGL::VisualOptions m_visualOptions;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_modelView;

    Q_DISABLE_COPY(GraphicItemGL)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicItemGL::VisualOptions)

#endif // GRAPHICITEMGL_H

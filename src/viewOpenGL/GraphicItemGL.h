/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#ifndef GRAPHICITEMGL_H
#define GRAPHICITEMGL_H

#include <QTransform>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_2_Compatibility>

#include "utils/Utils.h"

class QRectF;
class QMouseEvent;
class SelectionEvent;
class Renderer;
class ColoredLines;
class ColoredQuads;
class TexturedQuads;

// Base class for rendering nodes used in CellGLView it contains some basic functionalities and
// options. This class is (for now) also the class that determines the OpenGL version used by the
// ST Viewer application.
class GraphicItemGL : public QObject
{

    Q_OBJECT
    Q_FLAGS(VisualOptions)

public:
    static const int OPENGL_VERSION_MAJOR = 3;
    static const int OPENGL_VERSION_MINOR = 2;

    using OpenGLFunctionsVersion = QOpenGLFunctions_3_2_Compatibility;

    enum VisualOption {
        Visible = 1,
        Selectable = 2,
        Transformable = 4,
        Yinverted = 8,
        Xinverted = 16,
        RubberBandable = 32
    };
    Q_DECLARE_FLAGS(VisualOptions, VisualOption)

    explicit GraphicItemGL(QObject* parent = 0);
    virtual ~GraphicItemGL();

    Globals::Anchor anchor() const;
    void setAnchor(Globals::Anchor anchor);

    // transformation matrix of the element
    const QTransform transform() const;
    void setTransform(const QTransform& transform);

    // properties stored in the flags
    bool visible() const;
    bool selectable() const;
    bool transformable() const;
    bool invertedY() const;
    bool invertedX() const;
    bool rubberBandable() const;

    GraphicItemGL::VisualOptions visualOptions() const;
    void setVisualOptions(GraphicItemGL::VisualOptions visualOptions);
    void setVisualOption(GraphicItemGL::VisualOption visualOption, bool value);

    // Drawing method, calls the virtual method. Asserts that the OpenGL
    // state is error free before and after calling the virtual method.
    void draw(Renderer& renderer);

    // geometry of the graphic element
    virtual QRectF boundingRect() const = 0;

    // must be implemented in the node supports selection events
    virtual void setSelectionArea(const SelectionEvent* event) = 0;

    // bounding rect boundaries check
    bool contains(const QPointF& point) const;
    bool contains(const QRectF& point) const;

    // mouse events can be sent to the node
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    void setProjection(const QMatrix4x4& projection);
    void setModelView(const QMatrix4x4& modelview);

    const QMatrix4x4 getProjection() const;
    const QMatrix4x4 getModelView() const;

    void drawLines(Renderer& renderer, const ColoredLines& lines);

    void drawQuads(Renderer& renderer, const ColoredQuads& quads);

    void drawTexturedQuads(Renderer& renderer,
                           const TexturedQuads& quads,
                           const QString& textureName);

    void drawRectWithBorder(Renderer& renderer,
                            const QRectF& rect,
                            const QColor& borderColor,
                            const QColor& centreColor);

private:
    // Override this to draw the item using OpenGL.
    virtual void doDraw(Renderer& renderer) = 0;

    // Returns the model view project matrix for this item (proj x modelview).
    QMatrix4x4 getModelViewProjection() const;

public slots:

    // TODO should prepend "slot"
    void setVisible(bool);

signals:

    // TODO should prepend "signal"
    // tells the view the rendering is needed
    void updated();

protected:
    // This is used to adjust the position of the element according to an anchor option
    // returns local transform adjusted for the anchor position
    const QTransform adjustForAnchor(const QTransform& transform) const;

    // some local variables to store rendering properties
    QTransform m_transform;
    Globals::Anchor m_anchor;
    GraphicItemGL::VisualOptions m_visualOptions;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_modelView;

private:
    Q_DISABLE_COPY(GraphicItemGL)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicItemGL::VisualOptions)

#endif // GRAPHICITEMGL_H

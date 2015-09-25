/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GraphicItemGL.h"
#include "AssertOpenGL.h"

#include <QVector3D>
#include <QtOpenGL>

GraphicItemGL::GraphicItemGL(QObject* parent)
    : QObject(parent)
    , m_anchor(Globals::Anchor::NorthWest)
{
}

GraphicItemGL::~GraphicItemGL()
{
}

GraphicItemGL::VisualOptions GraphicItemGL::visualOptions() const
{
    return m_visualOptions;
}

void GraphicItemGL::setVisualOptions(GraphicItemGL::VisualOptions visualOptions)
{
    if (m_visualOptions != visualOptions) {
        m_visualOptions = visualOptions;
        emit updated();
    }
}

void GraphicItemGL::setVisualOption(GraphicItemGL::VisualOption visualOption, bool value)
{
    if (value) {
        m_visualOptions |= visualOption;
    } else {
        m_visualOptions &= ~visualOption;
    }

    emit updated();
}

bool GraphicItemGL::visible() const
{
    return m_visualOptions & VisualOption::Visible;
}

bool GraphicItemGL::selectable() const
{
    return m_visualOptions & VisualOption::Selectable;
}

bool GraphicItemGL::transformable() const
{
    return m_visualOptions & VisualOption::Transformable;
}

bool GraphicItemGL::invertedY() const
{
    return m_visualOptions & VisualOption::Yinverted;
}

bool GraphicItemGL::invertedX() const
{
    return m_visualOptions & VisualOption::Xinverted;
}

bool GraphicItemGL::rubberBandable() const
{
    return m_visualOptions & VisualOption::RubberBandable;
}

void GraphicItemGL::setVisible(bool value)
{
    setVisualOption(VisualOption::Visible, value);
}

Globals::Anchor GraphicItemGL::anchor() const
{
    return m_anchor;
}

void GraphicItemGL::setAnchor(Globals::Anchor anchor)
{
    if (m_anchor != anchor) {
        m_anchor = anchor;
        emit updated();
    }
}

const QTransform GraphicItemGL::transform() const
{
    return adjustForAnchor(m_transform);
}

void GraphicItemGL::setTransform(const QTransform& transform)
{
    m_transform = transform;
}

bool GraphicItemGL::contains(const QPointF& point) const
{
    return boundingRect().contains(point);
}

bool GraphicItemGL::contains(const QRectF& rect) const
{
    return boundingRect().contains(rect);
}

const QTransform GraphicItemGL::adjustForAnchor(const QTransform& transform) const
{
    static const int padding_x = 20.0;
    static const int padding_y = 20.0;

    const QRectF rect = boundingRect();
    QTransform adjustedTransform = transform;
    switch (m_anchor) {
    case Globals::Anchor::Center:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5,
                                    (rect.y() + rect.height()) * -0.5);
        break;
    case Globals::Anchor::North:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5, 0.0 + padding_y);
        break;
    case Globals::Anchor::NorthEast:
        adjustedTransform.translate((rect.x() + rect.width() + padding_x) * -1.0, 0.0 + padding_y);
        break;
    case Globals::Anchor::East:
        adjustedTransform.translate((rect.x() + rect.width() + padding_x) * -1.0,
                                    (rect.y() + rect.height()) * -0.5);
        break;
    case Globals::Anchor::SouthEast:
        adjustedTransform.translate((rect.x() + rect.width() + padding_x) * -1.0,
                                    (rect.y() + rect.height() + padding_y) * -1.0);
        break;
    case Globals::Anchor::South:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5,
                                    (rect.y() + rect.height() + padding_y) * -1.0);
        break;
    case Globals::Anchor::SouthWest:
        adjustedTransform.translate(0.0 + padding_x, (rect.y() + rect.height() + padding_y) * -1.0);
        break;
    case Globals::Anchor::West:
        adjustedTransform.translate(0.0 + padding_x, (rect.y() + rect.height()) * -0.5);
        break;
    case Globals::Anchor::NorthWest:
        adjustedTransform.translate(0.0 + padding_x, 0.0 + padding_y);
        break;
    case Globals::Anchor::None:
    // fall trough
    default:
        break;
    }
    return adjustedTransform;
}

void GraphicItemGL::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void GraphicItemGL::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void GraphicItemGL::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

// TODO perhaps the QOpenGLFunctions_2_0 should be a member variable
void GraphicItemGL::drawBorderRect(const QRectF& rect,
                                   QColor color,
                                   QOpenGLFunctionsVersion& qopengl_functions)
{
    const QPointF stl = rect.topLeft();
    const QPointF str = rect.topRight();
    const QPointF sbr = rect.bottomRight();
    const QPointF sbl = rect.bottomLeft();

    qopengl_functions.glBegin(GL_QUADS);
    {
        qopengl_functions.glColor4f(static_cast<GLfloat>(color.redF()),
                                    static_cast<GLfloat>(color.greenF()),
                                    static_cast<GLfloat>(color.blueF()),
                                    0.2f);
        qopengl_functions.glVertex2f(stl.x(), stl.y());
        qopengl_functions.glVertex2f(str.x(), str.y());
        qopengl_functions.glVertex2f(sbr.x(), sbr.y());
        qopengl_functions.glVertex2f(sbl.x(), sbl.y());
    }
    qopengl_functions.glEnd();

    qopengl_functions.glBegin(GL_LINES);
    {
        qopengl_functions.glColor4f(static_cast<GLfloat>(color.redF()),
                                    static_cast<GLfloat>(color.greenF()),
                                    static_cast<GLfloat>(color.blueF()),
                                    0.8f);
        qopengl_functions.glVertex2f(stl.x(), stl.y());
        qopengl_functions.glVertex2f(str.x(), str.y());
        qopengl_functions.glVertex2f(str.x(), str.y());
        qopengl_functions.glVertex2f(sbr.x(), sbr.y());
        qopengl_functions.glVertex2f(sbr.x(), sbr.y());
        qopengl_functions.glVertex2f(sbl.x(), sbl.y());
        qopengl_functions.glVertex2f(sbl.x(), sbl.y());
        qopengl_functions.glVertex2f(stl.x(), stl.y());
    }
    qopengl_functions.glEnd();

    // set the color back to white to not over-draw the textures
    qopengl_functions.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void GraphicItemGL::setProjection(const QMatrix4x4& projection)
{
    m_projection = projection;
}

void GraphicItemGL::setModelView(const QMatrix4x4& modelview)
{
    m_modelView = modelview;
}

const QMatrix4x4 GraphicItemGL::getProjection() const
{
    return m_projection;
}

const QMatrix4x4 GraphicItemGL::getModelView() const
{
    return m_modelView;
}

void GraphicItemGL::draw(QOpenGLFunctionsVersion& qpengl_functions)
{
    ASSERT_OPENGL_OK;
    // Call overridden method.
    doDraw(qpengl_functions);
    ASSERT_OPENGL_OK;
}
#include "GraphicItemGL.h"

#include <QVector3D>
#include <QtOpenGL>

GraphicItemGL::GraphicItemGL(QObject *parent)
    : QObject(parent)
    , m_anchor(NorthWest)
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

GraphicItemGL::Anchor GraphicItemGL::anchor() const
{
    return m_anchor;
}

void GraphicItemGL::setAnchor(Anchor anchor)
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

void GraphicItemGL::setTransform(const QTransform &transform)
{
    m_transform = transform;
}

bool GraphicItemGL::contains(const QPointF &point) const
{
    return boundingRect().contains(point);
}

bool GraphicItemGL::contains(const QRectF &rect) const
{
    return boundingRect().contains(rect);
}

const QTransform GraphicItemGL::adjustForAnchor(const QTransform &transform) const
{
    static const int padding_x = 20.0;
    static const int padding_y = 20.0;

    const QRectF rect = boundingRect();
    QTransform adjustedTransform = transform;
    switch (m_anchor) {
    case GraphicItemGL::Anchor::Center:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5,
                                    (rect.y() + rect.height()) * -0.5);
        break;
    case GraphicItemGL::Anchor::North:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5, 0.0 + padding_y);
        break;
    case GraphicItemGL::Anchor::NorthEast:
        adjustedTransform.translate((rect.x() + rect.width() + padding_x) * -1.0, 0.0 + padding_y);
        break;
    case GraphicItemGL::Anchor::East:
        adjustedTransform.translate((rect.x() + rect.width() + padding_x) * -1.0,
                                    (rect.y() + rect.height()) * -0.5);
        break;
    case GraphicItemGL::Anchor::SouthEast:
        adjustedTransform.translate((rect.x() + rect.width() + padding_x) * -1.0,
                                    (rect.y() + rect.height() + padding_y) * -1.0);
        break;
    case GraphicItemGL::Anchor::South:
        adjustedTransform.translate((rect.x() + rect.width()) * -0.5,
                                    (rect.y() + rect.height() + padding_y) * -1.0);
        break;
    case GraphicItemGL::Anchor::SouthWest:
        adjustedTransform.translate(0.0 + padding_x, (rect.y() + rect.height() + padding_y) * -1.0);
        break;
    case GraphicItemGL::Anchor::West:
        adjustedTransform.translate(0.0 + padding_x, (rect.y() + rect.height()) * -0.5);
        break;
    case GraphicItemGL::Anchor::NorthWest:
        adjustedTransform.translate(0.0 + padding_x, 0.0 + padding_y);
        break;
    case GraphicItemGL::Anchor::None:
    // fall trough
    default:
        break;
    }
    return adjustedTransform;
}

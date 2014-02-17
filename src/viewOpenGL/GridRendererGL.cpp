#include "GridRendererGL.h"

#include "math/Common.h"
#include "utils/Utils.h"

#include <QGLPainter>
#include <QVector2DArray>

static const qreal GRID_LINE_SIZE = 1.0f;
static const qreal GRID_LINE_SPACE = 5.0f;
static const QColor DEFAULT_COLOR_GRID_BORDER = Qt::darkRed;

GridRendererGL::GridRendererGL(QObject *parent)
    :QGLSceneNode(parent),
      m_visible(false)
{

}

GridRendererGL::~GridRendererGL()
{

}

void GridRendererGL::draw(QGLPainter *painter)
{
    if (!m_visible) {
        return;
    }

    glEnable(GL_LINE_SMOOTH);
    {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glLineWidth(GRID_LINE_SIZE);

        painter->modelViewMatrix().push();
        painter->modelViewMatrix().setToIdentity();
        painter->modelViewMatrix() *= m_transform;

        m_gridBorderColor.setAlphaF(0.5);
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(m_gridBorderColor);
        painter->setVertexAttribute(QGL::Position, m_border_vertex);
        painter->draw(QGL::Lines, m_border_vertex.size());

        m_gridColor.setAlphaF(0.5);
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(m_gridColor);
        painter->setVertexAttribute(QGL::Position, m_grid_vertex);
        painter->draw(QGL::Lines, m_grid_vertex.size());

        painter->modelViewMatrix().pop();
    }
    glDisable(GL_LINE_SMOOTH);
}

void GridRendererGL::drawGeometry(QGLPainter *painter)
{
    QGLSceneNode::drawGeometry(painter);
}

void GridRendererGL::clearData()
{
    // chip grid stuff
    m_border = QRectF();
    m_rect = QRectF();
    m_gridColor = Globals::DEFAULT_COLOR_GRID;
    m_gridBorderColor = Globals::DEFAULT_COLOR_GRID_BORDER;
}

void GridRendererGL::generateData()
{
    m_grid_vertex.clear();
    m_border_vertex.clear();

    // generate borders
    for (qreal y = m_border.top(); y <= m_border.bottom(); y += 1.0) {
        if (m_rect.top() <= y && y <= m_rect.bottom()) {
            m_border_vertex.append(m_border.left(), y);
            m_border_vertex.append(m_rect.left(), y);
            m_border_vertex.append(m_rect.right(), y);
            m_border_vertex.append(m_border.right(), y);
        } else {
            m_border_vertex.append(m_border.left(), y);
            m_border_vertex.append(m_border.right(), y);
        }
    }
    for (qreal x = m_border.left(); x <= m_border.right(); x += 1.0) {
        if (m_rect.left() <= x && x <= m_rect.right()) {
            m_border_vertex.append(x, m_border.top());
            m_border_vertex.append(x, m_rect.top());
            m_border_vertex.append(x, m_rect.bottom());
            m_border_vertex.append(x, m_border.bottom());
        } else {
            m_border_vertex.append(x, m_border.top());
            m_border_vertex.append(x, m_border.bottom());
        }
    }

    // generate grid
    for (qreal y = m_rect.top(); y <= m_rect.bottom(); y += GRID_LINE_SIZE) {
        m_grid_vertex.append(m_rect.left(),  y);
        m_grid_vertex.append(m_rect.left(),  y);
    }
    for (qreal x = m_rect.left(); x <= m_rect.right(); x += GRID_LINE_SIZE) {
        m_grid_vertex.append(x, m_rect.top());
        m_grid_vertex.append(x, m_rect.bottom());
    }

    // check boundaries
    if (!qFuzzyCompare(STMath::qMod(m_rect.bottom() - m_rect.top(), GRID_LINE_SIZE), 0.0)) {
        m_grid_vertex.append(m_rect.left(), m_rect.bottom());
        m_grid_vertex.append(m_rect.right(), m_rect.bottom());
    }
    if (!qFuzzyCompare(STMath::qMod(m_rect.right() - m_rect.left(), GRID_LINE_SIZE), 0.0)) {
        m_grid_vertex.append(m_rect.right(), m_rect.top());
        m_grid_vertex.append(m_rect.right(), m_rect.bottom());
    }
}

void GridRendererGL::setDimensions(const QRectF &border, const QRectF &rect)
{
    m_border = border;
    m_rect = rect;
    emit updated();
}

const QRectF& GridRendererGL::border() const
{
    return m_border;
}

const QRectF& GridRendererGL::rectangle() const
{
    return m_rect;
}

void GridRendererGL::setColor(const QColor &color)
{
    m_gridColor = color;
    emit updated();
}

const QColor& GridRendererGL::color() const
{
    return m_gridColor;
}

void GridRendererGL::setAlignmentMatrix(const QTransform &transform)
{
    m_transform = transform;
    emit updated();
}

const QTransform& GridRendererGL::alignmentMatrix() const
{
    return m_transform;
}

void GridRendererGL::setVisible(bool visible)
{
    m_visible = visible;
    emit updated();
}

bool GridRendererGL::visible() const
{
    return m_visible;
}

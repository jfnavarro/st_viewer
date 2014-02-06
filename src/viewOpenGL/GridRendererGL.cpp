#include "GridRendererGL.h"

#include "utils/Utils.h"
#include "utils/MathExtended.h"

#include <QGLPainter>
#include <QVector2DArray>

GridRendererGL::GridRendererGL(QObject *parent)
    :QGLSceneNode(parent)
{

}

GridRendererGL::~GridRendererGL()
{

}

void GridRendererGL::draw(QGLPainter *painter)
{
    glEnable(GL_LINE_SMOOTH);
    {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glLineWidth(Globals::grid_line_size);

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
    m_gridColor = Globals::color_grid;
    m_gridBorderColor = Globals::color_grid_border;
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
    for (qreal y = m_rect.top(); y <= m_rect.bottom(); y += Globals::grid_line_size) {
        m_grid_vertex.append(m_rect.left(),  y);
        m_grid_vertex.append(m_rect.left(),  y);
    }
    for (qreal x = m_rect.left(); x <= m_rect.right(); x += Globals::grid_line_size) {
        m_grid_vertex.append(x, m_rect.top());
        m_grid_vertex.append(x, m_rect.bottom());
    }

    // check boundaries
    if (!qFuzzyCompare(QtExt::qMod(m_rect.bottom() - m_rect.top(), Globals::grid_line_size), 0.0)) {
        m_grid_vertex.append(m_rect.left(), m_rect.bottom());
        m_grid_vertex.append(m_rect.right(), m_rect.bottom());
    }
    if (!qFuzzyCompare(QtExt::qMod(m_rect.right() - m_rect.left(), Globals::grid_line_size), 0.0)) {
        m_grid_vertex.append(m_rect.right(), m_rect.top());
        m_grid_vertex.append(m_rect.right(), m_rect.bottom());
    }
}

void GridRendererGL::setDimensions(const QRectF &border, const QRectF &rect)
{
    m_border = border;
    m_rect = rect;
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
}

const QColor& GridRendererGL::color() const
{
    return m_gridColor;
}

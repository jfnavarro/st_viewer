#include "GridRendererGL.h"

#include "utils/Utils.h"
#include "utils/MathExtended.h"

#include "qgl.h"
#include "GLCommon.h"
#include "data/GLElementLineFactory.h"

GridRendererGL::GridRendererGL()
{
}

void GridRendererGL::clearData()
{
    // grid data
    m_gridData.clear();
    m_gridData.setMode(GL_LINES);
    m_queue.clear();

    // chip grid stuff
    m_border = QRectF();
    m_rect = QRectF();
    m_gridColor = Globals::color_grid;
    m_gridBorderColor = Globals::color_grid_border;
}

void GridRendererGL::updateData()
{
    //TODO this is expensive, do the normal update
    m_gridData.clear();
    m_gridData.setMode(GL_LINES);
    generateData();
}

void GridRendererGL::rebuildData()
{
    m_gridData.clear();
    m_gridData.setMode(GL_LINES);
    generateData();
}

void GridRendererGL::generateData()
{
    const GL::GLflag flags =
            GL::GLElementShapeFactory::AutoAddColor |
            GL::GLElementShapeFactory::AutoAddTexture;
    GL::GLElementLineFactory factory(m_gridData, flags);

    // generate borders
    const GL::GLcolor gridBorderColor = 0.5f * GL::toGLcolor(m_gridBorderColor);
    factory.setColor(gridBorderColor);

    for (qreal y = m_border.top(); y <= m_border.bottom(); y += 1.0) {
        if (m_rect.top() <= y && y <= m_rect.bottom()) {
            factory.addShape(QLineF( QPointF(m_border.left(), y), QPointF(m_rect.left(), y) ));
            factory.addShape(QLineF( QPointF(m_rect.right(), y), QPointF(m_border.right(), y) ));
        } else {
            factory.addShape(QLineF( QPointF(m_border.left(), y) , QPointF(m_border.right(), y) ));
        }
    }

    for (qreal x = m_border.left(); x <= m_border.right(); x += 1.0) {
        if (m_rect.left() <= x && x <= m_rect.right()) {
            factory.addShape(QLineF( QPointF(x, m_border.top()), QPointF(x, m_rect.top()) ));
            factory.addShape(QLineF( QPointF(x, m_rect.bottom()), QPointF(x, m_border.bottom()) ));
        } else {
            factory.addShape(QLineF( QPointF(x, m_border.top()) , QPointF(x, m_border.bottom()) ));
        }
    }

    // generate grid
    const GL::GLcolor gridColor = 0.5f * GL::toGLcolor(m_gridColor);
    factory.setColor(gridColor);

    for (qreal y = m_rect.top(); y <= m_rect.bottom(); y += Globals::grid_line_size) {
        factory.addShape(QLineF( QPointF(m_rect.left(),  y) , QPointF(m_rect.left(),  y) ));
    }

    for (qreal x = m_rect.left(); x <= m_rect.right(); x += Globals::grid_line_size) {
        factory.addShape(QLineF( QPointF(x, m_rect.top()) , QPointF(x, m_rect.bottom()) ));
    }

    // check boundaries
    if (!qFuzzyCompare(QtExt::qMod(m_rect.bottom() - m_rect.top(), Globals::grid_line_size), 0.0)) {
        factory.addShape(QLineF( QPointF(m_rect.left(), m_rect.bottom()) , QPointF(m_rect.right(), m_rect.bottom()) ));
    }

    if (!qFuzzyCompare(QtExt::qMod(m_rect.right() - m_rect.left(), Globals::grid_line_size), 0.0)) {
        factory.addShape(QLineF( QPointF(m_rect.right(), m_rect.top()) , QPointF(m_rect.right(), m_rect.bottom()) ));
    }

    // generate element data render command
    m_queue.add(GL::GLElementRenderQueue::Command
                (GL::GLElementRenderQueue::Command::RenderItemAll));   // render elements
    m_queue.end();
}

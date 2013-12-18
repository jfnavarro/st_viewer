#include "GridRendererGL.h"

#include "utils/Utils.h"
#include "qgl.h"
#include "GLCommon.h"
#include "math/GLFloat.h"
#include "utils/MathExtended.h"

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
    // generate borders
    const GL::GLcolor gridBorderColor = 0.5f * GL::toGLcolor(m_gridBorderColor);
    for (qreal y = m_border.top(); y <= m_border.bottom(); y += 1.0) {

        if (m_rect.top() <= y && y <= m_rect.bottom()) {

            m_gridData.addPoint(GL::toGLpoint(m_border.left(), y));
            m_gridData.addPoint(GL::toGLpoint(m_rect.left(), y));
            m_gridData.addColor(GL::GLlinecolor(gridBorderColor));
            m_gridData.connect(GL::GLlineindex());
            m_gridData.addPoint(GL::toGLpoint(m_rect.right(), y));
            m_gridData.addPoint(GL::toGLpoint(m_border.right(), y));
            m_gridData.addColor(GL::GLlinecolor(gridBorderColor));
            m_gridData.connect(GL::GLlineindex());

        } else {

            m_gridData.addPoint(GL::toGLpoint(m_border.left(), y));
            m_gridData.addPoint(GL::toGLpoint(m_border.right(), y));
            m_gridData.addColor(GL::GLlinecolor(gridBorderColor));
            m_gridData.connect(GL::GLlineindex());

        }
    }
    for (qreal x = m_border.left(); x <= m_border.right(); x += 1.0) {

        if (m_rect.left() <= x && x <= m_rect.right()) {

            m_gridData.addPoint(GL::toGLpoint(x, m_border.top()));
            m_gridData.addPoint(GL::toGLpoint(x, m_rect.top()));
            m_gridData.addColor(GL::GLlinecolor(gridBorderColor));
            m_gridData.connect(GL::GLlineindex());
            m_gridData.addPoint(GL::toGLpoint(x, m_rect.bottom()));
            m_gridData .addPoint(GL::toGLpoint(x, m_border.bottom()));
            m_gridData.addColor(GL::GLlinecolor(gridBorderColor));
            m_gridData.connect(GL::GLlineindex());

        } else {

            m_gridData.addPoint(GL::toGLpoint(x, m_border.top()));
            m_gridData.addPoint(GL::toGLpoint(x, m_border.bottom()));
            m_gridData.addColor(GL::GLlinecolor(gridBorderColor));
            m_gridData.connect(GL::GLlineindex());

        }
    }

    // generate grid
    const GL::GLcolor gridColor = 0.5f * GL::toGLcolor(m_gridColor);
    for (qreal y = m_rect.top(); y <= m_rect.bottom(); y += Globals::grid_line_size) {

        m_gridData.addPoint(GL::toGLpoint(m_rect.left(),  y));
        m_gridData.addPoint(GL::toGLpoint(m_rect.right(), y));
        m_gridData.addColor(GL::GLlinecolor(gridColor));
        m_gridData.connect(GL::GLlineindex());

    }
    for (qreal x = m_rect.left(); x <= m_rect.right(); x += Globals::grid_line_size) {

        m_gridData.addPoint(GL::toGLpoint(x, m_rect.top()));
        m_gridData.addPoint(GL::toGLpoint(x, m_rect.bottom()));
        m_gridData.addColor(GL::GLlinecolor(gridColor));
        m_gridData.connect(GL::GLlineindex());

    }
    if (!qFuzzyCompare(QtExt::qMod(m_rect.bottom() - m_rect.top(), Globals::grid_line_size), 0.0)) {

        m_gridData.addPoint(GL::toGLpoint(m_rect.left(), m_rect.bottom()));
        m_gridData.addPoint(GL::toGLpoint(m_rect.right(), m_rect.bottom()));
        m_gridData.addColor(GL::GLlinecolor(gridColor));
        m_gridData.connect(GL::GLlineindex());

    }
    if (!qFuzzyCompare(QtExt::qMod(m_rect.right() - m_rect.left(), Globals::grid_line_size), 0.0)) {

        m_gridData.addPoint(GL::toGLpoint(m_rect.right(), m_rect.top()));
        m_gridData.addPoint(GL::toGLpoint(m_rect.right(), m_rect.bottom()));
        m_gridData.addColor(GL::GLlinecolor(gridColor));
        m_gridData.connect(GL::GLlineindex());

    }

    // generate element data render command
    m_queue.add(GL::GLElementRenderQueue::Command
                (GL::GLElementRenderQueue::Command::RenderItemAll));   // render elements
    m_queue.end();
}

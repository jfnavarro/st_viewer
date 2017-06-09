#include "RubberbandGL.h"
#include <QColor>

RubberbandGL::RubberbandGL(QObject *parent)
    : GraphicItemGL(parent)
{
    setVisualOption(GraphicItemGL::Transformable, false);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, false);
}

RubberbandGL::~RubberbandGL()
{
}

const QRectF RubberbandGL::boundingRect() const
{
    return m_rubberbandRect;
}

void RubberbandGL::setRubberbandRect(const QRectF &rect)
{
    if (m_rubberbandRect != rect) {
        m_rubberbandRect = rect;
    }
}

void RubberbandGL::draw(QOpenGLFunctionsVersion &qopengl_functions)
{
    if (!m_rubberbandRect.isNull() && m_rubberbandRect.isValid()) {
        const QColor color = QColor(Qt::blue);
        const QPointF stl = m_rubberbandRect.topLeft();
        const QPointF str = m_rubberbandRect.topRight();
        const QPointF sbr = m_rubberbandRect.bottomRight();
        const QPointF sbl = m_rubberbandRect.bottomLeft();

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
}

void RubberbandGL::setSelectionArea(const SelectionEvent &event)
{
    Q_UNUSED(event)
}

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
        drawBorderRect(m_rubberbandRect, QColor(Qt::blue), qopengl_functions);
    }
}


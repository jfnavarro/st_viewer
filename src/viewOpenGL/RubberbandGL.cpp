/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "RubberbandGL.h"

#include <QEvent>

RubberbandGL::RubberbandGL(QObject* parent)
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
    // TODO should prob return m_rubberbandRect but it is not relevant
    return QRectF();
}

void RubberbandGL::setRubberbandRect(const QRectF& rect)
{
    if (m_rubberbandRect != rect) {
        m_rubberbandRect = rect;
    }
}

void RubberbandGL::doDraw(QOpenGLFunctionsVersion& qopengl_functions)
{
    if (!m_rubberbandRect.isNull() && m_rubberbandRect.isValid()) {
        drawBorderRect(m_rubberbandRect, Qt::blue, qopengl_functions);
    }
}

void RubberbandGL::setSelectionArea(const SelectionEvent*)
{
}

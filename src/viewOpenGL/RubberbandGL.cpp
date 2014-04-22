/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "RubberbandGL.h"

#include <QVector2DArray>
#include <QGLPainter>
#include <QEvent>

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

RubberbandGL::~RubberbandGL() {
}

const QRectF RubberbandGL::boundingRect() const {
    return m_boundingRect;
}

void RubberbandGL::setRubberbandRect(const QRectF rect) {
    if ( rect.isValid() && m_rubberbandrect != rect) {
        m_rubberbandrect = rect;
    }
}

void RubberbandGL::setSelectionArea(const SelectionEvent *) {
}

void RubberbandGL::setBoundingRect(const QRectF rect) {
    if ( rect.isValid() && rect != m_boundingRect ) {
        m_boundingRect = rect;
    }
}

void RubberbandGL::draw(QGLPainter *painter)
{
    if ( !m_rubberbandrect.isNull() && m_rubberbandrect.isValid() ) {
        drawBorderRect(m_rubberbandrect, Qt::blue, painter);
    }
}

void RubberbandGL::drawGeometry(QGLPainter *painter)
{
    Q_UNUSED(painter);
}


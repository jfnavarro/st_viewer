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
    m_anchor =  Globals::Anchor::None;
}

RubberbandGL::~RubberbandGL() {
}

const QRectF RubberbandGL::boundingRect() const {
    return QRectF(QPointF(0.0,0.0),QSizeF(100000.0,100000.0));
}

void RubberbandGL::setRubberbandRect(const QRectF &rect) {
    m_rubberbandrect = rect;
}

void RubberbandGL::setSelectionArea(const SelectionEvent *) {
}

void RubberbandGL::drawBorderRect(const QRectF &rect, QColor color, QGLPainter *painter)
{
    const QPointF stl = rect.topLeft();
    const QPointF str = rect.topRight();
    const QPointF sbr = rect.bottomRight();
    const QPointF sbl = rect.bottomLeft();
    QVector2DArray vertices;
    vertices.append(stl.x(), stl.y());
    vertices.append(str.x(), str.y());
    vertices.append(sbr.x(), sbr.y());
    vertices.append(sbl.x(), sbl.y());

    color.setAlphaF(0.2);
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(color);
    painter->setVertexAttribute(QGL::Position, vertices );
    painter->draw(QGL::TriangleFan, vertices.size());

    color.setAlphaF(0.8);
    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(color);
    painter->setVertexAttribute(QGL::Position, vertices );
    painter->draw(QGL::LineLoop, vertices.size());
}

void RubberbandGL::draw(QGLPainter *painter)
{
    if (!m_rubberbandrect.isNull()) {
        drawBorderRect(m_rubberbandrect, Qt::blue, painter);
    }
}

void RubberbandGL::drawGeometry(QGLPainter *painter)
{
    Q_UNUSED(painter);
}


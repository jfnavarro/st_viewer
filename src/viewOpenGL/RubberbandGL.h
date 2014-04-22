/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef RUBBERBANDGL_H
#define RUBBERBANDGL_H

#include "GraphicItemGL.h"

class QGLPainter;
class QImage;
class QVector2DArray;
class QRectF;
class QColor;
class QEvent;
class QMouseEvent;

// RubberbandGL is an view port GUI item that visualizes a rubberband that is 
// shown while the user is selecting genes.

class RubberbandGL : public GraphicItemGL
{
    Q_OBJECT

public:

    explicit RubberbandGL(QObject* parent = 0);
    virtual ~RubberbandGL();

    const QRectF boundingRect() const;
    void setRubberbandRect(const QRectF rect);
    void setSelectionArea(const SelectionEvent *event);

    void setBoundingRect(const QRectF rect);

public slots:

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

private:

    QRectF m_rubberbandrect;
    QRectF m_boundingRect;
};

#endif // RUBBERBANDGL_H //

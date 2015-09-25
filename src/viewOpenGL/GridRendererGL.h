/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GRIDRENDERERGL_H
#define GRIDRENDERERGL_H

#include "GraphicItemGL.h"
#include "ColoredLines.h"

class QGLPainter;
class QRectF;
class QColor;
class QVector2DArray;

// This class represents a virtual chip or array corresponding
// to the chip or array where the experiment was performed
class GridRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:
    explicit GridRendererGL(QObject* parent = 0);
    virtual ~GridRendererGL();

    // If the border (or rect) is empty the border will not be drawn.
    // If the rect is empty nothing will be drawn.
    void setDimensions(const QRectF& border, const QRectF& rect);

    QColor gridColor() const;

    QRectF border() const;

    QRectF rectangle() const;

    // needs to be accessible from other classes
    static const QColor DEFAULT_COLOR_GRID;

public slots:

    void slotSetGridColor(const QColor& color);

protected:
    QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent*) override;

private:
    void doDraw(Renderer& renderer) override;

    void createGridlines();

    ColoredLines m_gridLines;

    // the internal gene (x,y) area
    QRectF m_rect;

    // area including the chip border (not total)
    QRectF m_border;

    // grid colors
    QColor m_centerColor;
    QColor m_borderColor;

    Q_DISABLE_COPY(GridRendererGL)
};

#endif // GRIDRENDERERGL_H

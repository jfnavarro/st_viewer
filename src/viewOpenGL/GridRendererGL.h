#ifndef GRIDRENDERERGL_H
#define GRIDRENDERERGL_H

#include "GraphicItemGL.h"

class QGLPainter;
class QRectF;
class QColor;
class QVector2DArray;

// This class represents a virtual chip or array corresponding
// to the chip or array where the experiment was performed (coordinates are in
// the arrray space)
class GridRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:
    explicit GridRendererGL(QObject *parent = 0);
    virtual ~GridRendererGL();

    // data generation
    void generateData();
    void clearData();

    // setters
    void setDimensions(const QRectF &border, const QRectF &rect);

    // gettters
    const QColor color() const;
    const QRectF border() const;
    const QRectF rectangle() const;

    // needs to be accesible from other classes
    static const QColor DEFAULT_COLOR_GRID;

public slots:

    // TODO slots should have the prefix "slot"

    void setColor(const QColor &color);

protected:
    const QRectF boundingRect() const override;
    void setSelectionArea(const SelectionEvent *) override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // data vertex arrays
    QVector<QVector2D> m_grid_vertex;
    QVector<QVector2D> m_border_vertex;

    // the internal gene (x,y) area
    QRectF m_rect;

    // area including the chip border (not total)
    QRectF m_border;

    // grid colors
    QColor m_gridColor;
    QColor m_gridBorderColor;

    Q_DISABLE_COPY(GridRendererGL)
};

#endif // GRIDRENDERERGL_H

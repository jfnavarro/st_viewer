#ifndef GRIDRENDERERGL_H
#define GRIDRENDERERGL_H

#include <QGLSceneNode>
#include <QTransform>

class QGLPainter;
class QRectF;
class QColor;
class QVector2DArray;

class GridRendererGL : public QGLSceneNode
{
    Q_OBJECT

public:

    explicit GridRendererGL(QObject *parent = 0);
    virtual ~GridRendererGL();

    //data generation
    void generateData();
    void clearData();

    //setters
    void setDimensions(const QRectF &border, const QRectF &rect);
    void setAlignmentMatrix(const QTransform &transform);
    void setColor(const QColor &color);

    //gettters
    const QColor& color() const;
    const QRectF& border() const;
    const QRectF& rectangle() const;
    const QTransform& alignmentMatrix() const;

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

private:

    // data vertex arrays
    QVector2DArray m_grid_vertex;
    QVector2DArray m_border_vertex;

    // the internal gene (x,y) area
    QRectF m_rect;

    // area including the chip border (not total)
    QRectF m_border;

    // grid colors
    QColor m_gridColor;
    QColor m_gridBorderColor;

    // alignment matrix transformation
    QTransform m_transform;
};

#endif // GRIDRENDERERGL_H

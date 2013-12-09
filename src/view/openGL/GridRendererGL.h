#ifndef GRIDRENDERERGL_H
#define GRIDRENDERERGL_H

#include "GLCommon.h"
#include "data/GLElementData.h"
#include "data/GLElementRender.h"

class GridRendererGL
{
public:
    GridRendererGL();

    //rendering functions
    void generateData();
    void updateData();
    void clearData();
    void rebuildData();

    //setters
    inline void setDimensions(const QRectF &border, const QRectF &rect) { m_border = border; m_rect = rect; }
    inline void setColor(const QColor &color) { m_gridColor = color; }

    //getters
    inline const GL::GLElementData& getData() const { return m_gridData; }
    inline const GL::GLElementRenderQueue& getCmds() const { return m_queue; }

    inline const QColor& color() const { return m_gridColor; }
    inline const QRectF& border() const { return m_border; }
    inline const QRectF& rectangle() const { return m_rect; }

private:
    // grid data
    GL::GLElementData m_gridData;
    GL::GLElementRenderQueue m_queue;

    // the internal gene (x,y) area
    QRectF m_rect;

    // area including the chip border (not total)
    QRectF m_border;

    // grid colors
    QColor m_gridColor;
    QColor m_gridBorderColor;
};

#endif // GRIDRENDERERGL_H

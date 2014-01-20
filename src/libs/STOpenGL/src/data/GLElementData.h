/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTDATA_H
#define GLELEMENTDATA_H

#include <QVector>

#include "GLCommon.h"

#include <QLineF>
#include <QRectF>
#include <QPointF>
#include <QColor4ub>

namespace GL
{

// GLElementData is a container class used to store and manipulate
// rendering data. It is intended to be used as the data model for
// rendering primitives stored in array structures (ie. via
// glDrawElements).

class GLElementData
{
public:

    typedef QVector<QPointF> PointsType;
    typedef QVector<QPointF> VerticesType;
    typedef QVector<QColor4ub> ColorsType;
    typedef QVector<GLindex> IndexesType;
    typedef QVector<QPointF> TexturesType;

    static const PointsType::size_type rect_size = 4;
    static const IndexesType::size_type line_size = 2;

    enum ClearFlags {
        PointArray = 0x001u,
        ColorArray = 0x002u,
        IndexArray = 0x004u,
        TextureArray = 0x008u,
        RenderMode = 0x100u,
        // composite flags
        Arrays = (PointArray | ColorArray | IndexArray | TextureArray),
        Modes = (RenderMode),
        All = (Arrays | Modes)
    };

    //empty constructor
    GLElementData();
    virtual ~GLElementData();

    //clear arrays
    void clear(GLflag flags = GLElementData::All);

    // data builders

    //points
    void addPoint(const qreal &x, const qreal &y);
    void addPoint(const QPointF &point);

    //shape
    GLindex addShape(const QLineF &line);
    GLindex addShape(const QRectF &rect);

    //colors (assume they extend to all vertex)
    void addColor(const QColor4ub &color);
    void addColorLine(const QColor4ub &color);
    void addColorRect(const QColor4ub &color);

    //textures
    void addTexture(const QPointF &point);
    void addTexture(const QLineF &line);
    void addTexture(const QRectF &rectangle);

    //connect indexes to data
    void connectLine(GLindex *index = 0);
    void connectRect(GLindex *index = 0);
    void connectLine(const GLindex &indexIn, GLindex *indexOut = 0);
    void connectRect(const GLindex &indexIn, GLindex *indexOut = 0);
    void deconnectLine(const GLindex &index);
    void deconnectRect(const GLindex &index);

    // data modifiers
    void setShape(const GLindex &index, const QLineF &line);
    void setShape(const GLindex &index, const QRectF &rectangle);

    void setColorLine(const GLindex &index, const QColor4ub &color);
    void setColorRect(const GLindex &index, const QColor4ub &color);

    void setTexture(const GLindex &index, const QLineF &line);
    void setTexture(const GLindex &index, const QRectF &rectangle);

    // data access
    const QColor4ub getColor(const GLindex &index);

    // state modifiers
    void setMode(const GLenum &mode);

    //array getters
    const VerticesType &vertices() const;
    const ColorsType &colors() const;
    const IndexesType &indices() const;
    const TexturesType &textures() const;

    //enum mode getter
    const GLenum& mode() const;

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty()
                && m_textures.empty() && m_points.empty();
    }

protected:

    //data members
    PointsType m_points;
    ColorsType m_colors;
    IndexesType m_indices;
    TexturesType m_textures;
    GLenum m_mode;
};

} // namespace GL //

#endif // GLELEMENTDATA_H //

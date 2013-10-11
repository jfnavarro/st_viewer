/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <math/GLVector.h>

#include "GLImageWriter.h"

namespace GL
{

const bool GLimagewriter::writePixel(const GLcolor &color)
{
    // early out
    if (!(m_index < m_indexEnd))
    {
        return false;
    }

    GLcolor *pixel = static_cast<GLcolor *>(m_image.pixels());
    pixel[m_index++] = color;
    return true;
}
const bool GLimagewriter::writeLine(const GLcolor &color)
{
    bool ret = true;
    const GLsizei width = m_image.width();
    const GLsizei end = GL::nextMultiple(m_index, width);
    while (ret && m_index < end)
    {
        ret = writePixel(color);
    }

    return ret;
}
const bool GLimagewriter::writeFillColor(const GLcolor &color)
{
    bool ret = true;
    while (ret && m_index < m_indexEnd)
    {
        ret = writePixel(color);
    }

    return ret;
}

const bool GLimagewriter::writeFillGradient(const GLcolor &color0, const GLcolor &color1, const GradientAngle gradient)
{
    const GLvector norm = (gradient == HorizontalGradient) ?
        GLvector(GLfloat(m_image.width()), GLfloat(0.0)) : 
        GLvector(GLfloat(0.0), GLfloat(m_image.height()));

    bool ret = true;

    const GLsizei width = m_image.width();
    while (ret && m_index < m_indexEnd)
    {
        const GLfloat x = GLfloat(m_index % width);
        const GLfloat y = GLfloat(m_index / width);
        const GLvector v = GLvector(x, y);

        // use orthogonal projection to determin lerp value
        const GLfloat t = v.dot(norm) / (norm.length2());

        ret = writePixel(GL::lerp(t, color0, color1));
    }

    return ret;
}

} // namespace GL //

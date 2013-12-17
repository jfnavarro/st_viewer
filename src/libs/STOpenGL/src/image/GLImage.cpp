/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLImage.h"

namespace GL
{

GLimage::GLimage()
    : m_width(0), m_height(0), m_pixels(0), m_mode(GL_RGBA), m_type(GL_FLOAT)
{

}

GLimage::GLimage(GLsizei width, GLsizei height, GLenum mode, GLenum type)
    : m_width(width), m_height(height), m_pixels(0), m_mode(mode), m_type(type)
{

}

GLimage::~GLimage()
{

}

void GLimage::reset(GLsizei width, GLsizei height, GLenum mode, GLenum type)
{
    // reset memory
    deleteImage();
    // assign new state
    m_width = width;
    m_height = height;
    m_mode = mode;
    m_type = type;
    // allocate memory
    createImage();
}

GLenum GLimage::mode() const
{
    return m_mode;
}

GLenum GLimage::type() const
{
    return m_type;
}

GLsizei GLimage::width() const
{
    return m_width;
}

GLsizei GLimage::height() const
{
    return m_height;
}

GLsizei GLimage::size() const
{
    return m_width * m_height;
}

//inline const GLcolor GLimage::pixel(const GLsizei x, const GLsizei y) const
//{
//    const int pixelColorCount = GLColorEnumInfo::valid_color(m_mode) ? GLColorEnumInfo::color_count(m_mode) : -1;
//    const int pixelTypeSize   = GLTypeEnumInfo::valid_type(m_type)   ? GLTypeEnumInfo::type_size(m_type)    : -1;
//    if ((pixelColorCount == 0) || (pixelTypeSize == -1))
//    {
//        return GLcolor();
//    }

//    const GLvoid *pixel = &static_cast<GLbyte *>(m_pixels)[y + x * pixelColorCount * pixelTypeSize];
//    return GLcolor::fromGLenum(pixel, m_type, m_mode);
//}

GLvoid *GLimage::pixels()
{
    return m_pixels;
}

const GLvoid *GLimage::pixels() const
{
    return m_pixels;
}

void GLimage::createImage()
{
    // early out
    if (m_width == 0 || m_height == 0) {
        return;
    }
    // delete any previously allocated memory
    if (m_pixels != 0) {
        delete[]((GLbyte *) m_pixels);
    }
    // allocate new memory
    const int pixelColorCount = GLColorEnumInfo::valid_color(m_mode) ? GLColorEnumInfo::color_count(m_mode) : -1;
    const int pixelTypeSize = GLTypeEnumInfo::valid_type(m_type) ? GLTypeEnumInfo::type_size(m_type) : -1;
    m_pixels = ((pixelColorCount == -1) || (pixelTypeSize == -1)) ? 0 :
               new GLbyte[m_width * m_height * pixelColorCount * pixelTypeSize];

}

void GLimage::deleteImage()
{
    // delete any allocated memory
    if (m_pixels != 0) {
        delete[]((GLbyte *) m_pixels);
        m_pixels = 0;
    }
}

} // namespace GL //

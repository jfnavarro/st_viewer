/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "libs/STOpenGL/src/GLCommon.h"

#include "GLTypeTraits.h"

namespace GL
{

// GLimage manages data storage for image objects as well as a target
// for image manipulating functions and classes.
class GLimage
{
public:

    inline GLimage();
    explicit inline GLimage(GLsizei width, GLsizei height, GLenum mode = GL_RGBA,
                   GLenum type = GL_FLOAT);
    inline virtual ~GLimage();

    inline void reset(GLsizei width, GLsizei height, GLenum mode = GL_RGBA,
                      GLenum type = GL_FLOAT);

    inline GLenum mode() const;
    inline GLenum type() const;
    inline GLsizei width() const;
    inline GLsizei height() const;
    inline GLsizei size() const;

    //inline const GLcolor pixel(const GLsizei x, const GLsizei y) const;
    inline GLvoid *pixels();
    inline const GLvoid *pixels() const;

    inline void createImage();
    inline void deleteImage();

private:

    GLsizei m_width, m_height;
    GLvoid *m_pixels;
    GLenum m_mode; // pixel format
    GLenum m_type; // pixel data type
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

inline GLimage::GLimage()
    : m_width(0), m_height(0), m_pixels(0), m_mode(GL_RGBA), m_type(GL_FLOAT)
{ 
    
}

inline GLimage::GLimage(GLsizei width, GLsizei height, GLenum mode, GLenum type)
    : m_width(width), m_height(height), m_pixels(0), m_mode(mode), m_type(type)
{
    
}

inline GLimage::~GLimage() 
{ 
    
}

inline void GLimage::reset(GLsizei width, GLsizei height, GLenum mode, GLenum type)
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

inline GLenum GLimage::mode() const
{
    return m_mode;
}

inline GLenum GLimage::type() const
{
    return m_type;
}

inline GLsizei GLimage::width() const
{
    return m_width;
}

inline GLsizei GLimage::height() const
{
    return m_height;
}

inline GLsizei GLimage::size() const
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

inline GLvoid *GLimage::pixels()
{
    return m_pixels;
}

inline const GLvoid *GLimage::pixels() const
{
    return m_pixels;
}

inline void GLimage::createImage()
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

inline void GLimage::deleteImage()
{
    // delete any allocated memory
    if (m_pixels != 0) {
        delete[]((GLbyte *) m_pixels);
        m_pixels = 0;
    }
}

} // namespace GL //

#endif // GLIMAGE_H //

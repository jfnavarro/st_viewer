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

    GLimage();
    explicit GLimage(GLsizei width, GLsizei height, GLenum mode = GL_RGBA,
                   GLenum type = GL_FLOAT);
    virtual ~GLimage();

    void reset(GLsizei width, GLsizei height, GLenum mode = GL_RGBA,
                      GLenum type = GL_FLOAT);

    GLenum mode() const;
    GLenum type() const;
    GLsizei width() const;
    GLsizei height() const;
    GLsizei size() const;

    //inline const GLcolor pixel(const GLsizei x, const GLsizei y) const;
    GLvoid *pixels();
    const GLvoid *pixels() const;

    void createImage();
    void deleteImage();

private:

    GLsizei m_width, m_height;
    GLvoid *m_pixels;
    GLenum m_mode; // pixel format
    GLenum m_type; // pixel data type
};

} // namespace GL //

#endif // GLIMAGE_H //

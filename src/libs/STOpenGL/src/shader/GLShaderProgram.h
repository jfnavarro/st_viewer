/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLSHADERPROGRAM_H
#define GLSHADERPROGRAM_H

//NOTE Qt does not expose opengl shader functions. Left as stub for now.
//TODO Finish implementation.

#include "GLCommon.h"
#include "shader/GLShader.h"

namespace GL
{

typedef GLuint GLshaderprogramhandle;

static const GLshaderprogramhandle INVALID_SHADER_PROGRAM_HANDLE = 0;

// GLshaderprogram represents a opengl shader programs, ie. the compiled
// collection of a vertex and fragment shader.

struct GLshaderprogram
{
    inline GLshaderprogram();
    inline explicit GLshaderprogram(GL::Initialization);
    inline explicit GLshaderprogram(GLshaderprogramhandle handle);

    inline void createHandle();
    inline void deleteHandle();

    inline void bind() const;
    static inline void unbind();

    GLshaderprogramhandle handle;
};

} // namespace GL //


namespace GL
{

inline GLshaderprogram::GLshaderprogram() : handle(GL::INVALID_SHADER_PROGRAM_HANDLE) { }
inline GLshaderprogram::GLshaderprogram(GL::Initialization) { }
inline GLshaderprogram::GLshaderprogram(GLshaderprogramhandle handle) : handle(handle) { }

inline void GLshaderprogram::createHandle()
{
    Q_ASSERT_X(handle == GL::INVALID_SHADER_PROGRAM_HANDLE, "GLshaderprogram", "GLshaderprogram already exists on GPU");
    handle = glCreateProgram();
}
inline void GLshaderprogram::deleteHandle()
{
    Q_ASSERT_X(handle != GL::INVALID_SHADER_PROGRAM_HANDLE, "GLshaderprogram", "GLshaderprogram does not exists on GPU");
    glDeleteProgram(handle);
    handle = GL::INVALID_SHADER_PROGRAM_HANDLE;
}

inline void GLshaderprogram::bind() const
{
    glUseProgram(handle);
}
inline void GLshaderprogram::unbind()
{
    glUseProgram(0);
}

} // namespace GL //

#endif // GLSHADERPROGRAM_H //

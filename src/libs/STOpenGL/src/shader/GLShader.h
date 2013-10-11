/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLSHADER_H
#define GLSHADER_H

//NOTE Qt does not expose opengl shader functions. Left as stub for now.
//TODO Finish implementation.

#include "GLCommon.h"

namespace GL
{

    enum ShaderType
    {
        EmptyShader,
        GeometryShader,
        VertexShader,
        FragmentShader
    };

    typedef GLuint GLshaderhandle;

    static const GLshaderhandle INVALID_SHADER_HANDLE = 0;

    // GLshader class represents a opengl shader. It represents the compiled
    // version of a vertex or fragment shader managing the interactions with
    // opengl.
    //TODO Stub implementation as Qt does not expose necessary opengl functions.
    struct GLshader
    {
        inline GLshader();
        inline explicit GLshader(GL::Initialization);
        inline explicit GLshader(const ShaderType type, GLshaderhandle handle = INVALID_SHADER_HANDLE);
        
        inline void createHandle();
        inline void deleteHandle();

        const bool compile(const char *source);

        inline const bool isCompiled() const;

        ShaderType type;
        GLshaderhandle handle;
    };

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

    inline GLshader::GLshader() : type(EmptyShader), handle(INVALID_SHADER_HANDLE) { }
    inline GLshader::GLshader(GL::Initialization) { }
    inline GLshader::GLshader(const ShaderType type, GLshaderhandle handle) : type(type), handle(handle) { }
    
    inline void GLshader::createHandle()
    {
        Q_ASSERT_X(handle == INVALID_SHADER_HANDLE, "GLshader", "GLshader already exists on GPU");

        GLenum shaders[] = {
            0,
            GL_GEOMETRY_SHADER,
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER,
        };
        //handle = glCreateShader(shaders[type]);
    }

    inline void GLshader::deleteHandle()
    {
        Q_ASSERT_X(handle != INVALID_SHADER_HANDLE, "GLshader", "GLshader does not exists on GPU");

        //glDeleteShader(handle);
        handle = 0;
    }

    inline const bool GLshader::isCompiled() const { return handle != GL::INVALID_SHADER_HANDLE; }

} // namespace GL //

#endif // GLSHADER_H //

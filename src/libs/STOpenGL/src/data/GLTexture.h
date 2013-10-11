/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "GLCommon.h"

#include "image/GLImage.h"

namespace GL
{

    enum Wrap
    {
        NoWrap = 0x00,
        HorisontalWrap = 0x01,
        VerticalWrap = 0x02,
        // composite flags
        FullWrap = (HorisontalWrap | VerticalWrap)
    };

    enum Interpolation
    {
        NoInterpolation = 0x00,
        MinLinear = 0x01,
        MaxLinear = 0x02,
        // composite flags
        FullLinear = (MinLinear | MaxLinear)
    };

    typedef GLuint GLtexturehandle;

    static const GLtexturehandle INVALID_TEXTURE_HANDLE = 0;

	// GLtexture represents an opengl texture and provides functionality for
	// manipulating rendering parameters.
    struct GLtexture
    {
        inline GLtexture();
        inline explicit GLtexture(GL::Initialization);
        inline explicit GLtexture(GLtexturehandle handle);

        //NOTE assumes callee takes ownership of handles
        inline static GLtexturehandle* allocateHandles(GLsizei n);
        inline static void deallocateHandles(GLsizei n, GLtexturehandle* handles);
        
        inline void createHandle();
        inline void deleteHandle();

        inline void bind() const;
        static inline void unbind();

        inline void setWrap(Wrap flags = FullWrap);
        inline void setInterpolation(Interpolation flags = FullLinear);

        inline void setPosition(GLint x, GLint y);
        inline void setSize(GLsizei width, GLsizei height);
        inline void setBounds(GLint x, GLint y, GLsizei width, GLsizei height);

        inline void loadImage(const GLimage &image);
        inline void loadImage(const GLvoid *pixels);
        inline void loadSubImage(GLsizei width, GLsizei height, const GLvoid *pixels);
        
        GLint x, y;
        GLsizei width, height;
        GLtexturehandle handle;
    };


} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

    inline GLtexture::GLtexture() : x(0), y(0), width(0), height(0), handle(GL::INVALID_TEXTURE_HANDLE) { }
    inline GLtexture::GLtexture(GL::Initialization) { }
    inline GLtexture::GLtexture(GLtexturehandle handle) : x(0), y(0), width(0), height(0), handle(handle) { }

    inline GLtexturehandle* GLtexture::allocateHandles(GLsizei n)
    {
        GLtexturehandle* handles = new GLtexturehandle[n];
        glGenTextures(n, handles);
        return handles;
    }

    inline void GLtexture::deallocateHandles(GLsizei n, GLtexturehandle* handles)
    {
        glDeleteTextures(n, handles);
    }
    
    inline void GLtexture::createHandle()
    {
        Q_ASSERT_X(handle == GL::INVALID_TEXTURE_HANDLE, "GLtexture", "GLtexture already exists on GPU");
        glGenTextures(1, &handle);
    }

    inline void GLtexture::deleteHandle()
    {
        Q_ASSERT_X(handle != GL::INVALID_TEXTURE_HANDLE, "GLtexture", "GLtexture does not exists on GPU");
        glDeleteTextures(1, &handle);
    }
    
    inline void GLtexture::bind() const
    {
        glBindTexture(GL_TEXTURE_2D, handle);
    }
    inline void GLtexture::unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    inline void GLtexture::setWrap(Wrap flags)
    {
        bind();

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        (flags & HorisontalWrap) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        (flags & VerticalWrap) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }

    inline void GLtexture::setInterpolation(Interpolation flags)
    {
        bind();
        
        // when texture area is small, bilinear filter the closest mipmap
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            (flags & MinLinear) ? GL_LINEAR : GL_NEAREST
        );
        // when texture area is large, bilinear filter the original
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            (flags & MaxLinear) ? GL_LINEAR : GL_NEAREST
        );
    }

    inline void GLtexture::setPosition(GLint x, GLint y)
    {
        GLtexture::x = x;
        GLtexture::y = y;
    }
    inline void GLtexture::setSize(GLsizei width, GLsizei height)
    {
        GLtexture::width = width;
        GLtexture::height = height;
    }
    inline void GLtexture::setBounds(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        setPosition(x, y);
        setSize(width, height);
    }

    inline void GLtexture::loadImage(const GLimage &image)
    {
        // set member variables
        x = y = 0;
        width = image.width(), height = image.height();

        // qt open gl image format settings
        const GLenum internalFormat = GL_RGBA;

        bind();

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
             GLtexture::width, GLtexture::height, 0,
             image.mode(), image.type(), image.pixels()
        );
    }

    inline void GLtexture::loadImage(const GLvoid *pixels)
    {
        // qt open gl image format settings
        const GLenum internalFormat = GL_RGBA;
        const GLenum type = GL_UNSIGNED_BYTE;

        bind();

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
             GLtexture::width, GLtexture::height, 0,
             internalFormat, type, pixels
        );
    }

    inline void GLtexture::loadSubImage(GLsizei width, GLsizei height, const GLvoid *pixels)
    {
        // qt open gl image format settings
        const GLenum internalFormat = GL_RGBA;
        const GLenum type = GL_UNSIGNED_BYTE;

        bind();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, GLtexture::x); // invert y-axis below
        glPixelStorei(GL_UNPACK_SKIP_ROWS, (height - GLtexture::y - GLtexture::height));
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
             GLtexture::width, GLtexture::height, 0,
             internalFormat, type, pixels
        );
    }
    
} // namespace GL //

#endif // GLTEXTURE_H //

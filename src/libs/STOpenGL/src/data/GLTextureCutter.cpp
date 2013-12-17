/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLTextureCutter.h"

//#include "GLScope.h"
#include <QOpenGLPixelTransferOptions>
#include <QOpenGLTexture>

namespace GL
{

GLTextureCutter::GLTextureCutter()
    : m_width(DEFAULT_WIDTH), m_height(DEFAULT_HEIGHT)
{

}

GLTextureCutter::~GLTextureCutter()
{

}

GLTextureCutter::GLTextureCutter(GLsizei width, GLsizei height)
    : m_width(width), m_height(height)
{

}

void GLTextureCutter::cut(int width, int height, GLvoid *pixels, GLTextureData& data)
{
    const GLint cutWidth = static_cast<GLint>(m_width);
    const GLint cutHeight = static_cast<GLint>(m_height);
    const GLint imgWidth = static_cast<GLint>(width);
    const GLint imgHeight = static_cast<GLint>(height);
    const GLint xCount = qCeil( (GLfloat)imgWidth / (GLfloat)cutWidth );
    const GLint yCount = qCeil( (GLfloat)imgHeight / (GLfloat)cutHeight );
    const GLint count = xCount * yCount;

    for (int i = 0; i < count; ++i) {

         // texture sizes
        const GLint x = cutWidth * (i % xCount);
        const GLint y = cutHeight * (i / xCount);
        const GLint width = qMin(imgWidth -  x, cutWidth);
        const GLint height = qMin(imgHeight -  y, cutHeight);

        // texture options
        QOpenGLPixelTransferOptions options;
        options.setAlignment( 1 );
        options.setRowLength( imgWidth );
        options.setSkipPixels( x );
        options.setSkipRows( imgHeight - y - height );

        // texture
        QOpenGLTexture *tile = new QOpenGLTexture(QOpenGLTexture::Target2D);
        tile->setMinificationFilter(QOpenGLTexture::Linear);
        tile->setMagnificationFilter(QOpenGLTexture::Linear);
        tile->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
        tile->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
        tile->setFormat(QOpenGLTexture::RGBA8_UNorm);
        tile->setSize(width, height);
        tile->allocateStorage();
        tile->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, pixels, &options);

        // add texture to rendering data
        data.addTexture(tile);

        // TOFIX this cordinates seem to be wrong
        // need to create the 4 vertex for the texture (we draw textures as GL_QUADS
        // TODO this could be done automatically in addTexture
        data.addVertex( x, y + height);
        data.addVertex( x, y );
        data.addVertex( x + width, y );
        data.addVertex( x + width, y + height );
    }
}

} // namespace GL //

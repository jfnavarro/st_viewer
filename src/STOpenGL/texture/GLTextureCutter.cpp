/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLTextureCutter.h"

#include <QOpenGLPixelTransferOptions>
#include <QOpenGLTexture>
#include <cmath>

namespace GL
{

GLTextureCutter::GLTextureCutter()
    : m_width(DEFAULT_WIDTH),
      m_height(DEFAULT_HEIGHT)
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
    const int xCount = std::ceil( static_cast<qreal>(width) / static_cast<qreal>(m_width) );
    const int yCount = std::ceil( static_cast<qreal>(height) / static_cast<qreal>(m_height) );
    const int count = xCount * yCount;
    const int alignment = 1;

    for (int i = 0; i < count; ++i) {

         // texture sizes
        const int x = m_width * (i % xCount);
        const int y = m_height * (i / xCount);
        const int texture_width = qMin(width -  x, m_width);
        const int texture_height = qMin(height -  y, m_height);
        const int offset = width - y - texture_height;

        // texture options
        QOpenGLPixelTransferOptions options;
        options.setAlignment( alignment );
        options.setRowLength( width );
        options.setSkipPixels( x );
        options.setSkipRows( offset );

        // texture
        QOpenGLTexture *tile = new QOpenGLTexture(QOpenGLTexture::Target2D);
        tile->setMinificationFilter(QOpenGLTexture::Linear);
        tile->setMagnificationFilter(QOpenGLTexture::Linear);
        tile->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
        tile->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
        tile->setFormat(QOpenGLTexture::RGBA8_UNorm);
        tile->setSize(texture_width, texture_height);
        tile->allocateStorage();
        tile->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, pixels, &options);

        // add texture to rendering data
        data.addTexture(tile);

        // TOFIX this coordinates seem to be wrong
        // TODO this could be done automatically in addTexture
        QPointF topleft = QPointF(x, y);
        QPointF bottomright = QPointF(x + width, y + height );
        QRectF textureShape(topleft, bottomright);
        data.addVertex( textureShape );
    }
}

} // namespace GL //

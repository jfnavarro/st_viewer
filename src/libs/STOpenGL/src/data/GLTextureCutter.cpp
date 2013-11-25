/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLScope.h"

#include "GLTextureCutter.h"

namespace GL
{

void GLTextureCutter::cut(GLsizei width, GLsizei height, const GLvoid* pixels, GLTextureData& data)
{
    GLsizei cutWidth = m_width;
    GLsizei cutHeight = m_height;
    GLsizei imgWidth = width;
    GLsizei imgHeight = height;
    GLsizei xCount = qCeil((GLfloat) imgWidth / (GLfloat) cutWidth);
    GLsizei yCount = qCeil((GLfloat) imgHeight / (GLfloat) cutHeight);
    GLsizei count = xCount * yCount;
    // allocate textures
    QScopedArrayPointer<GLtexturehandle> textureIds((count > 0) ? new GLtexturehandle[count] : NULL);
    glGenTextures(count, textureIds.data());
    GLscope scope(GL_TEXTURE_2D);
    for (int i = 0; i < count; ++i) {
        // just in case
        if (textureIds[i] == INVALID_TEXTURE_HANDLE) {
            continue;
        }
        GLtexture tile(textureIds[i]);
        GLint x = cutWidth * (i % xCount), y = cutHeight * (i / xCount);
        tile.setPosition(x, y);
        tile.setSize(
            qMin(imgWidth - ((GLsizei) x), cutWidth),
            qMin(imgHeight - ((GLsizei) y), cutHeight)
        );
        // set parameters
        tile.setInterpolation(FullLinear);
        tile.setWrap(NoWrap);
        // load pixel data
        tile.loadSubImage(imgWidth, imgHeight, pixels);
        // add texture to rendering data
        data.addTexture(tile);
    }
}

} // namespace GL //

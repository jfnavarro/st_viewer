/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLTextureRender.h"

#include "GLScope.h"
#include "GLCommon.h"
#include "GLTypeTraits.h"

namespace GL
{

GLTextureRender::GLTextureRender()
{
}

GLTextureRender::~GLTextureRender()
{

}

void GLTextureRender::render(const GLTextureData& renderData)
{
    // get references to member variables
    const GLarray<QOpenGLTexture*> textures = renderData.textures();
    const GLarray<GL::GLpoint> vertices = renderData.vertices();

    // texture scope
    GLscope textureScope(GL_TEXTURE_2D);

    for (GLsizei i = 0; i < textures.size; ++i) {

        QOpenGLTexture *texture = textures.data[ i ];
        texture->bind();

        GL::GLpoint v0 = vertices.data[ i ];
        GL::GLpoint v1 = vertices.data[ i + 1 ];
        GL::GLpoint v2 = vertices.data[ i + 2 ];
        GL::GLpoint v3 = vertices.data[ i + 3 ];

        glBegin(GL_QUADS);
            glTexCoord2i(0, 0);
            glVertex2i( v0.x , v0.y );
            glTexCoord2i(0, 1);
            glVertex2i( v1.x , v1.y );
            glTexCoord2i(1, 1);
            glVertex2i( v2.x , v2.y );
            glTexCoord2i(1, 0);
            glVertex2i( v3.x , v3.y );
        glEnd();
    }
}

} // namespace GL //

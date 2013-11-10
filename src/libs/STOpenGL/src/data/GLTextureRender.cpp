/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLScope.h"

#include "GLTextureRender.h"

namespace GL
{

void GLTextureRender::render(const GLTextureData& renderData)
{
    // get references to member variables
    const GLarray<GLtexture> textures = renderData.textures();

    GLscope textureScope(GL_TEXTURE_2D);

    GLint x, y;
    GLsizei w, h;
    for (GLsizei i=0; i<textures.size; ++i)
    {
        const GLtexture texture = textures.data[i];
        x = texture.x;
        y = texture.y;
        w = texture.width;
        h = texture.height;

        glBindTexture(GL_TEXTURE_2D, texture.handle);
        glBegin(GL_QUADS);
        glTexCoord2i(0,0); glVertex2i(x+0, y+h);
        glTexCoord2i(0,1); glVertex2i(x+0, y+0);
        glTexCoord2i(1,1); glVertex2i(x+w, y+0);
        glTexCoord2i(1,0); glVertex2i(x+w, y+h);
        glEnd();
    }
}

} // namespace GL //

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
    const GLTextureData::GLtextures &textures = renderData.textures();
    const GLTextureData::GLvertices &vertices = renderData.vertices();

    // texture scope
    GLscope textureScope(GL_TEXTURE_2D);

    //TODO replace glBegin... for glDrawArrays (faster)
    //TODO textures are not modified (use opengl buffers to resources time)

    for (GLTextureData::GLtextures::size_type i = 0; i < textures.size(); ++i) {

        GLTextureData::GLtextures::value_type texture = textures[ i ];
        texture->bind();

        //NOTE assuming each texture is a rectangle
        const GLTextureData::GLvertices::value_type textureShape = vertices[ i ];
        const QPointF v0(textureShape.topLeft());
        const QPointF v1(textureShape.topRight());
        const QPointF v2(textureShape.bottomLeft());
        const QPointF v3(textureShape.bottomRight());

        glBegin(GL_QUADS);
            glTexCoord2i(0, 0);
            glVertex2i( v0.x() , v0.y() );
            glTexCoord2i(0, 1);
            glVertex2i( v1.x() , v1.y() );
            glTexCoord2i(1, 1);
            glVertex2i( v2.x() , v2.y() );
            glTexCoord2i(1, 0);
            glVertex2i( v3.x() , v3.y() );
        glEnd();
    }
}

} // namespace GL //

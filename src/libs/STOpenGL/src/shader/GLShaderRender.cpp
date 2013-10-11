/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLTypeTraits.h"

#include "GLShaderRender.h"

namespace GL
{

    void GLShaderRender::render(const GLElementData& renderData)
    {
        const GLenum mode = renderData.mode();
        const GLarray<GLpoint> vertices = renderData.vertices();
        const GLarray<GLcolor> colors = renderData.colors();
        const GLarray<GLindex> indices = renderData.indices();
        const GLarray<GLpoint> textures = renderData.textures();
        const GLarray<GLoption> options = renderData.options();

        const bool hasVertex = (vertices.size != 0);
        const bool hasColors = (colors.size != 0);
        const bool hasTexture = (textures.size != 0);
        const bool hasOptions = (options.size != 0);

        if (hasVertex && (!hasColors || !hasTexture || !hasOptions))
        {
            qDebug() << QString("Unable to render element data using shader without colors||texture coordinates.");
            return;
        }

        // set vertex array
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, vertices.data);
        // set color array
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer (4, GL::GLTypeTrait<GLfloat>::type_enum, 0, colors.data);

        int textureLocation = -1, optionLocation = -1;
        if (m_program != 0)
        {
            textureLocation = m_program->attributeLocation("in_texture");
            optionLocation = m_program->attributeLocation("in_options");

            // enable attribute arrays
            m_program->setAttributeArray(textureLocation, GL::GLTypeTrait<GLfloat>::type_enum, (const void *) textures.data, 2);
            m_program->setAttributeArray(optionLocation, GL::GLTypeTrait<GLoption>::type_enum, (const void *) options.data, 1);

            m_program->enableAttributeArray(textureLocation);
            m_program->enableAttributeArray(optionLocation);
        }

        // draw call
        glDrawElements(mode, indices.size, GL::GLTypeTrait<GLindex>::type_enum, indices.data);

        if (m_program != 0)
        {
            m_program->disableAttributeArray(textureLocation);
            m_program->disableAttributeArray(optionLocation);
        }

        // unset color array
        glDisableClientState(GL_COLOR_ARRAY);
        // unset vertex array
        glDisableClientState(GL_VERTEX_ARRAY);
    }

} // namespace GL //

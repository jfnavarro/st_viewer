/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementRender.h"

#include "GLScope.h"

namespace GL
{

GLElementRender::GLElementRender()
    : m_textures(), m_shaders()
{

}

void GLElementRender::clear()
{
    // reset state variables
    m_textures.clear();
    m_shaders.clear();
}

void GLElementRender::render(const GLElementData &renderData, const GLElementRenderQueue &renderQueue)
{
    State state(renderData, renderQueue, m_textures, m_shaders);
    state.render();
}

void GLElementRender::State::render()
{
    foreach(const GLElementRenderQueue::Command item, m_renderQueue.commandQueue()) {
        cmdCall(item.op, item.arg);
    }
}

void GLElementRender::State::cmdEndOfCmd(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    Q_UNUSED(arg);
    //TODO implement cleanup?
}

void GLElementRender::State::cmdRenderItemAll(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    Q_UNUSED(arg);
    const GLsizei count = GLTypeTrait<GLsizei>::max_value;
    render(count);
}

void GLElementRender::State::cmdRenderItemOne(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    Q_UNUSED(arg);
    const GLsizei count = 1;
    render(count);
}

void GLElementRender::State::cmdRenderItemN(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    const GLsizei count = static_cast<GLsizei>(arg);
    render(count);
}

void GLElementRender::State::cmdBindTexture(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    const GLsizei index = static_cast<GLsizei>(arg);
    const GLtexture texture = m_textures[index];
    texture.bind();
}

void GLElementRender::State::cmdUnbindTexture(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    const GLsizei index = static_cast<GLsizei>(arg);
    const GLtexture texture = m_textures[index];
    texture.unbind();
}

void GLElementRender::State::cmdBindShader(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    const GLsizei index = static_cast<GLsizei>(arg);
    const GLshaderprogram shader = m_shaders[index];
    shader.bind();
}

void GLElementRender::State::cmdUnbindShader(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    const GLsizei index = static_cast<GLsizei>(arg);
    const GLshaderprogram shader = m_shaders[index];
    shader.unbind();
}

void GLElementRender::State::render(const GLsizei renderItemCount)
{
    GLenum mode = m_renderData.mode();
    const GLarray<GLpoint> vertices = m_renderData.vertices();
    const GLarray<GLcolor> colors = m_renderData.colors();
    const GLarray<GLindex> indices = m_renderData.indices();
    const GLarray<GLpoint> textures = m_renderData.textures();

    // set vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, vertices.data);

    // set color array
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL::GLTypeTrait<GLfloat>::type_enum, 0, colors.data);

    // set texture coordinate array
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, textures.data);

    // get geometry vertex count
    const GLsizei step = GLGeomEnumInfo::vertex_count(mode);

    // draw call
    Q_ASSERT(step > 0);
    {
        const GLvoid* data = &indices[m_index * step];
        const GLsizei size = GL::min((indices.size / step) - m_index, renderItemCount);
        glDrawElements(mode, size * step, GLTypeTrait<GLindex>::type_enum, data);
        m_index += size;
    }

    // unset texture coordinate array
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    // unset color array
    glDisableClientState(GL_COLOR_ARRAY);
    // unset vertex array
    glDisableClientState(GL_VERTEX_ARRAY);
}

} // namespace GL //

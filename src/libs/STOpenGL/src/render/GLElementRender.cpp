/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementRender.h"

#include "GLScope.h"
#include "GLTypeTraits.h"

namespace GL
{

GLElementRender::GLElementRender()
    : m_textures()
{

}

// Command
GLElementRenderQueue::Command::Command()
    : op(0), arg(0)
{

}

GLElementRenderQueue::Command::Command(const Operation op, const GLuint arg)
    : op(op), arg(arg)
{

}

// GLElementRenderQueue
GLElementRenderQueue::GLElementRenderQueue()
    : m_closed(false), m_queue()
{

}

GLElementRenderQueue::~GLElementRenderQueue()
{

}

GLElementRenderQueue &GLElementRenderQueue::add(const Command &cmd)
{
    // early out
    if (m_closed) {
        return (*this);
    }

    m_queue.append(cmd);
    if (cmd.op == Command::EndOfCmd) {
        m_closed = true;
    }

    return (*this);
}

void GLElementRenderQueue::end()
{
    add(Command(Command::EndOfCmd));
}

void GLElementRenderQueue::clear()
{
    m_queue.clear();
    m_closed = false;
}

const GLElementRenderQueue::CommandQueue &GLElementRenderQueue::commandQueue() const
{
    return m_queue;
}

// GLElementRender
void GLElementRender::addTexture(QOpenGLTexture *texture)
{
    m_textures.append(texture);
}

// GLElementRender::State
GLElementRender::State::State(const GLElementData &renderData,
    const GLElementRenderQueue &renderQueue,
    const QList<QOpenGLTexture *> &textures
)
    : m_renderData(renderData), m_renderQueue(renderQueue), m_textures(textures), m_index(0) //m_shaders(shaders),
{
    // create function lookup table
    m_renderFuncs[0] = &State::cmdEndOfCmd;
    m_renderFuncs[1] = &State::cmdRenderItemAll;
    m_renderFuncs[2] = &State::cmdRenderItemOne;
    m_renderFuncs[3] = &State::cmdRenderItemN;
    m_renderFuncs[4] = &State::cmdBindTexture;
    m_renderFuncs[5] = &State::cmdUnbindTexture;
}

GLElementRender::State::~State()
{

}

void GLElementRender::State::cmdCall(const GLbyte op, const GLuint arg)
{
    Q_ASSERT(op < NUM_OPERATIONS);
    // member function vodoo magic
    (this->*(m_renderFuncs[op]))(op, arg);
}

void GLElementRender::clear()
{
    // reset state variables
    m_textures.clear();
}

void GLElementRender::render(const GLElementData &renderData,
                             const GLElementRenderQueue &renderQueue)
{
    State state(renderData, renderQueue, m_textures);
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
    render(arg);
}

void GLElementRender::State::cmdBindTexture(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    QOpenGLTexture *texture = m_textures[arg];
    texture->bind();
}

void GLElementRender::State::cmdUnbindTexture(const GLbyte op, const GLuint arg)
{
    Q_UNUSED(op);
    QOpenGLTexture *texture = m_textures[(arg)];
    texture->release();
}

void GLElementRender::State::render(const GLsizei renderItemCount)
{
    GLenum mode = m_renderData.mode();
    const GLElementData::VerticesType &vertices = m_renderData.vertices();
    const GLElementData::ColorsType &colors = m_renderData.colors();
    const GLElementData::IndexesType &indices = m_renderData.indices();
    const GLElementData::TexturesType &textures = m_renderData.textures();

    // set vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, vertices.data());

    // set color array
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL::GLTypeTrait<GLfloat>::type_enum, 0, colors.data());

    // set texture coordinate array
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, textures.data());

    // get geometry vertex count
    const GLsizei step = GLGeomEnumInfo::vertex_count(mode);

    // draw call
    Q_ASSERT(step > 0);
    {
        const GLvoid* data = &indices[m_index * step];
        const GLsizei indexes_size = indices.size();
        const GLsizei size = std::min( (indexes_size / step) - m_index, renderItemCount);
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

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTRENDER_H
#define GLELEMENTRENDER_H

#include "GLCommon.h"
#include "data/GLElementData.h"
#include "data/GLTexture.h"
#include <QGLShaderProgram>

namespace GL
{

// Simple queue class extending the normal vector class with validation.
// Closed queues are marked as such.
class GLElementRenderQueue
{
public:

    // Command represents one rendering queue command instruction with and
    // integer argument.
    struct Command {
        enum Operation {
            EndOfCmd,       // marks the end of the command queue
            RenderItemAll,  // renders all remaining items
            RenderItemOne,  // renders a single item
            RenderItemN,    // renders N items (N in arg)
            BindTexture,    // binds texture (index in arg)
            UnbindTexture  // unbinds current texture
           // BindShader,     // binds shader (index in arg) //NOTE stub
           // UnbindShader,   // unbinds current shader      //NOTE stub
        };

        inline Command();
        inline explicit Command(const Operation op, const GLuint arg = 0);

        GLbyte op;
        GLuint arg;
    };

    inline GLElementRenderQueue();
    inline ~GLElementRenderQueue();

    inline GLElementRenderQueue &add(const Command &cmd);
    inline void end();
    inline void clear();

    typedef QVector<Command> CommandQueue;
    inline const CommandQueue &commandQueue() const;

private:

    bool m_closed;
    CommandQueue m_queue;
};

// GLElementRender provides an interface to render primitive geometric data
class GLElementRender
{
public:

    GLElementRender();

    //clean up
    void clear();

    // multi-step rendering
    void render(const GLElementData &renderData, const GLElementRenderQueue &renderQueue);

    // add a texture to the rendering state
    inline void addTexture(const GLtexture &texture);

private:

    // State class represents the rendering state, and is responsible for
    // interpreting the rendering queue and invoking the right functions.
    class State
    {
    public:
        
        inline State(const GLElementData &renderData,
                     const GLElementRenderQueue &renderQueue,
                     const QList<GLtexture> &textures);
                     //const QList<QGLShaderProgram*> &shaders);

        inline ~State();
        void render();

    private:
        // pointer to function type
        typedef void (State::*CmdFuncType)(const GLbyte op, const GLuint arg);

        inline void cmdCall(const GLbyte op, const GLuint arg);

        void cmdEndOfCmd(const GLbyte op, const GLuint arg);
        void cmdRenderItemAll(const GLbyte op, const GLuint arg);
        void cmdRenderItemOne(const GLbyte op, const GLuint arg);
        void cmdRenderItemN(const GLbyte op, const GLuint arg);
        void cmdBindTexture(const GLbyte op, const GLuint arg);
        void cmdUnbindTexture(const GLbyte op, const GLuint arg);
        //void cmdBindShader(const GLbyte op, const GLuint arg);
        //void cmdUnbindShader(const GLbyte op, const GLuint arg);

        // member function pointer lookup table ('cause switches are ugly)
        CmdFuncType m_renderFuncs[6]; //8

        //render function
        void render(const GLsizei renderItemCount);

        //member variables
        const GLElementData &m_renderData;
        const GLElementRenderQueue &m_renderQueue;
        const QList<GLtexture> &m_textures;
        //const QList<QGLShaderProgram*> &m_shaders;
        GLsizei m_index;
    };

    // state variables
    QList<GLtexture> m_textures;
    //QList<QGLShaderProgram*> m_shaders;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{
    
// Command
inline GLElementRenderQueue::Command::Command()
    : op(0), arg(0)
{ 
    
}

inline GLElementRenderQueue::Command::Command(const Operation op, const GLuint arg)
    : op(op), arg(arg)
{ 
    
}

// GLElementRenderQueue
inline GLElementRenderQueue::GLElementRenderQueue()
    : m_closed(false), m_queue()
{ 
    
}

inline GLElementRenderQueue::~GLElementRenderQueue() 
{ 
    
}

inline GLElementRenderQueue &GLElementRenderQueue::add(const Command &cmd)
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

inline void GLElementRenderQueue::end()
{
    add(Command(Command::EndOfCmd));
}

inline void GLElementRenderQueue::clear()
{
    m_queue.clear();
    m_closed = false;
}

inline const GLElementRenderQueue::CommandQueue &GLElementRenderQueue::commandQueue() const
{
    return m_queue;
}

// GLElementRender
void GLElementRender::addTexture(const GLtexture &texture)
{
    m_textures.append(texture);
}

// GLElementRender::State
inline GLElementRender::State::State(
    const GLElementData &renderData,
    const GLElementRenderQueue &renderQueue,
    const QList<GLtexture> &textures
    //const QList<QGLShaderProgram*> &shaders
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
    //m_renderFuncs[6] = &State::cmdBindShader;
    //m_renderFuncs[7] = &State::cmdUnbindShader;
}

inline GLElementRender::State::~State()
{

}

inline void GLElementRender::State::cmdCall(const GLbyte op, const GLuint arg)
{
    Q_ASSERT(op < 6); //8
    // member function vodoo magic
    (this->*(m_renderFuncs[op]))(op, arg);
}

} // namespace GL //

#endif // GLELEMENTRENDER_H //

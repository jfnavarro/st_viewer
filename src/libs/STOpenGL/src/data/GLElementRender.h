/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTRENDER_H
#define GLELEMENTRENDER_H

#include "GLCommon.h"
#include "data/GLElementData.h"
#include <QOpenGLTexture>

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
        };

        Command();
        explicit Command(const Operation op, const GLuint arg = 0);

        GLbyte op;
        GLuint arg;
    };

    GLElementRenderQueue();
    virtual ~GLElementRenderQueue();

    GLElementRenderQueue &add(const Command &cmd);
    void end();
    void clear();

    typedef QVector<Command> CommandQueue;
    const CommandQueue &commandQueue() const;

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
    void addTexture(QOpenGLTexture *texture);

private:

    // State class represents the rendering state, and is responsible for
    // interpreting the rendering queue and invoking the right functions.
    class State
    {
    public:
        
        explicit State(const GLElementData &renderData,
                 const GLElementRenderQueue &renderQueue,
                 const QList<QOpenGLTexture*> &textures);

        virtual ~State();
        void render();

        static const int NUM_OPERATIONS = 6;

    private:

        // pointer to function type
        typedef void (State::*CmdFuncType)(const GLbyte op, const GLuint arg);

        void cmdCall(const GLbyte op, const GLuint arg);

        void cmdEndOfCmd(const GLbyte op, const GLuint arg);
        void cmdRenderItemAll(const GLbyte op, const GLuint arg);
        void cmdRenderItemOne(const GLbyte op, const GLuint arg);
        void cmdRenderItemN(const GLbyte op, const GLuint arg);
        void cmdBindTexture(const GLbyte op, const GLuint arg);
        void cmdUnbindTexture(const GLbyte op, const GLuint arg);

        // member function pointer lookup table ('cause switches are ugly)
        CmdFuncType m_renderFuncs[NUM_OPERATIONS];

        //render function
        void render(const GLsizei renderItemCount);

        //member variables
        const GLElementData &m_renderData;
        const GLElementRenderQueue &m_renderQueue;
        const QList<QOpenGLTexture*> &m_textures;
        GLsizei m_index;
    };

    // state variables
    QList<QOpenGLTexture*> m_textures;
};

} // namespace GL //

#endif // GLELEMENTRENDER_H //

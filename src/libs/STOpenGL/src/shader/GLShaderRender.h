/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLSHADERRENDER_H
#define GLSHADERRENDER_H

#include "QGLShaderProgram"

#include "data/GLElementDataGene.h"

namespace GL
{
// Convenience class for rendering element data using a specific
// shader program (adapted to render GLELementDataGene)
class GLShaderRender
{
    
public:
    
    inline GLShaderRender();
    inline virtual ~GLShaderRender();

    inline void clear();
    inline void shader(QGLShaderProgram *program);
    void render(const GLElementDataGene& renderData);

private:

    QGLShaderProgram *m_program;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

inline GLShaderRender::GLShaderRender()
    : m_program(0)
{

}

inline GLShaderRender::~GLShaderRender()
{

}

inline void GLShaderRender::clear() 
{ 

}

inline void GLShaderRender::shader(QGLShaderProgram *program)
{
    m_program = program;
}

} // namespace GL //

#endif // GLSHADERRENDER_H //

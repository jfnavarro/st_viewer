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
    
    GLShaderRender();
    virtual ~GLShaderRender();

    void clear();
    void shader(QGLShaderProgram *program);
    void render(const GLElementDataGene& renderData);

private:

    QGLShaderProgram *m_program;
};

} // namespace GL //

#endif // GLSHADERRENDER_H //

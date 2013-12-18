/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementShapeFactory.h"

namespace GL
{

// GLElementShapeFactory
const GLfloat GLElementShapeFactory::DEFAULT_SIZE = 1.0f;

GLElementShapeFactory::GLElementShapeFactory(GLElementData &data, GLflag flags)
    : m_data(&data),
      m_flags(flags)
{
    // initialize data
    m_data->setMode(GL_LINES);
}

GLElementShapeFactory::~GLElementShapeFactory()
{

}

void GLElementShapeFactory::clear(GLflag flags)
{
    m_data->clear(flags);
}



} // namespace GL //

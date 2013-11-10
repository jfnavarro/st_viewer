/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLScope.h"

namespace GL
{

GLscope::GLscope(GLenum cap, GLflag modes) : m_cap(cap), m_modes(modes), m_enable(true)
{
    // toggle on or off
    if (m_modes & GLscope::Reverse)
    {
        glDisable(m_cap);
    }
    else
    {
        glEnable(m_cap);
    }
}

GLscope::GLscope(GLenum cap, bool enable, GLflag modes)
    : m_cap(cap), m_modes(modes), m_enable(enable)
{
    // toggle on or off
    if ((m_modes & GLscope::Reverse) && m_enable)
    {
        glDisable(m_cap);
    }
    else if (m_enable)
    {
        glEnable(m_cap);
    }
}

GLscope::~GLscope()
{
    // toggle on or off
    if ((m_modes & GLscope::Reverse) && m_enable)
    {
        glEnable(m_cap);
    }
    else if (m_enable)
    {
        glDisable(m_cap);
    }
}

} // namespace GL //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "math/GLVector.h"

#include "GLCommon.h"

namespace GL
{

    const GLrectangle GLrectangle::fromLine(const GLpoint& from, const GLpoint& to, GLfloat lineSize)
    {
        Q_ASSERT((from != to) && lineSize > 0.0f);

        const GLvector base0 = (0.5f * lineSize) * (GLvector::fromPoints(from, to).normalize());
        const GLvector base1 = GL::rotate<-90>(base0);

        GLrectangle rectangle(GL::Uninitialized);
        rectangle.p[0] = (from - base0 + base1);
        rectangle.p[1] = (to + base0 + base1);
        rectangle.p[2] = (to + base0 - base1);
        rectangle.p[3] = (from - base0 - base1);
        return rectangle;
    }

} // namespace GL //

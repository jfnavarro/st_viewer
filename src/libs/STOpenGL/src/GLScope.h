/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLSCOPE_H
#define GLSCOPE_H

#include "GLCommon.h"

namespace GL
{

	// GLscope is a convenience class that simplifies the toggling of opengl
	// attributes. GLscope can be used to enable or disable an attribute and
	// then make sure the change is reverted at the end of the scope.
    class GLscope
    {
    public:
        enum Mode
        {
            Normal = 0x00,
            Reverse = 0x01
        };

        static const GLflag DEFAULT_MODE = Normal;

        GLscope(GLenum cap, GLflag modes = DEFAULT_MODE);
        GLscope(GLenum cap, bool enable, GLflag modex = DEFAULT_MODE);
        ~GLscope();

        //NOTE should not be able to change enable value mid scope, hence read only!
        inline const bool enable() const;

    private:
        GLenum m_cap;
        GLflag m_modes;
        bool m_enable;
    };

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

    const bool GLscope::enable() const { return m_enable; }

} // namespace GL //

#endif // GLSCOPE_H //

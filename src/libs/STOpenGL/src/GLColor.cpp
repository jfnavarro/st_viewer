/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLColor.h"

namespace GL
{

GLcolor::GLcolor(GL::GlobalColor color)
{
#define __GL_RGB(r, g, b) \
    GLrgb(((0xffu) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | ((b & 0xff) << 0))
#define __GL_RGBA(r, g, b, a) \
    GLrgb(((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | ((b & 0xff) << 0))

    static const GLrgb global_colors[] = {
        __GL_RGB(0, 0, 0),       // black
        __GL_RGB(255, 255, 255), // white
        __GL_RGB(128, 128, 128), // medium gray
        __GL_RGB(192, 192, 192), // light gray
        __GL_RGB(255, 0, 0),     // red
        __GL_RGB(0, 255, 0),     // green
        __GL_RGB(0, 0, 255),     // blue
        __GL_RGB(0, 255, 255),   // cyan
        __GL_RGB(255, 0, 255),   // magenta
        __GL_RGB(255, 255, 0),   // yellow
        __GL_RGB(128, 0, 0),     // dark red
        __GL_RGB(0, 128, 0),     // dark green
        __GL_RGB(0, 0, 128),     // dark blue
        __GL_RGB(0, 128, 128),   // dark cyan
        __GL_RGB(128, 0, 128),   // dark magenta
        __GL_RGB(128, 128, 0),   // dark yellow
        __GL_RGBA(0, 0, 0, 0)    // transparent
    };
#undef __GL_RGB
#undef __GL_RGBA

    setColor(
        GL::red(global_colors[color]),
        GL::green(global_colors[color]),
        GL::blue(global_colors[color]),
        GL::alpha(global_colors[color])
    );
}

} // namespace GL //

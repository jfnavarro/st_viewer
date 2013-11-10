/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTEXTURECUTTER_H
#define GLTEXTURECUTTER_H

#include "GLCommon.h"
#include "data/GLTextureData.h"

namespace GL
{
// GLTextureCutter is used to cut a large texture into smaller components,
// each of which are laid out into a grid based fashion.
// Textures are cut into textures with a given size (default 512x512) which
// means end segments may end up smaller (but never bigger) than set
// preferred size.
// Segments laid out accordingly:
// ╔═══╤═══╤═══╤═══╤═══╤═╗
// ║ 1 │ 2 │ 3 │ 4 │ 5 │6║
// ╟───┼───┼───┼───┼───┼─╢
// ║ 7 │...│...│...│...│.║
// ╟───┼───┼───┼───┼───┼─╢
// ║...│   │   │   │   │ ║
// ╟───┼───┼───┼───┼───┼─╢
// ║   │   │   │   │   │ ║
// ╟───┼───┼───┼───┼───┼─╢
// ╚═══╧═══╧═══╧═══╧═══╧═╝
class GLTextureCutter
{
public:
    static const GLsizei DEFAULT_WIDTH = 512;
    static const GLsizei DEFAULT_HEIGHT = 512;

    inline GLTextureCutter();
    inline GLTextureCutter(GLsizei width, GLsizei height);

    void cut(GLsizei width, GLsizei height, const GLvoid* pixels, GLTextureData& data);

private:
    GLsizei m_width;
    GLsizei m_height;
};

} // namespace GL //

/***************************************************** DEFINITION ****************************************************************************/

namespace GL
{

GLTextureCutter::GLTextureCutter()
    : m_width(DEFAULT_WIDTH), m_height(DEFAULT_HEIGHT)
{

}
GLTextureCutter::GLTextureCutter(GLsizei width, GLsizei height)
    : m_width(width), m_height(height)
{

}

} // namespace GL //

#endif // GLTEXTURECUTTER_H //

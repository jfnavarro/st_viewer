/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTEXTURECUTTER_H
#define GLTEXTURECUTTER_H

#include "GLCommon.h"
#include "texture/GLTextureData.h"

namespace GL
{
// GLTextureCutter is used to cut a large texture into smaller components,
// each of which are laid out into a grid based fashion.
// Textures are cut into textures with a given size (default 512x512) which
// means end segments may end up smaller (but never bigger) than set
// preferred size.
// Segments laid out accordingly:
class GLTextureCutter
{
public:

    static const GLsizei DEFAULT_WIDTH = 512;
    static const GLsizei DEFAULT_HEIGHT = 512;

    GLTextureCutter();
    virtual ~GLTextureCutter();
    explicit GLTextureCutter(GLsizei width, GLsizei height);

    void cut(int width, int height, GLvoid *pixels, GLTextureData& data);

private:

    GLsizei m_width;
    GLsizei m_height;
};

} // namespace GL //

#endif // GLTEXTURECUTTER_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTEXTURERENDER_H
#define GLTEXTURERENDER_H

#include "texture/GLTextureData.h"

namespace GL
{

// GLTextureRender provides an interface to render texture data objects.
// Current implementation is stateless, but state variables can be added
// to control rendering.
class GLTextureRender
{
public:

    GLTextureRender();
    virtual ~GLTextureRender();

    void render(const GLTextureData& renderData);

private:
    // state variables
};

} // namespace GL //

#endif // GLTEXTURERENDER_H //

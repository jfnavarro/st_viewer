/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTEXTUREDATA_H
#define GLTEXTUREDATA_H

#include <QVector>
#include <QScopedArrayPointer>

#include "data/GLTexture.h"

namespace GL
{

// GLTextureData defines a simple structure for representing multiple
// aligned textures. Primarily used to render multiple textures in one go.
class GLTextureData
{
public:
    inline GLTextureData();

    inline GLTextureData& addTexture(const GLtexture& texture, GLindex* index = 0);
    inline const GLtexture operator[](GLint i) const;
    inline const GLarray<GLtexture> textures() const;
    inline void deallocate();

private:
    typedef QVector<GLtexture> GLtextures;
    GLtextures m_textures;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

inline GLTextureData::GLTextureData() { }

inline GLTextureData& GLTextureData::addTexture(const GLtexture& texture, GLindex* index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_textures.size();
    }
    m_textures.push_back(texture);
    return (*this);
}

inline const GLtexture GLTextureData::operator[](GLint i) const
{
    return m_textures[i];
}

inline const GLarray<GLtexture> GLTextureData::textures() const
{
    return GLarray<GLtexture>((GLsizei) m_textures.size(), static_cast<const GLtexture*>(m_textures.data()));
}

inline void GLTextureData::deallocate()
{
    int size = m_textures.size();
    QScopedArrayPointer<GLtexturehandle> handles(new GLtexturehandle[size]);
    GLtexturehandle* it = handles.data();
    foreach(const GLtexture & texture, m_textures) {
        *(it++) = texture.handle;
    }
    GLtexture::deallocateHandles(size, handles.data());
    m_textures.clear();
}

} // namespace GL //

#endif // GLTEXTUREDATA_H //

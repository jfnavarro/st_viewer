
#include "GLTextureData.h"

namespace GL
{

GLTextureData::GLTextureData()
{

}

GLTextureData::~GLTextureData()
{

}

void GLTextureData::addTexture(QOpenGLTexture *texture, GLindex* index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_textures.size();
    }
    m_textures .push_back(texture);
}

QOpenGLTexture* GLTextureData::getTexture(const GLindex &i) const
{
    return m_textures[i];
}

const GLTextureData::GLtextures &GLTextureData::textures() const
{
    return m_textures;
}

void GLTextureData::addVertex(const QRectF &texture, GLindex* index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_vertices.size();
    }
    m_vertices.push_back(texture);
}

const QRectF &GLTextureData::getVertex(const GLindex &index) const
{
    return m_vertices[index];
}

const GLTextureData::GLvertices &GLTextureData::vertices() const
{
    return m_vertices;
}

void GLTextureData::deallocate()
{
    foreach(QOpenGLTexture *texture, m_textures) {
        texture->release();
        texture->destroy();
    }
    m_textures.clear();
    m_vertices.clear();
}

} // namespace GL //


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

const GLarray<QOpenGLTexture *> GLTextureData::textures() const
{
    return GLarray<QOpenGLTexture*>( (GLsizei)m_textures.size(), m_textures.constData() );
}

void GLTextureData::addVertex(const GLfloat &x, const GLfloat &y, GLindex *index)
{
    addVertex(GL::GLpoint(x,y), index);
}

void GLTextureData::addVertex(const GL::GLpoint &v, GLindex* index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_vertices.size();
    }
    m_vertices.push_back(v);
}

const GL::GLpoint& GLTextureData::getVertex(const GLindex &i) const
{
    return m_vertices[i];
}

const GLarray<GL::GLpoint> GLTextureData::vertices() const
{
    return GLarray<GL::GLpoint>( (GLsizei)m_vertices.size(), m_vertices.constData() );
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

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTEXTUREDATA_H
#define GLTEXTUREDATA_H

#include <QVector>
#include <QScopedArrayPointer>
#include <QOpenGLTexture>

#include "GLCommon.h"

namespace GL
{

// GLTextureData defines a simple structure for representing multiple
// aligned textures. Primarily used to render multiple textures in one go.
class GLTextureData
{
public:

    GLTextureData();
    virtual ~GLTextureData();

    GLTextureData& addTexture(QOpenGLTexture *texture, GLindex* index = 0);
    QOpenGLTexture* getTexture(const GL::GLindex &i) const;
    const GLarray<QOpenGLTexture*> textures() const;

    GLTextureData& addVertex(const GLfloat x, const GLfloat y, GLindex* index = 0);
    GLTextureData& addVertex(const GL::GLpoint &point, GLindex* index = 0);
    const GL::GLpoint& getVertex(const GL::GLindex &i) const;
    const GLarray<GL::GLpoint> vertices() const;

    void deallocate();

private:
    typedef QVector<QOpenGLTexture*> GLtextures;
    typedef QVector<GL::GLpoint> GLvertices;

    GLtextures m_textures;
    GLvertices m_vertices;
};

} // namespace GL //



#endif // GLTEXTUREDATA_H //

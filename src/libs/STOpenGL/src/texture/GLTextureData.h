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

    typedef QVector<QOpenGLTexture*> GLtextures;
    typedef QVector<QRectF> GLvertices;

    GLTextureData();
    virtual ~GLTextureData();

    void addTexture(QOpenGLTexture *texture, GLindex* index = 0);
    QOpenGLTexture* getTexture(const GL::GLindex &i) const;
    const GLtextures& textures() const;

    void addVertex(const QRectF &texture, GLindex* index = 0);
    const QRectF& getVertex(const GL::GLindex &index) const;
    const GLvertices& vertices() const;

    void deallocate();

private:

    GLtextures m_textures;
    GLvertices m_vertices;
};

} // namespace GL //



#endif // GLTEXTUREDATA_H //

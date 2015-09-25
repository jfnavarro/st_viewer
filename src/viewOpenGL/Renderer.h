/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#ifndef STRENDERER_H
#define STRENDERER_H

#include <QSharedPointer>
#include <memory>

class QImage;
class QString;
class QMatrix4x4;
class ColoredLines;
class ColoredQuads;
class STTexturedQuads;
class QOpenGLTexture;

// Transforms and draws sets of Lines and Quads, using OpenGL and interacting with some Qt
// OpenGL types.
//
// The object should always be used with the same context active as the one when it was
// constructed, and the context must remain in scope and be active when the object goes out of
// scope.
//
// The object is safe to copy, with the caveat that each copy must be used with the same OpenGL
// context as the object it was copied from.
class Renderer
{
public:
    Renderer();

    // Applies the transform to the lines (each line is expanded to a 3D point with Z=0 and W=1).
    // The OpenGL context state is unchanged by this call, with the exception of any interaction
    // with the shader program. The currently set shader program is undefined after this call.
    void draw(const QMatrix4x4& transform, const ColoredLines& lines);

    // Applies the transform to the quads (each 2D quad vertex is expanded to a 3D point with
    // Z=0 and W=1). The OpenGL context state is unchanged by this call, with the exception of any
    // interaction with the shader program. The currently set shader program is undefined after
    // this call.
    void draw(const QMatrix4x4& transform, const ColoredQuads& quads);

    // Applies the transform to the quads (each 2D quad vertex is expanded to a 3D point with
    // Z=0 and W=1). The OpenGL context state is unchanged by this call, with the exception of any
    // interaction with the shader program. The currently set shader program is undefined after
    // this call. The quads are rendered with the named 2D texture applied. The texture must
    // previously have been added. If the named texture does not exist, the call proceeds but uses
    // a 'default' texture and issues a warning to qDebug.
    void draw(const QMatrix4x4& transform,
              const QString& textureName,
              const STTexturedQuads& quads);

    // Creates a 2D texture from the image, and associates it with 'name'. The texture will be
    // created with mipmaps and trilinear filtering if makeMipMaps is true, else no mipmaps and
    // nearest point filtering if false. See also removeTexture. If there is already a texture
    // associated with name, it will be removed and the new image replaces it.
    void addTexture(const QString& name, const QImage& image, const bool makeMipMaps);

    // Associates the given texture with 'name'. If there is already a texture associated with
    // name, it will be removed and the new image replaces it. See also removeTexture.
    void addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture);

    // If there is a texture associated with name, the association will be broken and any resources
    // managed by this object will be released.
    void removeTexture(const QString& name);

private:
    class Internals;

    std::shared_ptr<Internals> m_internals;
};

#endif

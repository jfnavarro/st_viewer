#ifndef IMAGEMESHGL_H
#define IMAGEMESHGL_H

#include <QVector2D>
#include <QOpenGLFunctions>
#include <QRectF>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

// This rendering object renders a 3D mesh if the
// user is using 3D mode and has added the mesh file (.obj format)
class ImageMeshGL : public QOpenGLFunctions
{

public:

    ImageMeshGL();
    virtual~ImageMeshGL();

    // initialize the OpenGL context and the shaders
    void init();

    // will remove and destroy all the buffers
    void clearData();

    // load the mesh object and update the buffers
    void loadMesh(const QString &meshFile);

    // draw the mesh
    void draw(const QMatrix4x4 &mvp_matrx);

private:

    // OpenGL rendering data and buffers
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_indexBuf;
    QOpenGLBuffer m_posBuf;
    QOpenGLBuffer m_normBuf;
    QOpenGLShaderProgram *m_program;

    int m_num_triangles;
    bool m_isInitialized;

    Q_DISABLE_COPY(ImageMeshGL);
};

#endif // IMAGEMESHGL_H

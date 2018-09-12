#ifndef CELLGLVIEW3D_H
#define CELLGLVIEW3D_H

#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include "Vertex.h"
#include "data/STData.h"

class QOpenGLShaderProgram;

class CellGLView3D : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:

    explicit CellGLView3D(SettingsWidget::Rendering &rendering_settings);
    virtual ~CellGLView3D() override;

    // return a QImage representation of the canvas
    const QImage grabPixmapGL();

    // clear all local variables and data
    void clearData();

    void attachData(QSharedPointer<STData> data);

    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

protected:

    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:

    void slotUpdate();

protected slots:

    void teardownGL();

signals:

private:

    // rendering settings
    SettingsWidget::Rendering &m_rendering_settings;

    // OpenGL State Information
    QOpenGLBuffer m_pos_buffer;
    QOpenGLBuffer m_color_buffer;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram *m_program;

    // Shader Information
    int u_modelToWorld;
    int u_worldToCamera;
    int u_cameraToView;
    int u_size;

    // camera/projection/view matrices
    QMatrix4x4 m_projection;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_transform;
    QQuaternion m_rotation;
    QVector3D m_translation;

    // helper variables for zooming and panning
    float m_zoom = 1.0f;
    QPoint m_lastPos;

    // Data
    QVector<Vertex> m_vertexs;

    // rendering data
    QSharedPointer<STData> m_geneData;

};
#endif // CELLGLVIEW3D_H

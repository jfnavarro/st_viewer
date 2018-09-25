#ifndef CELLGLVIEW3D_H
#define CELLGLVIEW3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

#include "data/Dataset.h"
#include "data/STData.h"
#include "HeatMapLegendGL.h"
#include "ImageTextureGL.h"

class QOpenGLShaderProgram;

class CellGLView3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:

    explicit CellGLView3D(QWidget *parent = nullptr);
    virtual ~CellGLView3D() override;

    // return a QImage representation of the canvas
    const QImage grabPixmapGL();

    // clear all local variables, buffers and data
    void clearData();

    // attach the ST data object
    void attachDataset(const Dataset &dataset);

    // attach the parameters object
    void attachSettings(SettingsWidget::Rendering *rendering_settings);

    // OpenGL visualization functions
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

    // when the view needs to be refreshed
    void slotUpdate();

protected slots:

    void teardownGL();

signals:

private:

    // rendering settings
    SettingsWidget::Rendering *m_rendering_settings;

    // OpenGL State Information
    QOpenGLBuffer m_pos_buffer;
    QOpenGLBuffer m_color_buffer;
    QOpenGLBuffer m_visible_buffer;
    QOpenGLBuffer m_selected_buffer;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram m_program;
    int m_num_points;
    bool m_initialized;

    // Shader Information (uniforms)
    int u_modelToWorld;
    int u_worldToCamera;
    int u_cameraToView;
    int u_size;
    int u_alpha;

    // camera/projection/view matrices
    QMatrix4x4 m_projection;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_transform;
    QQuaternion m_rotation;
    QVector3D m_translation;

    // alignment data to image (if applicable)
    QTransform m_aligment;

    // helper variables for zooming and panning
    float m_zoom = 1.0f;
    QPoint m_lastPos;

    // rendering data
    QSharedPointer<STData> m_geneData;

    // rendering objects
    QScopedPointer<ImageTextureGL> m_image;
    QScopedPointer<HeatMapLegendGL> m_legend;
};
#endif // CELLGLVIEW3D_H

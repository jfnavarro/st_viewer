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
#include "ImageMeshGL.h"

class QOpenGLShaderProgram;
class QRubberBand;

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

    void slotZoomIn();
    void slotZoomOut();
    void slotRotate(const double angle);
    void slotFlip(const double angle);

    void slotRubberBandSelectionMode(const bool rubberband);
    void slotLassoSelectionMode(const bool lasso);

    void slotLegendVisible(const bool visible);
    void slotImageVisible(const bool visible);

protected slots:

    void teardownGL();

signals:

private:

    void sendSelectionEvent(const QPainterPath &path, const QMouseEvent *event);
    const QMatrix4x4 viewMatrix3D() const;
    const QMatrix4x4 viewMatrix2D() const;
    const QMatrix4x4 projectionMatrix3D() const;
    const QMatrix4x4 projectionMatrix2D() const;
    const QVector3D cameraPosition();
    void setPan(const double dx, const double dy, const double dz, const bool view);
    void setRotation(const double azim, const double elevation);

    // rendering settings
    SettingsWidget::Rendering *m_rendering_settings;

    // OpenGL buffers
    QOpenGLBuffer m_pos_buffer;
    QOpenGLBuffer m_color_buffer;
    QOpenGLBuffer m_visible_buffer;
    QOpenGLBuffer m_selected_buffer;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram m_program;
    int m_num_points;
    bool m_initialized;

    // Shader Information (uniforms)
    int u_mvp_matrix;
    int u_size;
    int u_alpha;

    // flags to show or not the legend and the tissue image
    bool m_legend_show;
    bool m_image_show;

    // helper variables for zooming, rotation and panning
    QPoint m_pos;
    double m_zoom;
    double m_rotate_factor;
    double m_flip_factor;
    double m_elevation;
    double m_azimuth;
    double m_centerX;
    double m_centerY;
    double m_centerZ;
    double m_dist;
    double m_fov;

    // helper variables for selection
    QPoint m_originSelection;
    bool m_rubberBanding;
    bool m_lassoSelection;
    QScopedPointer<QRubberBand> m_rubberband;
    QPainterPath m_lasso;

    // dataset (to be rendered)
    Dataset m_dataset;

    // rendering objects
    QScopedPointer<ImageTextureGL> m_image;
    QScopedPointer<ImageMeshGL> m_mesh;
    QScopedPointer<HeatMapLegendGL> m_legend;
};
#endif // CELLGLVIEW3D_H

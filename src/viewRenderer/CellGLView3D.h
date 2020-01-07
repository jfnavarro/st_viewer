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
    void slotRotateX(const double angle);
    void slotRotateY(const double angle);
    void slotRotateZ(const double angle);
    void slotFlip(const double angle);

    void slotSelectionMode(const bool selection);
    void slotLassoSelectionMode(const bool lasso);

    void slotLegendVisible(const bool visible);
    void slotImageVisible(const bool visible);

protected slots:

    void teardownGL();

signals:

private:

    void sendSelectionEvent(const QPainterPath &path, const QMouseEvent *event);
    const QTransform sceneTransformations() const;
    void setSceneFocusCenterPoint(const QPointF &center_point);
    void setZoomFactor(const double zoom);

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
    int u_mvp_matrix;
    int u_size;
    int u_alpha;

    // camera/projection/view matrices
    QMatrix4x4 m_projection;
    QMatrix4x4 m_transform;

    // alignment data to image (if applicable)
    QTransform m_aligment;

    bool m_legend_show;
    bool m_image_show;

    // helper variables for zooming, rotation and panning
    double m_zoom;
    QPoint m_lastPos;
    QPoint m_originPanning;
    QPointF m_scene_focus_center_point;
    bool m_panning;
    QRectF m_boundingRect;
    QRectF m_boundingRectImage;
    double m_rotateX;
    double m_rotateY;
    double m_rotateZ;
    double m_flip_factor;

    // helper variables for selection
    QPoint m_originRubberBand;
    QPoint m_originLasso;
    bool m_rubberBanding;
    bool m_lassoSelection;
    bool m_selecting;
    QScopedPointer<QRubberBand> m_rubberband;
    QPainterPath m_lasso;

    // rendering data
    QSharedPointer<STData> m_geneData;

    // rendering objects
    QScopedPointer<ImageTextureGL> m_image;
    QScopedPointer<HeatMapLegendGL> m_legend;
};
#endif // CELLGLVIEW3D_H

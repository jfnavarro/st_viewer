#ifndef CELLGLVIEW3D_H
#define CELLGLVIEW3D_H

#include <QQuaternion>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "data/STData.h"
#include "GraphicItemGL.h"

class CellGLView3D : public QOpenGLWidget
{
    Q_OBJECT

public:

    explicit CellGLView3D(SettingsWidget::Rendering &rendering_settings,
                          QWidget *parent = nullptr);
    virtual ~CellGLView3D() override;

    // return a QImage representation of the canvas
    const QImage grabPixmapGL();

    // clear all local variables and data
    void clearData();

    void attachData(QSharedPointer<STData> data);

protected:

    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:

signals:

private:

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    // rendering settings
    SettingsWidget::Rendering &m_rendering_settings;

    QMatrix4x4 m_projection;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    float m_zoom;
    QPoint m_lastPos;

    // a cross platform wrapper around OpenGL functions
    GraphicItemGL::QOpenGLFunctionsVersion m_qopengl_functions;

    // rendering data
    QSharedPointer<STData> m_geneData;
};
#endif // CELLGLVIEW3D_H

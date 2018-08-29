#ifndef CELLGLVIEW3D_H
#define CELLGLVIEW3D_H

#include <QQuaternion>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include "data/STData.h"
#include "GraphicItemGL.h"

class CellGLView3D : public QOpenGLWidget
{
    Q_OBJECT

public:

    explicit CellGLView3D(SettingsWidget::Rendering &rendering_settings,
                          QWidget *parent = nullptr);
    ~CellGLView3D();

    void addData(QSharedPointer<STData> data);

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
    // rendering settings
    SettingsWidget::Rendering &m_rendering_settings;

    QMatrix4x4 m_projection;
    QQuaternion m_rotation;
    QPointF m_lastPoint;
    float m_zoom;

    // a cross platform wrapper around OpenGL functions
    GraphicItemGL::QOpenGLFunctionsVersion m_qopengl_functions;

    // rendering data
    QSharedPointer<STData> m_geneData;
};
#endif // CELLGLVIEW3D_H

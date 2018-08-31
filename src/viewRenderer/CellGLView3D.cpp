#include "CellGLView3D.h"
#include "color/HeatMap.h"

static const int OPENGL_VERSION_MAJOR = 3;
static const int OPENGL_VERSION_MINOR = 2;

#include <random>

float get_random_float(float min, float max)
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(min, max);
    return dis(e);
}

QVector4D fromQtColor(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

CellGLView3D::CellGLView3D(SettingsWidget::Rendering &rendering_settings, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_rendering_settings(rendering_settings)
    , m_projection()
    , m_xRot(0)
    , m_yRot(0)
    , m_zRot(0)
    , m_zoom(1.0f)
{
    // Configure OpenGL format for this view
    QSurfaceFormat format;
    format.setVersion(OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
    format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setDepthBufferSize(24);
    setFormat(format);
}

CellGLView3D::~CellGLView3D()
{
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void CellGLView3D::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        update();
    }
}

void CellGLView3D::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        update();
    }
}

void CellGLView3D::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        update();
    }
}


void CellGLView3D::initializeGL()
{
    if (!m_qopengl_functions.initializeOpenGLFunctions()) {
        QMessageBox::critical(this,
                              tr("STViewer"),
                              tr("Required OpenGL version not supported.\n"
                                 "Please update your system to at least OpenGL ")
                              + QString("%1.%2").arg(OPENGL_VERSION_MAJOR).arg(OPENGL_VERSION_MINOR)
                              + ".");
        QApplication::exit();
        return;
    }

    m_qopengl_functions.glClearColor(0, 0, 0, 1);
    m_qopengl_functions.glEnable(GL_ALPHA_TEST);
    m_qopengl_functions.glAlphaFunc(GL_NOTEQUAL, 0);
    m_qopengl_functions.glEnable(GL_BLEND);
    m_qopengl_functions.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_qopengl_functions.glEnable(GL_POINT_SMOOTH);
    m_qopengl_functions.glEnable(GL_DEPTH_TEST);
    m_qopengl_functions.glEnable(GL_CULL_FACE);
}

void CellGLView3D::resizeGL(int width, int height)
{
    const float aspect = float(width) / float(height);
    const float zNear = 0.1f, zFar = 100.0f, fov = 45.0f;
    m_projection.setToIdentity();
    m_projection.perspective(fov, aspect, zNear, zFar);
}

void CellGLView3D::paintGL()
{
    const bool is_dynamic =
            m_rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange;
    const bool do_values = m_rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;

    const auto &spots = m_geneData->spots();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &colors = m_geneData->renderingColors();
    const auto &values = m_geneData->renderingValues();
    const float size = m_rendering_settings.size;
    const double min_value = m_rendering_settings.legend_min;
    const double max_value = m_rendering_settings.legend_max;
    const double intensity = m_rendering_settings.intensity;
    m_qopengl_functions.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_qopengl_functions.glMatrixMode(GL_PROJECTION);
    m_qopengl_functions.glLoadMatrixf(m_projection.constData());

    m_qopengl_functions.glMatrixMode(GL_MODELVIEW);
    QMatrix4x4 m_world;
    m_world.translate(0.0, 0.0, -10.0);
    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);
    m_world.scale(m_zoom, m_zoom, 1.0);
    m_qopengl_functions.glLoadMatrixf(m_world.constData());

    m_qopengl_functions.glPointSize(size);
    m_qopengl_functions.glBegin(GL_POINTS);
    for (int i = 0; i < spots.size(); ++i) {
        const bool visible = visibles.at(i);
        const auto spot  = spots.at(i)->adj_coordinates();
        const double x = spot.x;
        const double y = spot.y;
        const double z = spot.z;
        if (visible) {
            const double value = values.at(i);
            QColor color = colors.at(i);
            if (do_values && !spots.at(i)->visible()) {
                color = Color::adjustVisualMode(color, value, min_value,
                                                max_value, m_rendering_settings.visual_mode);
            }

            if (!is_dynamic) {
                color.setAlphaF(intensity);
            }

            const QVector4D vcolor = fromQtColor(color);
            m_qopengl_functions.glColor4fv(reinterpret_cast<const float*>(&vcolor));
            m_qopengl_functions.glVertex3f(x, y, z);
        }
    }
    m_qopengl_functions.glEnd();
    m_qopengl_functions.glFlush();
}

void CellGLView3D::wheelEvent(QWheelEvent *event)
{
    const float delta = event->delta();
    if (delta > 0) {
        m_zoom -= 0.5f;
    } else if (delta < 0) {
        m_zoom += 0.5f;
    }
    update();
}

void CellGLView3D::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void CellGLView3D::mouseMoveEvent(QMouseEvent *event)
{
    const int dx = event->x() - m_lastPos.x();
    const int dy = event->y() - m_lastPos.y();
    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}

void CellGLView3D::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

void CellGLView3D::attachData(QSharedPointer<STData> geneData)
{
    m_geneData = geneData;
}

void CellGLView3D::clearData()
{

}

const QImage CellGLView3D::grabPixmapGL()
{
    const QPixmap res = grab(QRect(0,0,width(),height()));
    return res.toImage();
}

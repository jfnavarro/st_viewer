#include "CellGLView3D.h"
#include "color/HeatMap.h"

static const int OPENGL_VERSION_MAJOR = 2;
static const int OPENGL_VERSION_MINOR = 0;

CellGLView3D::CellGLView3D(SettingsWidget::Rendering &rendering_settings, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_rendering_settings(rendering_settings)
    , m_projection()
    , m_rotation()
    , m_zoom(1.0f)
{
    // Configure OpenGL format for this view
    QSurfaceFormat format;
    format.setVersion(OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
    format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    //format.setStereo(false);
    //format.setStencilBufferSize(0);
    //format.setDepthBufferSize(0);
    //format.setSwapInterval(0);
    setFormat(format);
}

CellGLView3D::~CellGLView3D()
{
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

    m_qopengl_functions.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_qopengl_functions.glEnable(GL_DEPTH_TEST);
    m_qopengl_functions.glDepthFunc(GL_LESS);
    m_qopengl_functions.glEnable(GL_CULL_FACE);
    m_qopengl_functions.glDepthFunc(GL_LEQUAL);
    m_qopengl_functions.glShadeModel(GL_SMOOTH);
    m_qopengl_functions.glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
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
    m_qopengl_functions.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_qopengl_functions.glMatrixMode(GL_PROJECTION);
    m_qopengl_functions.glLoadMatrixf(m_projection.constData());

    m_qopengl_functions.glMatrixMode(GL_MODELVIEW);
    QMatrix4x4 mvMatrix;
    mvMatrix.scale(m_zoom);
    mvMatrix.rotate(m_rotation);
    m_qopengl_functions.glLoadMatrixf(mvMatrix.constData());

    const bool is_dynamic =
            m_rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange;
    const bool do_values = m_rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;

    const auto &spots = m_geneData->spots();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &colors = m_geneData->renderingColors();
    const auto &values = m_geneData->renderingValues();
    const float size = m_rendering_settings.size * 10;
    const double min_value = m_rendering_settings.legend_min;
    const double max_value = m_rendering_settings.legend_max;
    const double intensity = m_rendering_settings.intensity;
    double r,g,b,a;

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

            color.getRgbF(&r, &g, &b, &a);
            m_qopengl_functions.glColor4f(r, g, b, a);
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
        m_zoom /= 0.8f;
    } else if (delta < 0) {
        m_zoom *= 0.8f;
    }
    event->accept();
    update();
}

void CellGLView3D::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        m_lastPoint = event->localPos();
        event->accept();
    }
}

void CellGLView3D::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        const QPointF delta = event->localPos() - m_lastPoint;
        m_lastPoint = event->localPos();

        QQuaternion dt = QQuaternion::fromEulerAngles(delta.y() * 0.1f, delta.x() * 0.1f, 0);
        m_rotation = m_rotation * dt;

        update();
        event->accept();
    }
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

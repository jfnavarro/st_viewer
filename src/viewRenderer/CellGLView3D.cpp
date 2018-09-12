#include "CellGLView3D.h"
#include "color/HeatMap.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QList>
#include <random>

static const float transSpeed = 0.5f;
static const float rotSpeed = 0.5f;
static const QVector3D FORWARD(0.0f, 0.0f, -0.1f);
static const QVector3D UP(0.0f, 0.1f, 0.0f);
static const QVector3D RIGHT(0.1f, 0.0f, 0.0f);

QVector4D fromQtColor(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

float get_random_float(float min, float max)
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(min, max);
    return dis(e);
}

// Qt Streams
QDebug operator<<(QDebug dbg, const Vertex &vertex)
{
    dbg << "Position: <" << vertex.position() << ", " << "Color:" << vertex.color();
    return dbg;
}

CellGLView3D::CellGLView3D(SettingsWidget::Rendering &rendering_settings)
    : m_rendering_settings(rendering_settings)
{

}

CellGLView3D::~CellGLView3D()
{

}

void CellGLView3D::teardownGL()
{
    // Actually destroy our OpenGL information
    m_vao.destroy();
    m_pos_buffer.destroy();
    m_color_buffer.destroy();
    delete m_program;
}

void CellGLView3D::initializeGL()
{
    qDebug() << "initializeGL()";
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);

    // Set global information
    glEnable(GL_CULL_FACE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Create Shader
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/geneShader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/geneShader.frag");
    m_program->link();
    m_program->bind();

    // Cache Uniform Locations
    u_modelToWorld = m_program->uniformLocation("modelToWorld");
    u_worldToCamera = m_program->uniformLocation("worldToCamera");
    u_cameraToView = m_program->uniformLocation("cameraToView");
    u_size = m_program->uniformLocation("size");

    m_program->release();
}

void CellGLView3D::resizeGL(int width, int height)
{
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width / float(height), 0.1f, 1000.0f);
}

void CellGLView3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render using our shader
    m_program->bind();
    m_camera.setToIdentity();
    m_camera.translate(-m_translation);
    m_program->setUniformValue(u_worldToCamera, m_camera);
    m_program->setUniformValue(u_cameraToView, m_projection);
    const int size = m_rendering_settings.size * 10;
    m_program->setUniformValue(u_size, size);
    {
        m_vao.bind();
        m_transform.setToIdentity();
        m_transform.translate(0.0f, 0.0f, -5.0f);
        m_transform.rotate(m_rotation);
        m_transform.scale(m_zoom, m_zoom, 1.0f);
        m_program->setUniformValue(u_modelToWorld, m_transform);
        glDrawArrays(GL_POINTS, 0, m_vertexs.size());
        m_vao.release();
    }
    m_program->release();
}

void CellGLView3D::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        event->ignore();
    }
    else
    {
        Qt::KeyboardModifiers modifiers = event->modifiers();
        if (modifiers.testFlag(Qt::ShiftModifier) && event->key() == Qt::Key_Up)
        {
            m_translation += (transSpeed * -FORWARD);
        }
        else if (modifiers.testFlag(Qt::ShiftModifier) && event->key() == Qt::Key_Down)
        {
            m_translation += (transSpeed * FORWARD);
        }
        else if (event->key() == Qt::Key_Right)
        {
            m_translation += (transSpeed * -RIGHT);
        }
        else if (event->key() == Qt::Key_Left)
        {
            m_translation += (transSpeed * RIGHT);
        }
        else if (event->key() == Qt::Key_Up)
        {
            m_translation += (transSpeed * -UP);
        }
        else if (event->key() == Qt::Key_Down)
        {
            m_translation += (transSpeed * UP);
        }
    }
    update();
}

void CellGLView3D::wheelEvent(QWheelEvent *event)
{
    //TODO have a max/min zoom level
    const float delta = event->delta();
    if (delta > 0) {
        m_zoom -= 0.5f;
    } else if (delta < 0) {
        m_zoom += 0.5f;
    }
    event->ignore();
}

void CellGLView3D::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    event->ignore();
}

void CellGLView3D::mouseMoveEvent(QMouseEvent *event)
{
    const int dx = event->x() - m_lastPos.x();
    const int dy = event->y() - m_lastPos.y();
    if (event->buttons() & Qt::LeftButton) {
        m_rotation *= QQuaternion::fromAxisAndAngle(UP, -rotSpeed * dx);
        m_rotation *= QQuaternion::fromAxisAndAngle(RIGHT, -rotSpeed * dy);
    } else if (event->buttons() & Qt::RightButton) {
        //TODO translate from mouse position
        m_translation += (transSpeed * dx * -RIGHT);
        m_translation += (transSpeed * dy * UP);
    }
    m_lastPos = event->pos();
    event->ignore();
    update();
}

void CellGLView3D::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

void CellGLView3D::attachData(QSharedPointer<STData> geneData)
{
    m_geneData = geneData;
}

void CellGLView3D::slotUpdate()
{
    qDebug() << "slotUpdate()";

    m_geneData->computeRenderingData(m_rendering_settings);

    const bool is_dynamic =
            m_rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange;
    const bool do_values = m_rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;

    const auto &spots = m_geneData->spots();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &colors = m_geneData->renderingColors();
    const auto &values = m_geneData->renderingValues();
    const double min_value = m_rendering_settings.legend_min;
    const double max_value = m_rendering_settings.legend_max;
    const double intensity = m_rendering_settings.intensity;

    m_vertexs.clear();
    for (int i = 0; i < spots.size(); ++i) {
        const bool visible = visibles.at(i);
        if (visible) {
            const double value = values.at(i);
            QColor color = colors.at(i);
            const auto spot = spots.at(i);
            if (do_values && !spot->visible()) {
                color = Color::adjustVisualMode(color, value, min_value,
                                                max_value, m_rendering_settings.visual_mode);
            }

            if (!is_dynamic) {
                color.setAlphaF(intensity);
            }
            m_vertexs.append(Vertex(spot->adj_coordinates(), fromQtColor(color)));
        }
    }

    {
        makeCurrent();

        m_program->bind();

        // Create VAO
        m_vao.create();
        m_vao.bind();

        // Create Buffer (Index)
        m_pos_buffer.create();
        m_pos_buffer.bind();
        m_pos_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_pos_buffer.allocate(m_vertexs.constData(), m_vertexs.size() * sizeof(Vertex));
        m_program->enableAttributeArray(0);
        m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                      Vertex::PositionTupleSize, Vertex::stride());

        // Create Buffer (Color)
        m_color_buffer.create();
        m_color_buffer.bind();
        m_color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_color_buffer.allocate(m_vertexs.constData(), m_vertexs.size() * sizeof(Vertex));
        m_program->enableAttributeArray(1);
        m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                      Vertex::ColorTupleSize, Vertex::stride());

        // Release (unbind) all
        m_pos_buffer.release();
        m_color_buffer.release();
        m_vao.release();
        m_program->release();

        doneCurrent();
    }

    update();
}

void CellGLView3D::clearData()
{
    //m_vertex.destroy();
    //m_object.destroy();
    //m_geneData.clear();
    //m_vertexs.clear();
    m_zoom = 1.0f;
    m_transform.setToIdentity();
    m_rotation = QQuaternion();
    m_translation = QVector3D();
}

const QImage CellGLView3D::grabPixmapGL()
{
    return QImage();

    //const QPixmap res = grab(QRect(0,0,width(),height()));
    //return res.toImage();
}

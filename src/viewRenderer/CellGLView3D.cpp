#include "CellGLView3D.h"
#include "color/HeatMap.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QList>
#include <random>

static const float transSpeed = 0.1f;
static const float rotSpeed = 0.1f;
static const QVector3D FORWARD(0.0f, 0.0f, -0.1f);
static const QVector3D UP(0.0f, 0.1f, 0.0f);
static const QVector3D RIGHT(0.1f, 0.0f, 0.0f);


CellGLView3D::CellGLView3D(SettingsWidget::Rendering &rendering_settings, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_rendering_settings(rendering_settings)
    , m_num_points(0)
    , m_initialized(false)
    , m_zoom(1.0f)
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
    glEnable(GL_POINT_SMOOTH);
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
    u_alpha = m_program->uniformLocation("alpha");

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

    if (!m_initialized) {
        return;
    }

    // Render using our shader
    m_program->bind();
    m_camera.setToIdentity();
    m_camera.translate(-m_translation);
    m_program->setUniformValue(u_worldToCamera, m_camera);
    m_program->setUniformValue(u_cameraToView, m_projection);
    m_program->setUniformValue(u_size, m_rendering_settings.size);
    const double alpha =
            m_rendering_settings.visual_mode == SettingsWidget::DynamicRange ?
                1.0 : m_rendering_settings.intensity;
    m_program->setUniformValue(u_alpha, static_cast<GLfloat>(alpha));
    {
        m_vao.bind();
        m_transform.setToIdentity();
        m_transform.translate(0.0f, 0.0f, -5.0f);
        m_transform.rotate(m_rotation);
        m_transform.scale(m_zoom, m_zoom, 1.0f);
        m_program->setUniformValue(u_modelToWorld, m_transform);
        glDrawArrays(GL_POINTS, 0, m_num_points);
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
    update();
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

    const auto &indexes = m_geneData->renderingCoords();
    const auto &colors = m_geneData->renderingColors();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &selecteds = m_geneData->renderingSelected();
    m_num_points = indexes.size();

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
        m_pos_buffer.allocate(indexes.constData(), indexes.size() * sizeof(QVector3D));
        m_program->enableAttributeArray(0);
        m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

        // Create Buffer (Color)
        m_color_buffer.create();
        m_color_buffer.bind();
        m_color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_color_buffer.allocate(colors.constData(), colors.size() * sizeof(QVector4D));
        m_program->enableAttributeArray(1);
        m_program->setAttributeBuffer(1, GL_FLOAT, 0, 4, 0);

        // Create Buffer (Selected)
        m_selected_buffer.create();
        m_selected_buffer.bind();
        m_selected_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_selected_buffer.allocate(selecteds.constData(), selecteds.size() * sizeof(int));
        m_program->enableAttributeArray(2);
        m_program->setAttributeBuffer(2, GL_INT, 0, 1, 0);

        // Create Buffer (Visible)
        m_visible_buffer.create();
        m_visible_buffer.bind();
        m_visible_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_visible_buffer.allocate(visibles.constData(), visibles.size() * sizeof(int));
        m_program->enableAttributeArray(3);
        m_program->setAttributeBuffer(3, GL_INT, 0, 1, 0);

        // Release (unbind) all
        m_pos_buffer.release();
        m_color_buffer.release();
        m_selected_buffer.release();
        m_visible_buffer.release();
        m_vao.release();
        m_program->release();

        doneCurrent();
    }

    m_initialized = true;
    update();
}

void CellGLView3D::slotUpdate()
{
    m_geneData->computeRenderingData(m_rendering_settings);
    const auto &colors = m_geneData->renderingColors();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &selecteds = m_geneData->renderingSelected();

    {
        makeCurrent();

        // Update Buffer (Color)
        m_color_buffer.bind();
        void* buffer_data_color = m_color_buffer.mapRange(0,
                                                          colors.size(),
                                                          QOpenGLBuffer::RangeWrite);
        std::memcpy(buffer_data_color, colors.constData(), colors.size() * sizeof(QVector4D));
        m_color_buffer.unmap();

        // Update Buffer (Selected)
        m_selected_buffer.bind();
        void* buffer_data_selected = m_selected_buffer.mapRange(0,
                                                                selecteds.size(),
                                                                QOpenGLBuffer::RangeWrite);
        std::memcpy(buffer_data_selected, selecteds.constData(), selecteds.size() * sizeof(int));
        m_selected_buffer.unmap();

        // Update Buffer (Visible)
        m_visible_buffer.bind();
        void* buffer_data_visible = m_visible_buffer.mapRange(0,
                                                              visibles.size(),
                                                              QOpenGLBuffer::RangeWrite);
        std::memcpy(buffer_data_visible, visibles.constData(), visibles.size() * sizeof(int));
        m_visible_buffer.unmap();

        // Release (unbind) all
        m_color_buffer.release();
        m_selected_buffer.release();
        m_visible_buffer.release();

        doneCurrent();
    }

    update();
}

void CellGLView3D::clearData()
{
    m_pos_buffer.destroy();
    m_color_buffer.destroy();
    m_visible_buffer.destroy();
    m_selected_buffer.destroy();
    m_vao.destroy();
    m_zoom = 1.0f;
    m_num_points = 0;
    m_initialized = false;
    m_transform.setToIdentity();
    m_rotation = QQuaternion();
    m_translation = QVector3D();
}

const QImage CellGLView3D::grabPixmapGL()
{
    const QPixmap res = grab(QRect(0,0,width(),height()));
    return res.toImage();
}

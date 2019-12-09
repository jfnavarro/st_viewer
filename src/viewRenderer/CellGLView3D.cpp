#include "CellGLView3D.h"
#include "color/HeatMap.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QList>
#include <random>

static const float transSpeed = 0.5f;
static const float rotSpeed = 0.1f;
static const QVector3D FORWARD(0.0f, 0.0f, -0.1f);
static const QVector3D UP(0.0f, 0.1f, 0.0f);
static const QVector3D RIGHT(0.1f, 0.0f, 0.0f);
static const QColor lasso_color = QColor(0,0,255,90);

CellGLView3D::CellGLView3D(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_rendering_settings(nullptr)
    , m_num_points(0)
    , m_initialized(false)
    , m_zoom(1.0f)
    , m_image(nullptr)
    , m_legend(nullptr)
{

}

CellGLView3D::~CellGLView3D()
{
    m_rendering_settings = nullptr;
}

void CellGLView3D::teardownGL()
{
    // Actually destroy our OpenGL information
    m_vao.destroy();
    m_pos_buffer.destroy();
    m_color_buffer.destroy();
    m_selected_buffer.destroy();
    m_visible_buffer.destroy();
}

void CellGLView3D::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);

    // Set global information
    glDisable(GL_CULL_FACE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile Shaders
    m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/geneShader.vert");
    m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/geneShader.frag");
    m_program.link();
    m_program.bind();

    // Cache Uniform Locations
    u_mvp_matrix = m_program.uniformLocation("mvp_matrix");
    u_size = m_program.uniformLocation("size");
    u_alpha = m_program.uniformLocation("alpha");

    m_program.release();

    // image texture graphical object
    m_image.reset(new ImageTextureGL());
    m_image->init();

    // heatmap component
    m_legend.reset(new HeatMapLegendGL());
}

void CellGLView3D::resizeGL(int width, int height)
{
    if (!m_initialized) {
        return;
    }

    m_projection.setToIdentity();
    if (m_geneData->is3D()) {
        m_projection.perspective(60.0f, width / float(height), 0.1f, 1000.0f);
    } else {
        m_projection.ortho(QRect(0.0, 0.0, width, height));
    }
}

void CellGLView3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_initialized) {
        return;
    }

    // Compute local transformations
    m_transform.setToIdentity();
    if (m_geneData->is3D()) {
        m_transform.translate(0.0f, 0.0f, -5.0f);
        m_transform.rotate(m_rotation);
        m_transform.scale(1.0f, 1.0f, m_zoom);
    } else {
        const QRectF image_rect = m_image->boundingRect();
        //const QRectF image_rect = m_geneData->getBorder();
        const QPointF image_center = image_rect.center();
        const double viewport_w = static_cast<double>(width());
        const double viewport_h = static_cast<double>(height());
        QTransform image_trans;
        image_trans.translate(image_center.x(), image_center.y());
        image_trans.scale(m_zoom, m_zoom);
        image_trans.translate(-viewport_w / 2, -viewport_h / 2);
        m_transform = QMatrix4x4(m_aligment * image_trans.inverted());
        // render image
        m_image->draw(m_projection * image_trans.inverted());
    }

    // render legend
    QPainter painter(this);
    painter.setWorldMatrixEnabled(true);
    painter.setViewTransformEnabled(true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setWorldTransform(local_transform);
    m_legend->draw(*m_rendering_settings, painter);
    //painter.resetTransform();

    const double alpha =
            m_rendering_settings->visual_mode == SettingsWidget::DynamicRange ?
                1.0 : m_rendering_settings->intensity;
    m_camera.setToIdentity();
    m_camera.translate(-m_translation);

    // Render gene data
    m_program.bind();
    m_program.setUniformValue(u_size, m_rendering_settings->size * 2);
    m_program.setUniformValue(u_alpha, static_cast<GLfloat>(alpha));
    m_program.setUniformValue(u_mvp_matrix, m_projection * m_camera * m_transform);
    {
        m_vao.bind();
        glDrawArrays(GL_POINTS, 0, m_num_points);
        m_vao.release();
    }
    m_program.release();

    // draw selection box/lasso
    if (m_selecting && m_lassoSelection && !m_lasso.isEmpty()) {
        painter.setBrush(lasso_color);
        painter.setPen(lasso_color);
        painter.drawPath(m_lasso.simplified());
    }
}

void CellGLView3D::slotZoomIn()
{

}

void CellGLView3D::slotZoomOut()
{

}

void CellGLView3D::slotRotate(const float angle)
{

}

void CellGLView3D::slotFlip(const float angle)
{

}

void CellGLView3D::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if (modifiers.testFlag(Qt::ShiftModifier) && event->key() == Qt::Key_Up) {
        m_translation += (transSpeed * -FORWARD);
    } else if (modifiers.testFlag(Qt::ShiftModifier) && event->key() == Qt::Key_Down) {
        m_translation += (transSpeed * FORWARD);
    } else if (event->key() == Qt::Key_Right) {
        m_translation += (transSpeed * -RIGHT);
    } else if (event->key() == Qt::Key_Left) {
        m_translation += (transSpeed * RIGHT);
    } else if (event->key() == Qt::Key_Up) {
        m_translation += (transSpeed * -UP);
    } else if (event->key() == Qt::Key_Down) {
        m_translation += (transSpeed * UP);
    }
    event->ignore();
    update();
}

void CellGLView3D::wheelEvent(QWheelEvent *event)
{
    //TODO have a max/min zoom level
    const float delta = event->delta();
    if (delta > 0) {
        m_zoom -= 0.1f;
    } else if (delta < 0) {
        m_zoom += 0.1f;
    }
    event->ignore();
    update();
}

void CellGLView3D::mousePressEvent(QMouseEvent *event)
{
    const bool is_left = event->button() == Qt::LeftButton;
    if (!m_geneData->is3D()) {
        if (is_left && m_selecting && !m_lassoSelection) {
            // rubberbanding changes cursor to pointing hand
            setCursor(Qt::PointingHandCursor);
            m_rubberBanding = true;
            //m_originRubberBand = event->pos();
            //m_rubberband->setGeometry(QRect(m_originRubberBand, QSize()));
            //m_rubberband->show();
        } else if (is_left && m_selecting) {
            m_lasso = QPainterPath();
            //m_originLasso = event->pos();
            //m_lasso.moveTo(m_originLasso);
            update();
        } else if (is_left) {
            //m_panning = true;
            //m_originPanning = event->globalPos(); // panning needs globalPos
            // panning changes cursor to closed hand
            setCursor(Qt::ClosedHandCursor);
        }
    }
    event->ignore();

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

void CellGLView3D::attachSettings(SettingsWidget::Rendering *rendering_settings)
{
    m_rendering_settings = rendering_settings;
}

void CellGLView3D::attachDataset(const Dataset &dataset)
{
    m_geneData = dataset.data();

    makeCurrent();

    // If the dataset contains a valid image we load it and we also load the
    // transformation matrix
    if (!dataset.imageFile().isNull() && !dataset.imageFile().isEmpty() && !m_geneData->is3D()) {
        QTransform alignment = dataset.imageAlignment();
        const bool result = m_image->createTiles(dataset.imageFile());
        if (!result) {
            QMessageBox::warning(this, tr("Tissue image"), tr("Error loading tissue image"));
        } else {
            qDebug() << "Setting image of size " << m_image->boundingRect();
            // If the user has not given any transformation matrix
            // we compute a simple transformation matrix using
            // the image and chip dimensions so the spot's coordinates
            // can be mapped to the image's coordinates space
            if (alignment.isIdentity()) {
                const QRect chip = dataset.chip();
                const double chip_x2 = static_cast<double>(chip.width());
                const double chip_y2 = static_cast<double>(chip.height());
                const double width_image = static_cast<double>(m_image->boundingRect().width());
                const double height_image = static_cast<double>(m_image->boundingRect().height());
                const double a11 = width_image / (chip_x2 - 1);
                const double a12 = 0.0;
                const double a13 = 0.0;
                const double a21 = 0.0;
                const double a22 = height_image / (chip_y2 - 1);
                const double a23 = 0.0;
                const double a31 = -a11;
                const double a32 = -a22;
                const double a33 = 1.0;
                alignment.setMatrix(a11, a12, a13, a21, a22, a23, a31, a32, a33);
            } else if (m_image->scaled()) {
                alignment *= QTransform::fromScale(0.5, 0.5);
            }
        }
        m_aligment = alignment;
        qDebug() << "Setting alignment matrix to " << alignment;
    }

    const auto &indexes = m_geneData->renderingCoords();
    const auto &colors = m_geneData->renderingColors();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &selecteds = m_geneData->renderingSelected();
    m_num_points = indexes.size();

    {
        m_program.bind();

        // Create VAO
        m_vao.create();
        m_vao.bind();

        // Create Buffer (Index)
        m_pos_buffer.create();
        m_pos_buffer.bind();
        m_pos_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_pos_buffer.allocate(indexes.constData(), indexes.size() * sizeof(QVector3D));
        m_program.enableAttributeArray(0);
        m_program.setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

        // Create Buffer (Color)
        m_color_buffer.create();
        m_color_buffer.bind();
        m_color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_color_buffer.allocate(colors.constData(), colors.size() * sizeof(QVector4D));
        m_program.enableAttributeArray(1);
        m_program.setAttributeBuffer(1, GL_FLOAT, 0, 4, 0);

        // Create Buffer (Selected)
        m_selected_buffer.create();
        m_selected_buffer.bind();
        m_selected_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_selected_buffer.allocate(selecteds.constData(), selecteds.size() * sizeof(int));
        m_program.enableAttributeArray(2);
        m_program.setAttributeBuffer(2, GL_INT, 0, 1, 0);

        // Create Buffer (Visible)
        m_visible_buffer.create();
        m_visible_buffer.bind();
        m_visible_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_visible_buffer.allocate(visibles.constData(), visibles.size() * sizeof(int));
        m_program.enableAttributeArray(3);
        m_program.setAttributeBuffer(3, GL_INT, 0, 1, 0);

        // Release (unbind) all
        m_pos_buffer.release();
        m_color_buffer.release();
        m_selected_buffer.release();
        m_visible_buffer.release();
        m_vao.release();
        m_program.release();
    }

    doneCurrent();
    m_initialized = true;
    resizeGL(width(), height());
    update();
}

void CellGLView3D::slotUpdate()
{
    m_geneData->computeRenderingData(*m_rendering_settings);
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
    m_image->clearData();
    m_legend->clearData();
    m_aligment.reset();
    update();
}

const QImage CellGLView3D::grabPixmapGL()
{
    const QPixmap res = grab(QRect(0,0,width(),height()));
    return res.toImage();
}

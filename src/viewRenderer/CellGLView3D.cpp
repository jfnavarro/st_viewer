#include "CellGLView3D.h"
#include "color/HeatMap.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QList>
#include <random>

static const QColor lasso_color = QColor(0,0,255,90);
static const int KEY_OFFSET = 20;
static const double DEFAULT_ZOOM_ADJUSTMENT = 10.0;

CellGLView3D::CellGLView3D(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_rendering_settings(nullptr)
    , m_num_points(0)
    , m_initialized(false)
    , m_legend_show(false)
    , m_image_show(true)
    , m_zoom(1.0)
    , m_rotateX(0.0)
    , m_rotateY(0.0)
    , m_rotateZ(0.0)
    , m_flip_factor(0.0)
    , m_rubberBanding(false)
    , m_lassoSelection(false)
    , m_selecting(false)
    , m_rubberband(nullptr)
    , m_geneData(nullptr)
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
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
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

    // init rubber band object
    m_rubberband.reset(new QRubberBand(QRubberBand::Rectangle, this));
    QPalette palette;
    palette.setBrush(QPalette::Highlight, QBrush(lasso_color));
    m_rubberband->setPalette(palette);

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
        const float fovY = 45.0f;
        const float front = 0.1f;
        const float back = 128.0f;
        const float ratio = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
        m_projection.perspective(fovY, ratio, front, back);
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
    QTransform image_trans = sceneTransformations();
    m_transform = QMatrix4x4(m_aligment * image_trans);
    qDebug() << m_transform;
    if (!m_geneData->is3D() && m_image_show) {
        // render image
        m_image->draw(m_projection * image_trans);
    }

    // render legend
    if (m_legend_show) {
        QPainter painter(this);
        painter.beginNativePainting();
        painter.setWorldMatrixEnabled(true);
        painter.setViewTransformEnabled(true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        m_legend->draw(*m_rendering_settings, painter);
        painter.endNativePainting();
    }

    // draw selection box/lasso
    if (!m_geneData->is3D() && m_selecting && m_lassoSelection && !m_lasso.isEmpty()) {
        QPainter painter(this);
        painter.beginNativePainting();
        painter.setBrush(lasso_color);
        painter.setPen(lasso_color);
        painter.drawPath(m_lasso.simplified());
        painter.endNativePainting();
    }

    const double alpha =
            m_rendering_settings->visual_mode == SettingsWidget::DynamicRange ?
                1.0 : m_rendering_settings->intensity;

    // Render gene data
    m_program.bind();
    m_program.setUniformValue(u_size, m_rendering_settings->size * 2);
    m_program.setUniformValue(u_alpha, static_cast<GLfloat>(alpha));
    m_program.setUniformValue(u_mvp_matrix, m_projection * m_transform);
    {
        m_vao.bind();
        glDrawArrays(GL_POINTS, 0, m_num_points);
        m_vao.release();
    }
    m_program.release();
}

void CellGLView3D::slotZoomIn()
{
    setZoomFactor(m_zoom * (100.0 + DEFAULT_ZOOM_ADJUSTMENT) / 100.0);
}

void CellGLView3D::slotZoomOut()
{
    setZoomFactor(m_zoom * (100.0 - DEFAULT_ZOOM_ADJUSTMENT) / 100.0);
}

void CellGLView3D::slotRotateX(const double angle)
{
    m_rotateX += angle;
    if (std::abs(m_rotateX) >= 360) {
        m_rotateX = 0;
    }
}

void CellGLView3D::slotRotateY(const double angle)
{
    m_rotateY += angle;
    if (std::abs(m_rotateY) >= 360) {
        m_rotateY = 0;
    }
}

void CellGLView3D::slotRotateZ(const double angle)
{
    m_rotateZ += angle;
    if (std::abs(m_rotateZ) >= 360) {
        m_rotateZ = 0;
    }
}

void CellGLView3D::slotFlip(const double angle)
{
    m_flip_factor += angle;
    if (std::abs(m_flip_factor) >= 360) {
        m_flip_factor = 0;
    }
}

void CellGLView3D::slotSelectionMode(const bool selection)
{
    m_selecting = selection;
}

void CellGLView3D::slotLassoSelectionMode(const bool lasso)
{
    m_lassoSelection = lasso;
}

void CellGLView3D::slotLegendVisible(const bool visible)
{
   m_legend_show = visible;
}

void CellGLView3D::slotImageVisible(const bool visible)
{
    m_image_show = visible;
}

void CellGLView3D::keyPressEvent(QKeyEvent *event)
{
    if (!m_initialized) {
        return;
    }

    const double shortest_side_length = qMin(width(), height());
    const double delta_panning_key = shortest_side_length / (KEY_OFFSET * m_zoom);

    QPointF pan_adjustment(0, 0);
    switch (event->key()) {
    case Qt::Key_Right:
        pan_adjustment = QPointF(-delta_panning_key, 0);
        break;
    case Qt::Key_Left:
        pan_adjustment = QPointF(delta_panning_key, 0);
        break;
    case Qt::Key_Up:
        pan_adjustment = QPointF(0, delta_panning_key);
        break;
    case Qt::Key_Down:
        pan_adjustment = QPointF(0, -delta_panning_key);
        break;
    default:
        break;
    }

    // move the view
    setSceneFocusCenterPoint(pan_adjustment + m_scene_focus_center_point);
    event->ignore();
    update();
}

void CellGLView3D::wheelEvent(QWheelEvent *event)
{
    if (!m_initialized) {
        return;
    }
    // computes zoom factor and update zoom
    const double zoomFactor = qPow(4.0 / 3.0, (event->delta() / 240.0));
    setZoomFactor(zoomFactor * m_zoom);
    event->ignore();
    update();
}

void CellGLView3D::mousePressEvent(QMouseEvent *event)
{
    if (!m_initialized) {
        return;
    }
    const bool is_left = event->button() == Qt::LeftButton;
    if (is_left && m_selecting && !m_lassoSelection && !m_geneData->is3D()) {
        // rubberbanding changes cursor to pointing hand
        setCursor(Qt::PointingHandCursor);
        m_rubberBanding = true;
        m_originRubberBand = event->pos();
        m_rubberband->setGeometry(QRect(m_originRubberBand, QSize()));
        m_rubberband->show();
    } else if (is_left && m_selecting && !m_geneData->is3D()) {
        m_lasso = QPainterPath();
        m_originLasso = event->pos();
        m_lasso.moveTo(m_originLasso);
        update();
    } else if (is_left) {
        m_panning = true;
        m_originPanning = event->globalPos(); // panning needs globalPos
        // panning changes cursor to closed hand
        setCursor(Qt::ClosedHandCursor);
    }
    m_lastPos = event->pos();
    event->ignore();
}

void CellGLView3D::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_initialized) {
        return;
    }
    const bool is_left = event->buttons() & Qt::LeftButton;
    // first check if we are in selection mode
    if (is_left && m_selecting && m_rubberBanding && !m_geneData->is3D()) {
        // update the rubber band
        m_rubberband->setGeometry(QRect(m_originRubberBand, event->pos()).normalized());
    } else if (is_left && m_selecting && !m_geneData->is3D()) {
        const QPoint new_point = event->pos();
        if ((new_point - m_originLasso).manhattanLength() > 5) {
            m_lasso.lineTo(new_point);
            m_originLasso = new_point;
            update();
        }
    } else if (is_left && m_panning) {
        // user is moving the view (panning)
        const QPoint point = event->globalPos(); // panning needs global pos
        const QPointF pan_adjustment = QPointF(point - m_originPanning) / m_zoom;
        setSceneFocusCenterPoint(pan_adjustment + m_scene_focus_center_point);
        m_originPanning = point;
    }
    m_lastPos = event->pos();
    event->ignore();
    update();
}

void CellGLView3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_initialized) {
        return;
    }
    const bool is_left = event->button() == Qt::LeftButton;
    if (is_left && m_selecting && m_rubberBanding && !m_geneData->is3D()) {
        unsetCursor();
        const QRectF rubberBandRect = m_rubberband->geometry();
        QPainterPath path;
        path.addRect(rubberBandRect);
        sendSelectionEvent(path, event);
        m_rubberband->hide();
        m_rubberBanding = false;
    } else if (is_left && m_selecting && !m_geneData->is3D()) {
        sendSelectionEvent(m_lasso, event);
        m_lasso = QPainterPath();
        update();
    } else if (is_left && m_panning) {
        unsetCursor();
        m_panning = false;
    }
    event->ignore();
}

void CellGLView3D::setZoomFactor(const double zoom)
{
    if (m_zoom != zoom) {
        m_zoom = zoom;
        setSceneFocusCenterPoint(m_scene_focus_center_point);
        update();
    }
}

void CellGLView3D::setSceneFocusCenterPoint(const QPointF &center_point)
{
    if (center_point != m_scene_focus_center_point) {
        m_scene_focus_center_point = center_point;
        update();
    }
}

void CellGLView3D::sendSelectionEvent(const QPainterPath &path, const QMouseEvent *event)
{
    // map selected area to node cordinate system
    QPainterPath transformed = (m_aligment * sceneTransformations()).inverted().map(path);

    // if selection area is not inside the bounding rect select empty rect
    if (!transformed.intersects(m_boundingRect)) {
        transformed = QPainterPath();
    }

    // Set the new selection area
    const SelectionEvent::SelectionMode mode
            = SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
    const SelectionEvent selectionEvent(transformed, mode);

    // send selection event to dataset
    m_geneData->selectSpots(selectionEvent);
    slotUpdate();
}

void CellGLView3D::attachSettings(SettingsWidget::Rendering *rendering_settings)
{
    m_rendering_settings = rendering_settings;
}

void CellGLView3D::attachDataset(const Dataset &dataset)
{
    m_geneData = dataset.data();
    m_boundingRect = QRect(dataset.xrange().x(),
                           dataset.yrange().x(),
                           dataset.xrange().y(),
                           dataset.yrange().y());
    m_boundingRectImage = dataset.image_bounds();
    m_aligment = dataset.imageAlignment();

    makeCurrent();

    // If the dataset is 3D we create textures from the image tiles
    // and load the image alignment
    if (!m_geneData->is3D()) {
        m_image->createTiles(dataset.image_tiles());
        m_scene_focus_center_point = m_boundingRectImage.center();
    } else {
        m_scene_focus_center_point = m_boundingRect.center();
    }

    // Create buffers
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
    m_zoom = 1.0;
    m_num_points = 0;
    m_initialized = false;
    m_rubberBanding = false;
    m_lassoSelection = false;
    m_selecting = false;
    m_image_show = true;
    m_legend_show = false;
    m_transform.setToIdentity();
    m_image->clearData();
    m_legend->clearData();
    m_aligment.reset();
    m_projection.setToIdentity();
    m_transform.setToIdentity();
    m_flip_factor = 0.0;
    m_rotateX = 0.0;
    m_rotateY = 0.0;
    m_rotateZ = 0.0;
    update();
}

const QTransform CellGLView3D::sceneTransformations() const
{
    // returns all the transformations applied to the scene from the user with respect to the viewport
    const QPointF image_center = m_geneData->is3D() ? m_boundingRect.center() :
                                                      m_boundingRectImage.center();
    const QPointF point = image_center + (image_center - m_scene_focus_center_point);
    const double viewport_w = static_cast<double>(width());
    const double viewport_h = static_cast<double>(height());
    QTransform transform;
    transform.translate(point.x(), point.y());
    transform.rotate(m_rotateX, Qt::XAxis);
    transform.rotate(m_rotateY, Qt::YAxis);
    transform.rotate(m_rotateZ, Qt::ZAxis);
    if (m_flip_factor == 180) {
        transform.scale(1.0, -1.0);
    }
    transform.scale(1 / m_zoom, 1 / m_zoom);
    transform.translate(-viewport_w / 2.0, -viewport_h / 2.0);
    return transform.inverted();
}

const QImage CellGLView3D::grabPixmapGL()
{
    const QPixmap res = grab(QRect(0,0,width(),height()));
    return res.toImage();
}

#include "CellGLView3D.h"
#include "color/HeatMap.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QList>
#include <random>
#include <algorithm>

//TODO replace by Qt weak color
constexpr QColor lasso_color = QColor(0,0,255,90);
constexpr int KEY_OFFSET = 2;
constexpr double DEFAULT_ZOOM_ADJUSTMENT = 10.0;

CellGLView3D::CellGLView3D(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_rendering_settings(nullptr)
    , m_pos_buffer(QOpenGLBuffer::VertexBuffer)
    , m_color_buffer(QOpenGLBuffer::VertexBuffer)
    , m_visible_buffer(QOpenGLBuffer::VertexBuffer)
    , m_selected_buffer(QOpenGLBuffer::VertexBuffer)
    , m_num_points(0)
    , m_initialized(false)
    , m_legend_show(false)
    , m_image_show(true)
    , m_zoom(1.0)
    , m_rotate_factor(0.0)
    , m_flip_factor(0.0)
    , m_elevation(30)
    , m_azimuth(45)
    , m_centerX(0.0)
    , m_centerY(0.0)
    , m_centerZ(0.0)
    , m_dist(10.0)
    , m_fov(60.0)
    , m_rubberBanding(false)
    , m_lassoSelection(false)
    , m_rubberband(nullptr)
    , m_dataset()
    , m_image(nullptr)
    , m_mesh(nullptr)
    , m_legend(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
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

void CellGLView3D::clearData()
{
    m_pos_buffer.destroy();
    m_color_buffer.destroy();
    m_visible_buffer.destroy();
    m_selected_buffer.destroy();
    m_vao.destroy();
    m_num_points = 0;
    m_initialized = false;
    m_rubberBanding = false;
    m_lassoSelection = false;
    m_image_show = true;
    m_legend_show = false;
    m_image->clearData();
    m_legend->clearData();
    m_mesh->clearData();
    m_zoom = 1.0;
    m_rotate_factor = 0.0;
    m_flip_factor = 0.0;
    m_elevation = 30.0;
    m_azimuth = 45.0;
    m_centerX = 0.0;
    m_centerY = 0.0;
    m_centerZ = 0.0;
    m_dist = 10.0;
    m_fov = 60.0;
    update();
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
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SPRITE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Compile Shaders
    m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/geneShader.vert");
    m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/geneShader.frag");

    // Cache Uniform Locations
    m_program.bind();
    u_mvp_matrix = m_program.uniformLocation("mvp_matrix");
    u_size = m_program.uniformLocation("size");
    u_alpha = m_program.uniformLocation("alpha");
    m_program.link();
    m_program.release();

    // init rubber band object
    m_rubberband.reset(new QRubberBand(QRubberBand::Rectangle, this));
    QPalette palette;
    palette.setBrush(QPalette::Highlight, QBrush(lasso_color));
    m_rubberband->setPalette(palette);

    // image texture graphical object
    m_image.reset(new ImageTextureGL());
    m_image->init();

    // mesh
    m_mesh.reset(new ImageMeshGL());
    m_mesh->init();

    // legend
    m_legend.reset(new HeatMapLegendGL());
}

void CellGLView3D::resizeGL(int width, int height)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
}

const QMatrix4x4 CellGLView3D::projectionMatrix3D() const
{
    const double dpr = devicePixelRatioF();
    const double x0 = 0;
    const double y0 = 0;
    const double w = static_cast<double>(width()) * dpr;
    const double h = static_cast<double>(height()) * dpr;
    const double nearClip = m_dist * 0.001;
    const double farClip = m_dist * 1000;
    const double r = nearClip * std::tan(m_fov * 0.5 * STMath::PI / 180);
    const double t = r * h / w;
    const double left  = r * ((x0 - x0) * (2.0 / w) - 1);
    const double right = r * ((x0 + w - x0) * (2.0 / w) - 1);
    const double bottom = t * ((y0 - y0) * (2.0 / h) - 1);
    const double top = t * ((y0 + h - y0) * (2.0 / h) - 1);
    QMatrix4x4 tr;
    tr.frustum(left, right, bottom, top, nearClip, farClip);
    return tr;
}

const QMatrix4x4 CellGLView3D::projectionMatrix2D() const
{
    const double dpr = devicePixelRatioF();
    const double x0 = 0;
    const double y0 = 0;
    const double w = static_cast<double>(width()) * dpr;
    const double h = static_cast<double>(height()) * dpr;
    QMatrix4x4 tr;
    tr.ortho(QRect(x0, y0, w, h));
    return tr;
}

const QMatrix4x4 CellGLView3D::viewMatrix3D() const
{
    QMatrix4x4 tr;
    tr.translate(0.0, 0.0, -m_dist);
    tr.rotate(m_elevation - 90, 1, 0, 0);
    tr.rotate(m_azimuth + 90, 0, 0, -1);
    tr.translate(-m_centerX, -m_centerY, -m_centerZ);
    return tr;
}

const QMatrix4x4 CellGLView3D::viewMatrix2D() const
{
    QTransform tr;
    const double dpr = devicePixelRatioF();
    const double w = static_cast<double>(width()) * dpr;
    const double h = static_cast<double>(height()) * dpr;
    const QPointF focus(m_centerX, m_centerY);
    const QRect imageRect = m_dataset.image_bounds();
    const QPointF point = imageRect.center() + (imageRect.center() - focus);
    tr.translate(point.x(), point.y());
    if (m_rotate_factor != 0) {
        tr.rotate(m_rotate_factor);
    }
    if (m_flip_factor == 180) {
        tr.scale(1.0, -1.0);
    }
    tr.scale(1 / m_zoom, 1 / m_zoom);
    tr.translate(-w / 2.0, -h / 2.0);
    return QMatrix4x4(tr.inverted());
}

const QVector3D CellGLView3D::cameraPosition()
{
    const double elev = m_elevation * STMath::PI / 180;
    const double azim = m_azimuth * STMath::PI / 180;
    return QVector3D(
                m_centerX + m_dist * std::cos(elev) * std::cos(azim),
                m_centerY + m_dist * std::cos(elev) * std::sin(azim),
                m_centerZ + m_dist * std::sin(elev)
                );
}

void CellGLView3D::setPan(const double dx, const double dy, const double dz, const bool view = false)
{
    if (view) {
        const double elev = STMath::deg3rad(m_elevation);
        const double azim = STMath::deg3rad(m_azimuth);
        const double fov = STMath::deg3rad(m_fov);
        const double dist = (QVector3D(m_centerX, m_centerY, m_centerZ) - cameraPosition()).length();
        const double fov_factor = std::tan(fov / 2) * 2;
        const double scale_factor = dist * fov_factor / static_cast<double>(width());
        m_centerZ += scale_factor * std::cos(elev) * dy;
        m_centerX += scale_factor * (std::sin(azim) * dx - std::sin(elev) * std::cos(azim) * dy);
        m_centerY -= scale_factor * (std::cos(azim) * dx + std::sin(elev) * std::sin(azim) * dy);
    } else {
        m_centerX += dx;
        m_centerY += dy;
        m_centerZ += dz;
    }
}

void CellGLView3D::setRotation(const double azim, const double elevation)
{
    m_azimuth += azim;
    m_elevation += elevation;
    if (m_elevation > 90) {
        m_elevation = 90;
    }
    if (m_elevation < -90) {
        m_elevation = -90;
    }
}

void CellGLView3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_initialized) {
        return;
    }

    const bool is3D = m_dataset.is3D();

    // model view matrices
    const QTransform aligment = m_dataset.alignmentMatrix();
    const QMatrix4x4 view = is3D ? viewMatrix3D() : viewMatrix2D();
    const QMatrix4x4 projection = is3D ? projectionMatrix3D() : projectionMatrix2D();
    const QMatrix4x4 mvp = is3D ? projection * view : projection * view * aligment;

    // render image
    if (!is3D && m_image_show) {
        m_image->draw(projection * view);
    }

    // render mesh
    if (is3D && m_image_show) {
        m_mesh->draw(projection * view);
    }

    // alpha value
    const double alpha =
            m_rendering_settings->visual_mode == SettingsWidget::DynamicRange ?
                -1.0 : m_rendering_settings->intensity;

    // make size proportional to the zoom
    const int size = is3D ? m_rendering_settings->size * 2:
                            std::clamp(static_cast<int>(m_rendering_settings->size * 5 * m_zoom), 5, 25);

    // Render gene data
    m_program.bind();
    m_program.setUniformValue(u_size, size);
    m_program.setUniformValue(u_alpha, static_cast<GLfloat>(alpha));
    m_program.setUniformValue(u_mvp_matrix, mvp);
    m_vao.bind();
    glDrawArrays(GL_POINTS, 0, m_num_points);
    m_vao.release();
    m_program.release();

    QPainter painter(this);
    //painter.setBackgroundMode(Qt::TransparentMode);
    //painter.setBackground(Qt::transparent);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // render legend
    if (m_legend_show) {
        //TODO no need to always update the legend if the color mode is not changed
        m_legend->update();
        m_legend->draw(*m_rendering_settings, painter);
    }

    // render selection box/lasso
    if (!is3D && m_lassoSelection && !m_lasso.isEmpty()) {
        painter.setBrush(lasso_color);
        painter.setPen(lasso_color);
        painter.drawPath(m_lasso.simplified());
    }
}

void CellGLView3D::slotZoomIn()
{
    m_zoom *= (100.0 + DEFAULT_ZOOM_ADJUSTMENT) / 100.0;
    update();
}

void CellGLView3D::slotZoomOut()
{
    m_zoom *= (100.0 - DEFAULT_ZOOM_ADJUSTMENT) / 100.0;
    update();
}

void CellGLView3D::slotRotate(const double angle)
{
    m_rotate_factor += angle;
    if (std::fabs(m_rotate_factor) >= 360) {
        m_rotate_factor = 0;
    }
    update();
}

void CellGLView3D::slotFlip(const double angle)
{
    m_flip_factor += angle;
    if (std::fabs(m_flip_factor) >= 360) {
        m_flip_factor = 0;
    }
    update();
}

void CellGLView3D::slotRubberBandSelectionMode(const bool rubberband)
{
    m_rubberBanding = rubberband;
}

void CellGLView3D::slotLassoSelectionMode(const bool lasso)
{
    m_lassoSelection = lasso;
}

void CellGLView3D::slotLegendVisible(const bool visible)
{
    m_legend_show = visible;
    update();
}

void CellGLView3D::slotImageVisible(const bool visible)
{
    m_image_show = visible;
    update();
}

void CellGLView3D::keyPressEvent(QKeyEvent *event)
{
    if (!m_initialized) {
        return;
    }
    const bool is3D = m_dataset.is3D();
    switch (event->key()) {
    case Qt::Key_Right:
        if (is3D) {
            setRotation(-KEY_OFFSET, 0);
        } else {
            m_centerX -= KEY_OFFSET;
        }
        break;
    case Qt::Key_Left:
        if (is3D) {
            setRotation(KEY_OFFSET, 0);
        } else {
            m_centerX += KEY_OFFSET;
        }
        break;
    case Qt::Key_Up:
        if (is3D) {
            setRotation(0, -KEY_OFFSET);
        } else {
            m_centerY += KEY_OFFSET;
        }
        break;
    case Qt::Key_Down:
        if (is3D) {
            setRotation(0, KEY_OFFSET);
        } else {
            m_centerY -= KEY_OFFSET;
        }
        break;
    default:
        break;
    }
    event->ignore();
    update();
}

void CellGLView3D::wheelEvent(QWheelEvent *event)
{
    if (!m_initialized) {
        return;
    }

    //TODO we may want to also use the delta Y
    const double zoomFactor = std::pow(4.0 / 3.0, (event->angleDelta().x() / 240.0));
    m_zoom *= zoomFactor;

    if (event->modifiers() & Qt::ControlModifier) {
        m_fov *= zoomFactor;
    } else {
        m_dist *= zoomFactor;
    }

    event->ignore();
    update();
}

void CellGLView3D::mousePressEvent(QMouseEvent *event)
{
    if (!m_initialized) {
        return;
    }

    const bool is_left = event->button() == Qt::LeftButton;
    const bool is3D = m_dataset.is3D();

    if (is_left && m_rubberBanding && !is3D) {
        // rubberbanding changes cursor to pointing hand
        setCursor(Qt::PointingHandCursor);
        m_originSelection = event->pos();
        m_rubberband->setGeometry(QRect(m_originSelection, QSize()));
        m_rubberband->show();
    } else if (is_left && m_lassoSelection && !is3D) {
        m_lasso = QPainterPath();
        m_originSelection = event->pos();
        m_lasso.moveTo(m_originSelection);
    } else {
        m_pos = event->globalPosition(); // panning needs globalPos
        // panning changes cursor to closed hand
        setCursor(Qt::ClosedHandCursor);
    }

    event->ignore();
    update();
}

void CellGLView3D::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_initialized) {
        return;
    }

    const bool is_left = event->buttons() & Qt::LeftButton;
    const bool is3D = m_dataset.is3D();
    const QPointF diff = event->globalPosition() - m_pos;
    m_pos = event->globalPosition();

    // first check if we are in selection mode
    if (is_left && m_rubberBanding && !is3D) {
        // update the rubber band
        m_rubberband->setGeometry(QRect(m_originSelection, event->pos()).normalized());
    } else if (is_left && m_lassoSelection && !is3D) {
        const QPoint new_point = event->pos();
        if ((new_point - m_originSelection).manhattanLength() > 5) {
            m_lasso.lineTo(new_point);
            m_originSelection = new_point;
        }
    } else if (is_left) {
        setPan(diff.x(), diff.y(), 0, is3D);
    } else {
        setRotation(-diff.x(), diff.y());
    }

    event->ignore();
    update();
}

void CellGLView3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_initialized) {
        return;
    }

    unsetCursor();

    const bool is_left = event->button() == Qt::LeftButton;
    const bool is3D = m_dataset.is3D();

    if (is_left && m_rubberBanding && !is3D) {
        const QRectF rubberBandRect = m_rubberband->geometry();
        QPainterPath path;
        path.addRect(rubberBandRect);
        sendSelectionEvent(path, event);
        m_rubberband->hide();
    } else if (is_left && m_lassoSelection && !is3D) {
        sendSelectionEvent(m_lasso, event);
        m_lasso = QPainterPath();
    }

    event->ignore();
    update();
}

void CellGLView3D::sendSelectionEvent(const QPainterPath &path, const QMouseEvent *event)
{
    // map selected area to node cordinate system
    const QTransform alignment = m_dataset.alignmentMatrix();
    QPainterPath transformed = QTransform(viewMatrix2D().toTransform() * alignment).inverted().map(path);

    // if selection area is not inside the bounding rect select empty rect
    // TODO this is not working well when the image is scaled
    //if (!transformed.intersects(m_dataset.image_bounds())) {
    //    transformed = QPainterPath();
    //}

    // Set the new selection area
    const SelectionEvent::SelectionMode mode
            = SelectionEvent::modeFromKeyboardModifiers(event->modifiers());
    const SelectionEvent selectionEvent(transformed, mode);

    // send selection event to dataset
    m_dataset.data()->selectSpots(selectionEvent);
    slotUpdate();
}

void CellGLView3D::attachSettings(SettingsWidget::Rendering *rendering_settings)
{
    m_rendering_settings = rendering_settings;
}

void CellGLView3D::attachDataset(const Dataset &dataset)
{
    m_dataset = dataset;

    makeCurrent();

    // If the dataset is not 3D we create textures from the image tiles
    // and load the image alignment
    if (!dataset.is3D() && !dataset.imageFile().isNull() && !dataset.imageFile().isEmpty()) {
        m_image->createTiles(dataset.image_tiles());
        m_centerX = dataset.image_bounds().center().x();
        m_centerY = dataset.image_bounds().center().y();
    }

    // Load the 3D mesh if applies
    if (dataset.is3D() && !dataset.meshFile().isNull() && !dataset.meshFile().isEmpty()) {
        m_mesh->loadMesh(dataset.meshFile());
    }

    // Create buffers
    const auto &indexes = dataset.data()->renderingCoords();
    const auto &colors = dataset.data()->renderingColors();
    const auto &visibles = dataset.data()->renderingVisible();
    const auto &selecteds = dataset.data()->renderingSelected();
    m_num_points = indexes.size();

    // Create VAO
    m_vao.create();
    m_vao.bind();
    m_program.bind();

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
    m_color_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_color_buffer.allocate(colors.constData(), colors.size() * sizeof(QVector4D));
    m_program.enableAttributeArray(1);
    m_program.setAttributeBuffer(1, GL_FLOAT, 0, 4, 0);

    // Create Buffer (Selected)
    m_selected_buffer.create();
    m_selected_buffer.bind();
    m_selected_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_selected_buffer.allocate(selecteds.constData(), selecteds.size() * sizeof(int));
    m_program.enableAttributeArray(2);
    m_program.setAttributeBuffer(2, GL_INT, 0, 1, 0);

    // Create Buffer (Visible)
    m_visible_buffer.create();
    m_visible_buffer.bind();
    m_visible_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_visible_buffer.allocate(visibles.constData(), visibles.size() * sizeof(int));
    m_program.enableAttributeArray(3);
    m_program.setAttributeBuffer(3, GL_INT, 0, 1, 0);

    // Release (unbind) all
    m_pos_buffer.release();
    m_color_buffer.release();
    m_selected_buffer.release();
    m_visible_buffer.release();
    m_program.release();
    m_vao.release();

    doneCurrent();
    m_initialized = true;
    update();
}

void CellGLView3D::slotUpdate()
{
    m_dataset.data()->computeRenderingData(*m_rendering_settings);
    const auto &colors = m_dataset.data()->renderingColors();
    const auto &visibles = m_dataset.data()->renderingVisible();
    const auto &selecteds = m_dataset.data()->renderingSelected();

    {
        makeCurrent();
        m_vao.bind();

        // Update Buffer (Color)
        m_color_buffer.bind();
        m_color_buffer.write(0, colors.constData(), colors.size() * sizeof(QVector4D));
        m_color_buffer.release();

        // Update Buffer (Selected)
        m_selected_buffer.bind();
        m_selected_buffer.write(0, selecteds.constData(), selecteds.size() * sizeof(int));

        // Update Buffer (Visible)
        m_visible_buffer.bind();
        m_visible_buffer.write(0, visibles.constData(), visibles.size() * sizeof(int));
        m_visible_buffer.release();

        m_vao.release();
        doneCurrent();
    }

    update();
}

const QImage CellGLView3D::grabPixmapGL()
{
    //const QPixmap res = grab(QRect(0,0,width(),height()));
    //return res.toImage();
    return grabFramebuffer();
}

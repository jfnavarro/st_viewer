#include "OpenGLTestWindow.h"
#include "viewOpenGL/AssertOpenGL.h"

#include <stdexcept>
#include <QTest>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>

// This file is intended to provide a 'reference' OpenGL 3.3 sample. It makes explicit
// many steps which would work with defaults, such as the fragment location binding and the
// vertex array binding points. By making these steps explicit it helps understand how the
// system is working and can help design solutions to more complex problems.
//
// Note that there is no matching glDeleteVertexArrays for glGenVertexArrays, nor any
// glDeleteBuffers call to match glGenBuffers. This is because we cannot be certain we have
// an OpenGL context available to use in the dtor of the object.

namespace
{

static const char* vertexShaderSource = "#version 330                      \n"
                                        "in vec4 vertex;                   \n"
                                        "in vec4 color;                    \n"
                                        "smooth out vec4 col;              \n"
                                        "uniform mat4 matrix;              \n"
                                        "void main() {                     \n"
                                        "   col = color;                   \n"
                                        "   gl_Position = matrix * vertex; \n"
                                        "}\n";

static const char* fragmentShaderSource = "#version 330                    \n"
                                          "in vec4 col;                    \n"
                                          "out vec4 fragColor;             \n"
                                          "void main() {                   \n"
                                          "   fragColor = col;             \n"
                                          "}\n";

// Vertex Array Index Binding Points.
static const GLuint VA_POINTS = 0;
static const GLuint VA_COLORS = 1;

// Returns a new Vertex Buffer Object (VBO) containing the given data. Returns 0 if the
// the VBO could not be constructed.
static GLuint makeStaticVBO(const GLfloat* const data, const GLsizei dataBytesCount)
{
    GLuint vbo = 0;

    auto context = QOpenGLContext::currentContext();

    if (context) {
        auto funcs = context->functions();

        funcs->glGenBuffers(1, &vbo);
        funcs->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        funcs->glBufferData(GL_ARRAY_BUFFER, dataBytesCount, data, GL_STATIC_DRAW);

        Q_ASSERT(funcs->glIsBuffer(vbo));
    }

    Q_ASSERT(vbo != 0);

    return vbo;
}

static GLuint makeTriangleVerticesVBO()
{
    const GLfloat vertices[] = {0.0f, 0.707f, -0.5f, -0.5f, 0.5f, -0.5f};
    return makeStaticVBO(vertices, sizeof(vertices));
}

static GLuint makeTriangleColorsVBO()
{
    const GLfloat colors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    return makeStaticVBO(colors, sizeof(colors));
}
}

bool OpenGLTestWindow::createAndShowWindow(const int timeoutMs,
                                           std::function<void(void)> renderFunc)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setOption(QSurfaceFormat::DebugContext, true);

    OpenGLTestWindow window(format, renderFunc, nullptr);

    window.resize(640, 480);
    window.show();
    window.setAnimating(true);

    QElapsedTimer timer;
    timer.start();

    bool wasExposed = false;

    int remainingMs = timeoutMs - int(timer.elapsed());

    // Pump the window for the specified duration.
    while (remainingMs > 0) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, remainingMs);
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        QTest::qSleep(10);

        remainingMs = timeoutMs - int(timer.elapsed());
        wasExposed = wasExposed || window.isExposed();
    }

    return wasExposed;
}

OpenGLTestWindow::OpenGLTestWindow(const QSurfaceFormat& format,
                                   std::function<void(void)> renderFunc,
                                   QWindow* parent)
    : QWindow(parent)
    , m_update_pending(false)
    , m_animating(false)
    , m_renderFunc(renderFunc)
    , m_glfuncs()
    , m_context(nullptr)
    , m_program(nullptr)
    , m_verticesVBO(0)
    , m_coloursVBO(0)
    , m_vao(0)
    , m_frame()
{
    setFormat(format);
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLTestWindow::~OpenGLTestWindow()
{
}

bool OpenGLTestWindow::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLTestWindow::exposeEvent(QExposeEvent* event)
{
    Q_UNUSED(event);

    if (isExposed()) {
        renderNow();
    }
}

void OpenGLTestWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating) {
        renderLater();
    }
}

void OpenGLTestWindow::initialize()
{
    ASSERT_OPENGL_OK;

    if (m_glfuncs.initializeOpenGLFunctions()) {
        qDebug() << "GL_RENDERER: "
                 << reinterpret_cast<const char*>(m_glfuncs.glGetString(GL_RENDERER));
        qDebug() << "GL_VERSION : "
                 << reinterpret_cast<const char*>(m_glfuncs.glGetString(GL_VERSION));

    } else {
        throw std::runtime_error("Unable to initialize OpenGL functions.");
    }

    if (!m_program) {
        m_program.reset(new QOpenGLShaderProgram(this));

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

        m_program->bindAttributeLocation("vertex", VA_POINTS);
        m_program->bindAttributeLocation("color", VA_COLORS);

        m_glfuncs.glBindFragDataLocation(m_program->programId(), 0, "fragColor");

        m_program->link();
    }

    if (0 == m_verticesVBO) {
        m_verticesVBO = makeTriangleVerticesVBO();
    }

    if (0 == m_coloursVBO) {
        m_coloursVBO = makeTriangleColorsVBO();
    }

    if (0 == m_vao) {
        m_glfuncs.glGenVertexArrays(1, &m_vao);

        m_glfuncs.glBindVertexArray(m_vao);

        m_glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
        m_glfuncs.glEnableVertexAttribArray(VA_POINTS);
        m_glfuncs.glVertexAttribPointer(VA_POINTS, 2, GL_FLOAT, GL_FALSE, 0, 0);

        m_glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_coloursVBO);
        m_glfuncs.glEnableVertexAttribArray(VA_COLORS);
        m_glfuncs.glVertexAttribPointer(VA_COLORS, 3, GL_FLOAT, GL_FALSE, 0, 0);

        m_glfuncs.glBindVertexArray(0);
    }

    ASSERT_OPENGL_OK;
}

void OpenGLTestWindow::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

void OpenGLTestWindow::renderNow()
{
    if (!isExposed()) {
        return;
    }

    bool needsInitialize = false;

    if (!m_context) {
        m_context.reset(new QOpenGLContext(this));
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating) {
        renderLater();
    }
}

void OpenGLTestWindow::render(QPainter* painter)
{
    Q_UNUSED(painter);
}

void OpenGLTestWindow::render()
{
    ASSERT_OPENGL_OK;

    const qreal retinaScale = devicePixelRatio();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_glfuncs.glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    m_glfuncs.glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();
    m_program->setUniformValue("matrix", matrix);

    m_glfuncs.glBindVertexArray(m_vao);
    m_glfuncs.glDrawArrays(GL_TRIANGLES, 0, 3);
    m_glfuncs.glBindVertexArray(0);

    m_program->release();

    ASSERT_OPENGL_OK;

    if (m_renderFunc) {
        m_renderFunc();
        ASSERT_OPENGL_OK;
    }

    ++m_frame;
}

void OpenGLTestWindow::doNothing()
{
}

#include "OpenGLTestWindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include <QtGui/QPainter>

namespace
{

static const char *vertexShaderSource = "attribute highp vec4 posAttr;\n"
                                        "attribute lowp vec4 colAttr;\n"
                                        "varying lowp vec4 col;\n"
                                        "uniform highp mat4 matrix;\n"
                                        "void main() {\n"
                                        "   col = colAttr;\n"
                                        "   gl_Position = matrix * posAttr;\n"
                                        "}\n";

static const char *fragmentShaderSource = "varying lowp vec4 col;\n"
                                          "void main() {\n"
                                          "   gl_FragColor = col;\n"
                                          "}\n";
}

int OpenGLTestWindow::run(const int maxRenderCount, std::function<void(void)> renderFunc)
{
    int argc = 1;
    char fakeAppName[] = "'Fake OpenGLTestWindow App Name'";
    char *fakeAppArgs[] = {&fakeAppName[0]};

    QGuiApplication app(argc, &fakeAppArgs[0]);

    QSurfaceFormat format;
    format.setSamples(16);

    OpenGLTestWindow window(maxRenderCount, renderFunc, nullptr);
    window.setFormat(format);
    window.resize(640, 480);
    window.show();
    window.setAnimating(true);

    return app.exec();
}

OpenGLTestWindow::OpenGLTestWindow(const int renderCount,
                                   std::function<void(void)> renderFunc,
                                   QWindow *parent)
    : QWindow(parent)
    , m_update_pending(false)
    , m_animating(false)
    , m_renderFunc(renderFunc)
    , m_context(nullptr)
    , m_device(nullptr)
    , m_program(nullptr)
    , m_posAttr()
    , m_colAttr()
    , m_matrixUniform()
    , m_frame()
    , m_renderCountdown(renderCount)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLTestWindow::~OpenGLTestWindow()
{
    delete m_device;
}

bool OpenGLTestWindow::event(QEvent *event)
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

void OpenGLTestWindow::exposeEvent(QExposeEvent *event)
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

GLuint OpenGLTestWindow::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void OpenGLTestWindow::initialize()
{
    if (nullptr == m_program) {
        m_program = new QOpenGLShaderProgram(this);
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
        m_program->link();
    }

    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
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
        m_context = new QOpenGLContext(this);
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

void OpenGLTestWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

void OpenGLTestWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    GLfloat vertices[] = {0.0f, 0.707f, -0.5f, -0.5f, 0.5f, -0.5f};

    GLfloat colors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();

    if (m_renderFunc) {
        m_renderFunc();
    }

    ++m_frame;

    advanceRenderCounter();
}

void OpenGLTestWindow::advanceRenderCounter()
{
    if (m_renderCountdown > 0) {
        --m_renderCountdown;

        if (m_renderCountdown == 0) {
            close();
        }
    }
}

void OpenGLTestWindow::doNothing()
{
}

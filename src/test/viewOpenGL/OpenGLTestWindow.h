#ifndef TEST_OPENGLTESTWINDOW_H
#define TEST_OPENGLTESTWINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <functional>

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
class QOpenGLShaderProgram;

// A window used to test OpenGL with. Based on the code from here:
// http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
// By default this window shows a triangle rotating about its central axis. The user can limit
// the number of frames the triangle is displayed for, and can also pass in a functor object
// which will be called during the render call.
class OpenGLTestWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    // A self contained call that will create a Qt application and a OpenGLTestWindow with the
    // params passed in. This call is intended to be used to simplify testing.
    // Returns true if the window was successfully created and shown.
    // The window will be shown for timeoutMs milliseconds only.
    static bool createAndShowWindow(const int timeoutMs,
                                    std::function<void(void)> renderFunc
                                    = &OpenGLTestWindow::doNothing);

    // renderFunc is a functor that will be called inside the render loop, after the rotating
    // triangle has been setup and before advanceRenderCounter is called.
    explicit OpenGLTestWindow(std::function<void(void)> renderFunc = &OpenGLTestWindow::doNothing,
                              QWindow* parent = nullptr);

    virtual ~OpenGLTestWindow();

    virtual void render(QPainter* painter);

    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent* event) Q_DECL_OVERRIDE;

private:
    GLuint loadShader(GLenum type, const char* source);

    // Default (post)render function, does nothing.
    static void doNothing();

private:
    bool m_update_pending;
    bool m_animating;
    std::function<void(void)> m_renderFunc;
    QOpenGLContext* m_context;
    QOpenGLPaintDevice* m_device;
    QOpenGLShaderProgram* m_program;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    int m_frame;
};

#endif

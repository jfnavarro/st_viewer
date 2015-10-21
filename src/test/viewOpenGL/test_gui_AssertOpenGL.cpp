/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include "test_gui_AssertOpenGL.h"
#include "OpenGLTestWindow.h"
#include "viewOpenGL/AssertOpenGL.h"

#include <QtTest/QTest>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

namespace unit
{

OpenGLAssertTest::OpenGLAssertTest(QObject* parent)
    : QObject(parent)
{
}

void OpenGLAssertTest::initTestCase()
{
}

void OpenGLAssertTest::cleanupTestCase()
{
}

void OpenGLAssertTest::test_checkOpenGLNoErrorWithNoErrors()
{
    const int visibleDurationMs = 1200;

    // We expect that the default OpenGL app has no OpenGL errors.
    auto test_OpenGL_has_no_error
        = [=](void) { QVERIFY2(checkOpenGLNoError(), "checkOpenGLError returned false."); };

    const bool shown
        = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, test_OpenGL_has_no_error);

    QVERIFY2(shown, "Window was not visible.");
}

void OpenGLAssertTest::test_checkOpenGLNoErrorDetectsError()
{
    const int visibleDurationMs = 1200;

    // Create an error and check that we detect it.
    auto test_OpenGL_error_detected = [=](void) {
        QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
        QVERIFY2(funcs, "No OpenGL context found.");

        // glEnable does not accept GL_LINE as an argument: causes GL_INVALID_ENUM.
        funcs->glEnable(GL_LINE);
        QVERIFY2(false == checkOpenGLNoError(), "checkOpenGLError incorrectly returned true.");
    };

    const bool shown
        = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, test_OpenGL_error_detected);

    QVERIFY2(shown, "Window was not visible.");
}

} // namespace unit

QTEST_MAIN(unit::OpenGLAssertTest)
#include "test_gui_AssertOpenGL.moc"

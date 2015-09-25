/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include "test_AssertOpenGL.h"
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

void OpenGLAssertTest::test_checkOpenGLErrorWithNoErrors()
{
    const int framesVisible = 4;

    // We expect that the default OpenGL app has no OpenGL errors.
    auto test_OpenGL_has_no_error
        = [=](void) { QVERIFY2(checkOpenGLError(), "checkOpenGLError returned false."); };

    OpenGLTestWindow::run(framesVisible, test_OpenGL_has_no_error);
}

void OpenGLAssertTest::test_checkOpenGLErrorDetectsError()
{
    const int framesVisible = 4;

    // Create an error and check that we detect it.
    auto test_OpenGL_error_detected = [=](void) {
        QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
        QVERIFY2(funcs, "No OpenGL context found.");

        // glEnable does not accept GL_LINE as an argument: causes GL_INVALID_ENUM.
        funcs->glEnable(GL_LINE);
        QVERIFY2(false == checkOpenGLError(), "checkOpenGLError incorrectly returned true.");
    };

    OpenGLTestWindow::run(framesVisible, test_OpenGL_error_detected);
}
}

/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include "test_gui_ColoredLines.h"
#include "OpenGLTestWindow.h"
#include "viewOpenGL/AssertOpenGL.h"
#include "viewOpenGL/ColoredLines.h"
#include "viewOpenGL/Renderer.h"

#include <QtTest/QTest>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <stdexcept>

namespace unit
{

ColoredLinesTest::ColoredLinesTest(QObject* parent)
    : QObject(parent)
{
}

void ColoredLinesTest::initTestCase()
{
}

void ColoredLinesTest::cleanupTestCase()
{
}

void ColoredLinesTest::test_equality()
{
    ColoredLines lines0;
    ColoredLines lines1;
    QVERIFY(lines0 == lines1);
    QVERIFY(!(lines0 != lines1));

    // Add same lines
    lines0.addLine(QLineF(1.0f, -1.0f, 2.0f, 2.0f), Qt::red);
    lines1.addLine(QLineF(1.0f, -1.0f, 2.0f, 2.0f), Qt::red);
    QVERIFY(lines0 == lines1);
    QVERIFY(!(lines0 != lines1));

    // Add lines differing by color.
    lines0.addLine(QLineF(2.0f, -3.0f, 4.0f, 5.0f), Qt::blue);
    lines1.addLine(QLineF(2.0f, -3.0f, 4.0f, 5.0f), Qt::green);
    QVERIFY(lines0 != lines1);
    QVERIFY(!(lines0 == lines1));

    lines0.clear();
    lines1.clear();
    QVERIFY(lines0 == lines1);
    QVERIFY(!(lines0 != lines1));

    // Add lines differing by location.
    lines0.addLine(QLineF(2.0f, -3.0f, 4.0f, 5.0f), Qt::yellow);
    lines1.addLine(QLineF(6.0f, -7.0f, 8.0f, 9.0f), Qt::yellow);
    QVERIFY(lines0 != lines1);
    QVERIFY(!(lines0 == lines1));
}

void ColoredLinesTest::test_redVerticalsGreenHorizontals()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeLines = [&openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        ColoredLines lines;

        for (float i = -1.0f; i <= 1.0f; i += 0.05f) {
            // Red Vertical lines:
            lines.addLine(QLineF(i, -1.0f, i, 1.0f), Qt::red);
            // Green Horizontal lines:
            lines.addLine(QLineF(-1.0f, i, 1.0f, i), Qt::green);
        }

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.draw(matrix, lines);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeLines);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

void ColoredLinesTest::test_alphaBlendedLines()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeLines = [&openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        ColoredLines lines;

        const QColor verticalColor(0, 0, 255, 128);
        const QColor horizontalColor(0, 255, 0, 64);

        // Vertical lines:
        for (float x = -1.0f; x <= 1.0f; x += 0.05f) {
            lines.addLine(QLineF(x, -1.0f, x, 1.0f), verticalColor);
        }

        // Horizontal lines:
        for (float y = -1.0f; y <= 1.0f; y += 0.05f) {
            lines.addLine(QLineF(-1.0f, y, 1.0f, y), horizontalColor);
        }

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.draw(matrix, lines);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeLines);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

void ColoredLinesTest::test_thickLines()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeLines = [&openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        ColoredLines lines;

        const QColor verticalColor(255, 255, 255, 255);

        // Vertical lines:
        for (float x = -1.0f; x <= 1.0f; x += 0.05f) {
            lines.addLine(QLineF(x, -1.0f, x, 1.0f), verticalColor);
        }

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.draw(matrix, lines);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeLines);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

void ColoredLinesTest::test_exceptionAccessingNonExistantLine()
{
    bool caughtCorrectException = false;

    try {
        ColoredLines lines;
        QLineF a = lines.line(0);
        Q_UNUSED(a);
    } catch (const std::out_of_range&) {
        caughtCorrectException = true;
    } catch (...) {
    }

    QVERIFY2(caughtCorrectException, "Incorrect or no exception was thrown by ColoredLines::line.");
}

void ColoredLinesTest::test_accessingLines()
{
    try {
        QLineF line0(0.0f, 1.0f, 2.0f, 3.0f);
        QLineF line1(1.0f, 2.0f, 3.0f, 4.0f);
        QLineF line2a(2.0f, 3.0f, 4.0f, 5.0f);
        QLineF line2b(3.0f, 4.0f, 5.0f, 6.0f);

        ColoredLines lines;
        QVERIFY(lines.lineCount() == 0);

        lines.addLine(line0, Qt::red);
        QVERIFY(lines.lineCount() == 1);
        QVERIFY(lines.line(0) == line0);
        QVERIFY(lines.lineColor(0) == Qt::red);

        lines.addLine(line1, Qt::blue);
        QVERIFY(lines.lineCount() == 2);
        QVERIFY(lines.line(0) == line0);
        QVERIFY(lines.lineColor(0) == Qt::red);
        QVERIFY(lines.line(1) == line1);
        QVERIFY(lines.lineColor(1) == Qt::blue);

        lines.addLine(line2a, Qt::green);
        QVERIFY(lines.lineCount() == 3);
        QVERIFY(lines.line(0) == line0);
        QVERIFY(lines.lineColor(0) == Qt::red);
        QVERIFY(lines.line(1) == line1);
        QVERIFY(lines.lineColor(1) == Qt::blue);
        QVERIFY(lines.line(2) == line2a);
        QVERIFY(lines.lineColor(2) == Qt::green);

        lines.setLine(2, line2b, Qt::yellow);
        QVERIFY(lines.lineCount() == 3);
        QVERIFY(lines.line(2) == line2b);
        QVERIFY(lines.lineColor(2) == Qt::yellow);

        lines.clear();
        QVERIFY(lines.lineCount() == 0);
    } catch (...) {
        QFAIL("Exception thrown when accessing line.");
    }
}

} // namespace unit

QTEST_MAIN(unit::ColoredLinesTest)
#include "test_gui_ColoredLines.moc"

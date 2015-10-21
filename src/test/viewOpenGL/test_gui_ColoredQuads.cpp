/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include "test_gui_ColoredQuads.h"
#include "OpenGLTestWindow.h"
#include "viewOpenGL/ColoredLines.h"
#include "viewOpenGL/ColoredQuads.h"
#include "viewOpenGL/AssertOpenGL.h"
#include "viewOpenGL/ColoredQuads.h"
#include "viewOpenGL/TexturedQuads.h"
#include "viewOpenGL/Renderer.h"

#include <QImage>
#include <QPainter>
#include <QMatrix4x4>
#include <QtTest/QTest>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <stdexcept>
#include <cmath>

namespace unit
{

ColoredQuadsTest::ColoredQuadsTest(QObject* parent)
    : QObject(parent)
{
}

void ColoredQuadsTest::initTestCase()
{
}

void ColoredQuadsTest::cleanupTestCase()
{
}

void ColoredQuadsTest::test_OpenGLQuads_ctor()
{
    ColoredQuads quads;
    QVERIFY(quads.quadCount() == 0);
}

void ColoredQuadsTest::test_addQuad()
{
    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    ColoredQuads quads;
    QVERIFY(quads.quadCount() == 0);

    quads.addQuad(q0, Qt::red);
    QVERIFY(quads.quadCount() == 1);

    quads.addQuad(q1, Qt::green);
    quads.addQuad(q2, Qt::blue);
    QVERIFY(quads.quadCount() == 3);

    QVERIFY(quads.quad(0) == q0);
    QVERIFY(quads.quad(1) == q1);
    QVERIFY(quads.quad(2) == q2);
}

void ColoredQuadsTest::test_setQuad()
{
    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    ColoredQuads quads;
    quads.addQuad(q0, Qt::red);
    quads.addQuad(q1, Qt::green);

    QVERIFY(quads.quad(0) == q0);
    QVERIFY(quads.quad(1) == q1);
    QVERIFY(quads.quadCount() == 2);

    quads.setQuad(0, q2, Qt::blue);
    QVERIFY(quads.quadCount() == 2);

    QVERIFY(quads.quad(0) != q0);
    QVERIFY(quads.quad(0) == q2);

    QVERIFY(quads.quadColor(0) != Qt::red);
    QVERIFY(quads.quadColor(0) == Qt::blue);
}

void ColoredQuadsTest::test_quadColor()
{
    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    ColoredQuads quads;
    quads.addQuad(q0, Qt::red);
    quads.addQuad(q1, Qt::green);
    quads.addQuad(q2, Qt::blue);

    QVERIFY(quads.quadColor(0) == Qt::red);
    QVERIFY(quads.quadColor(1) == Qt::green);
    QVERIFY(quads.quadColor(2) == Qt::blue);
}

void ColoredQuadsTest::test_quadCount()
{
    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    ColoredQuads quads;
    QVERIFY(quads.quadCount() == 0);

    quads.addQuad(q0, Qt::red);
    QVERIFY(quads.quadCount() == 1);

    quads.addQuad(q1, Qt::green);
    QVERIFY(quads.quadCount() == 2);

    quads.addQuad(q2, Qt::blue);
    QVERIFY(quads.quadCount() == 3);

    quads.clear();
    QVERIFY(quads.quadCount() == 0);
}

void ColoredQuadsTest::test_equals()
{
    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    ColoredQuads quadsA;
    ColoredQuads quadsB;
    QVERIFY(quadsA == quadsB);

    quadsA.addQuad(q0, Qt::white);
    QVERIFY(!(quadsA == quadsB));

    quadsB.addQuad(q0, Qt::white);
    QVERIFY(quadsA == quadsB);

    quadsA.addQuad(q1, Qt::red);
    quadsB.addQuad(q1, Qt::red);
    quadsA.addQuad(q2, Qt::blue);
    quadsB.addQuad(q2, Qt::blue);
    QVERIFY(quadsA == quadsB);
}

void ColoredQuadsTest::test_notequals()
{
    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    ColoredQuads quadsA;
    ColoredQuads quadsB;
    QVERIFY(!(quadsA != quadsB));

    quadsA.addQuad(q0, Qt::white);
    QVERIFY(quadsA != quadsB);

    quadsB.addQuad(q0, Qt::white);
    QVERIFY(!(quadsA != quadsB));

    quadsA.addQuad(q1, Qt::red);
    quadsB.addQuad(q1, Qt::red);
    quadsA.addQuad(q2, Qt::blue);
    quadsB.addQuad(q2, Qt::blue);
    QVERIFY(!(quadsA != quadsB));
}

void ColoredQuadsTest::test_drawingQuad()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeQuads = [&openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        ColoredQuads quads;

        quads.addQuad(QRectF(-1.0, -1.0, 2.0, 2.0), Qt::red);

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.draw(matrix, quads);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeQuads);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

void ColoredQuadsTest::test_drawLinesOnQuad()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeQuads = [&openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        ColoredQuads quads;
        ColoredLines lines;

        const QPointF topLeft(-1.0, -1.0);
        const QPointF topRight(1.0, -1.0);
        const QPointF bottomLeft(-1.0, 1.0);
        const QPointF bottomRight(1.0, 1.0);

        quads.addQuad(QRectF(topLeft, bottomRight), Qt::darkYellow);

        lines.addLine(QLineF(topLeft, topRight), Qt::red);
        lines.addLine(QLineF(topRight, bottomRight), Qt::blue);
        lines.addLine(QLineF(bottomRight, bottomLeft), Qt::green);
        lines.addLine(QLineF(bottomLeft, topLeft), Qt::white);
 
        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.draw(matrix, quads);
        renderer.draw(matrix, lines);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeQuads);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

void ColoredQuadsTest::test_alphaBlending()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeQuads = [&openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        ColoredQuads quads;
        ColoredLines lines;

        const QColor reddish(255, 0, 0, 128);
        const QColor greenish(0, 255, 0, 128);
        const QColor blueish(0, 0, 255, 128);

        quads.addQuad(QRectF(-1.0, -1.0, 1.5, 1.5), reddish);
        quads.addQuad(QRectF(-0.5, -0.5, 1.5, 1.5), greenish);
        quads.addQuad(QRectF(-0.75, -0.75, 1.5, 1.5), blueish);

        const QPointF topLeft(-1.0, -1.0);
        const QPointF topRight(1.0, -1.0);
        const QPointF bottomLeft(-1.0, 1.0);
        const QPointF bottomRight(1.0, 1.0);
        lines.addLine(QLineF(topLeft, topRight), Qt::white);
        lines.addLine(QLineF(topRight, bottomRight), Qt::white);
        lines.addLine(QLineF(bottomRight, bottomLeft), Qt::white);
        lines.addLine(QLineF(bottomLeft, topLeft), Qt::white);

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.draw(matrix, quads);
        renderer.draw(matrix, lines);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeQuads);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

} // namespace unit

QTEST_MAIN(unit::ColoredQuadsTest)
#include "test_gui_ColoredQuads.moc"

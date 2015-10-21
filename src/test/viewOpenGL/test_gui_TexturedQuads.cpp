/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include "test_gui_TexturedQuads.h"
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
namespace
{
QSharedPointer<QImage> makeTestImage(const unsigned int sideLength)
{
    QSharedPointer<QImage> image(new QImage(sideLength, sideLength, QImage::Format_ARGB32));

    image->fill(Qt::white);

    QPainter painter(&*image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int halfSide = static_cast<int>(sideLength / 2u);

    painter.setPen(QPen(Qt::black, 16));
    painter.drawText(QPointF(halfSide / 8, halfSide / 8), "HELLO WORLD");

    painter.translate(sideLength / 2.0, sideLength / 2.0);
    const int circleSize = static_cast<int>(sideLength);
    int colorChanger = 0;
    for (int r = 0; r < circleSize; r += 25) {

        if (colorChanger == 0) {
            painter.setPen(QPen(Qt::yellow, 5));
        } else if (colorChanger == 1) {
            painter.setPen(QPen(Qt::blue, 5));
        } else {
            painter.setPen(QPen(Qt::red, 5));
        }

        colorChanger = (colorChanger + 1) % 3;

        painter.drawEllipse(QRectF(-r / 2.0, -r / 2.0, r, r));
    }

    return image;
}

QSharedPointer<QOpenGLTexture> makeTestTexture(const unsigned int sideLength)
{
    auto image = makeTestImage(sideLength);
    QSharedPointer<QOpenGLTexture> texture(new QOpenGLTexture(image->mirrored()));
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    return texture;
}
}

TexturedQuadsTest::TexturedQuadsTest(QObject* parent)
    : QObject(parent)
{
}

void TexturedQuadsTest::initTestCase()
{
}

void TexturedQuadsTest::cleanupTestCase()
{
}

void TexturedQuadsTest::test_equals_different_quads()
{
    // Same texture coords, differing quads.

    const QRectF q0(0.0, 0.0, 1.0, 1.0);
    const QRectF q1(2.0, 2.0, 1.0, 1.0);
    const QRectF q2(5.0, 5.0, 2.0, 2.0);

    const TexturedQuads::RectUVCoords uvCoords = TexturedQuads::defaultTextureCoords();

    TexturedQuads quadsA;
    TexturedQuads quadsB;
    QVERIFY(quadsA == quadsB);
    QVERIFY(!(quadsA != quadsB));

    quadsA.addQuad(q0, uvCoords, Qt::white);
    QVERIFY(!(quadsA == quadsB));
    QVERIFY(quadsA != quadsB);

    quadsB.addQuad(q0, uvCoords, Qt::white);
    QVERIFY(quadsA == quadsB);
    QVERIFY(!(quadsA != quadsB));

    quadsA.addQuad(q1, uvCoords, Qt::red);
    quadsB.addQuad(q1, uvCoords, Qt::red);
    quadsA.addQuad(q2, uvCoords, Qt::blue);
    quadsB.addQuad(q2, uvCoords, Qt::blue);
    QVERIFY(quadsA == quadsB);
    QVERIFY(!(quadsA != quadsB));
}

void TexturedQuadsTest::test_quadTextureCoords()
{
    const QRectF point(0.0, 1.0, 2.0, 3.0);

    TexturedQuads::RectUVCoords uvCoords;
    uvCoords[0] = QPointF(0.0f, 1.0f);
    uvCoords[1] = QPointF(2.0f, 3.0f);
    uvCoords[2] = QPointF(4.0f, 5.0f);
    uvCoords[3] = QPointF(5.0f, 6.0f);

    TexturedQuads quads;

    quads.addQuad(point, uvCoords, Qt::gray);

    auto coords = quads.quadTextureCoords(0);

    QVERIFY(coords == uvCoords);

    TexturedQuads::RectUVCoords uvCoordsB;
    uvCoordsB[0] = QPointF(40.0f, 51.0f);
    uvCoordsB[1] = QPointF(42.0f, 53.0f);
    uvCoordsB[2] = QPointF(44.0f, 55.0f);
    uvCoordsB[3] = QPointF(45.0f, 56.0f);

    quads.setQuad(0, point, uvCoordsB, Qt::gray);

    coords = quads.quadTextureCoords(0);

    QVERIFY(coords == uvCoordsB);
}

void TexturedQuadsTest::test_defaultTextureCoords()
{
    const TexturedQuads::RectUVCoords uvCoords = TexturedQuads::defaultTextureCoords();

    const QPointF topLeft(0.0f, 0.0f);
    const QPointF topRight(1.0f, 0.0f);
    const QPointF bottomRight(1.0f, 1.0f);
    const QPointF bottomLeft(0.0f, 1.0f);

    QVERIFY(uvCoords[0] == topLeft);
    QVERIFY(uvCoords[1] == topRight);
    QVERIFY(uvCoords[2] == bottomRight);
    QVERIFY(uvCoords[3] == bottomLeft);
}

void TexturedQuadsTest::test_equality_uvcoords()
{
    // Same quads, differing texture coords.

    const QRectF point(0.0, 1.0, 2.0, 3.0);

    TexturedQuads::RectUVCoords uvCoords0 = TexturedQuads::defaultTextureCoords();

    TexturedQuads::RectUVCoords uvCoords1;
    uvCoords1[0] = QPointF(0.0, 0.1);
    uvCoords1[1] = QPointF(0.2, 0.3);
    uvCoords1[2] = QPointF(0.4, 0.5);
    uvCoords1[3] = QPointF(0.5, 0.6);

    TexturedQuads::RectUVCoords uvCoords2;
    uvCoords2[0] = QPointF(0.1, 0.2);
    uvCoords2[1] = QPointF(0.3, 0.4);
    uvCoords2[2] = QPointF(0.5, 0.6);
    uvCoords2[3] = QPointF(0.7, 0.8);

    TexturedQuads quadsA;
    TexturedQuads quadsB;
    QVERIFY(quadsA == quadsB);
    QVERIFY(!(quadsA != quadsB));

    quadsA.addQuad(point, uvCoords0, Qt::white);
    QVERIFY(!(quadsA == quadsB));
    QVERIFY(quadsA != quadsB);

    quadsB.addQuad(point, uvCoords0, Qt::white);
    QVERIFY(quadsA == quadsB);
    QVERIFY(!(quadsA != quadsB));

    quadsA.addQuad(point, uvCoords1, Qt::red);
    quadsB.addQuad(point, uvCoords1, Qt::red);
    quadsA.addQuad(point, uvCoords2, Qt::blue);
    quadsB.addQuad(point, uvCoords2, Qt::blue);
    QVERIFY(quadsA == quadsB);
    QVERIFY(!(quadsA != quadsB));
}

void TexturedQuadsTest::test_drawingMissingTextureTexturedQuad()
{
    const int visibleDurationMs = 500;

    bool openglHadError = false;

    // This test generates a lot of noise as each failed call to draw (due to missing texture)
    // generates a debug message. So we only do the drawing once. However this can mean that
    // the visualization is so fast this is not a good interactive test as you fail to see the
    // fact that the draw call succeeds (it correctly renders the requested quads, but with
    // a default yellow texture).

    bool runAlready = false;

    auto drawSomeQuads = [&runAlready, &openglHadError](void) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        TexturedQuads quads;
        TexturedQuads::RectUVCoords uvCoords = TexturedQuads::defaultTextureCoords();

        quads.addQuad(QRectF(-1.0, -1.0, 2.0, 2.0), uvCoords, Qt::white);

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        // Only do the draw call once due to 'noise' from qDebug.
        if (!runAlready) {
            runAlready = true;
            renderer.draw(matrix, "NO TEXTURE WITH THIS NAME", quads);
        }

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeQuads);

    // CppCheck incorrectly reports runAlready as unused, so we "use" it here.
    Q_UNUSED(runAlready);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}

void TexturedQuadsTest::test_drawingTexturedQuad()
{
    const int visibleDurationMs = 1200;

    bool openglHadError = false;

    auto drawSomeQuads = [&openglHadError](void) {

        auto texture = makeTestTexture(512u);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ASSERT_OPENGL_OK;
        TexturedQuads quads;
        TexturedQuads::RectUVCoords uvCoords = TexturedQuads::defaultTextureCoords();

        quads.addQuad(QRectF(-1.0, -1.0, 2.0, 2.0), uvCoords, Qt::white);

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        matrix.translate(0, 0, -2);

        Renderer renderer;

        renderer.addTexture("test", texture);
        renderer.draw(matrix, "test", quads);

        if (!checkOpenGLNoError()) {
            openglHadError = true;
        }
    };

    const bool shown = OpenGLTestWindow::createAndShowWindow(visibleDurationMs, drawSomeQuads);

    QVERIFY2(shown, "Window was not visible.");
    QVERIFY2(!openglHadError, "OpenGL detected an error.");
}
} // namespace unit

QTEST_MAIN(unit::TexturedQuadsTest)
#include "test_gui_TexturedQuads.moc"

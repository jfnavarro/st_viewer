/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include <qdebug.h>

#include <GLCommon.h>
#include <GLColor.h>
#include <compression/GLBitColor.h>

Q_DECLARE_METATYPE(GL::GLcolor)

#include "tst_glcolortest.h"

namespace unit
{

GLColorTest::GLColorTest(QObject *parent) : QObject(parent) { }

void GLColorTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}
void GLColorTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void GLColorTest::testCompressToRGB8Bit()
{
    QFETCH(GL::GLcolor, src);
    QFETCH(GL::GLcolorRGB8b, tar);
    QFETCH(bool, expected);

    QCOMPARE(GL::compressToRGB8Bit(src) == tar, expected);
}
void GLColorTest::testCompressToRGB8Bit_data()
{
    QTest::addColumn<GL::GLcolor>("src");
    QTest::addColumn<GL::GLcolorRGB8b>("tar");
    QTest::addColumn<bool>("expected");

    QTest::newRow("primecolor0") << GL::GLcolor(1.0f, 0.0f, 0.0f) << GL::GLcolorRGB8b(0xE0u) << true;
    QTest::newRow("primecolor1") << GL::GLcolor(0.0f, 1.0f, 0.0f) << GL::GLcolorRGB8b(0x1Cu) << true;
    QTest::newRow("primecolor2") << GL::GLcolor(0.0f, 0.0f, 1.0f) << GL::GLcolorRGB8b(0x03u) << true;

    QTest::newRow("shade0") << GL::GLcolor(0.33f, 0.33f, 0.33f) << GL::GLcolorRGB8b(0x49u) << true;
    QTest::newRow("shade1") << GL::GLcolor(0.66f, 0.66f, 0.66f) << GL::GLcolorRGB8b(0xB6u) << true;
}

void GLColorTest::testDecompressFromRGB8Bit()
{
    QFETCH(GL::GLcolorRGB8b, src);
    QFETCH(GL::GLcolor, tar);
    QFETCH(bool, expected);

    QCOMPARE(GL::fuzzyEqual(GL::decompressFromRGB8Bit(src), tar), expected);
}
void GLColorTest::testDecompressFromRGB8Bit_data()
{
    QTest::addColumn<GL::GLcolorRGB8b>("src");
    QTest::addColumn<GL::GLcolor>("tar");
    QTest::addColumn<bool>("expected");

    QTest::newRow("primecolor0") << GL::GLcolorRGB8b(0xE0u) << GL::GLcolor(1.0f, 0.0f, 0.0f) << true;
    QTest::newRow("primecolor1") << GL::GLcolorRGB8b(0x1Cu) << GL::GLcolor(0.0f, 1.0f, 0.0f) << true;
    QTest::newRow("primecolor2") << GL::GLcolorRGB8b(0x03u) << GL::GLcolor(0.0f, 0.0f, 1.0f) << true;

    const GLfloat c_inv_3 = (1.0f / 3.0f);
    const GLfloat c_inv_7 = (1.0f / 7.0f);
    QTest::newRow("shade0") << GL::GLcolorRGB8b(0x49u) << GL::GLcolor(2.0f * c_inv_7, 2.0f * c_inv_7, 1.0f * c_inv_3) << true;
    QTest::newRow("shade1") << GL::GLcolorRGB8b(0xB6u) << GL::GLcolor(5.0f * c_inv_7, 5.0f * c_inv_7, 2.0f * c_inv_3) << true;
}

void GLColorTest::testCompressToRGB16Bit()
{
    QFETCH(GL::GLcolor, src);
    QFETCH(GL::GLcolorRGB16b, tar);
    QFETCH(bool, expected);

    qDebug() << hex << tar << GL::compressToRGB16Bit(src);

    QCOMPARE(GL::compressToRGB16Bit(src) == tar, expected);
}
void GLColorTest::testCompressToRGB16Bit_data()
{
    QTest::addColumn<GL::GLcolor>("src");
    QTest::addColumn<GL::GLcolorRGB16b>("tar");
    QTest::addColumn<bool>("expected");

    QTest::newRow("primecolor0") << GL::GLcolor(1.0f, 0.0f, 0.0f) << GL::GLcolorRGB16b(0xF800u) << true;
    QTest::newRow("primecolor1") << GL::GLcolor(0.0f, 1.0f, 0.0f) << GL::GLcolorRGB16b(0x07E0u) << true;
    QTest::newRow("primecolor2") << GL::GLcolor(0.0f, 0.0f, 1.0f) << GL::GLcolorRGB16b(0x001Fu) << true;

    QTest::newRow("shade0") << GL::GLcolor(0.33f, 0.33f, 0.33f) << GL::GLcolorRGB16b(0x52AAu) << true;
    QTest::newRow("shade1") << GL::GLcolor(0.66f, 0.66f, 0.66f) << GL::GLcolorRGB16b(0xA554u) << true;
}

void GLColorTest::testDecompressFromRGB16Bit()
{
    QFETCH(GL::GLcolorRGB16b, src);
    QFETCH(GL::GLcolor, tar);
    QFETCH(bool, expected);

    QCOMPARE(GL::fuzzyEqual(GL::decompressFromRGB16Bit(src), tar), expected);
}
void GLColorTest::testDecompressFromRGB16Bit_data()
{
    QTest::addColumn<GL::GLcolorRGB16b>("src");
    QTest::addColumn<GL::GLcolor>("tar");
    QTest::addColumn<bool>("expected");

    QTest::newRow("primecolor0") << GL::GLcolorRGB16b(0xF800u) << GL::GLcolor(1.0f, 0.0f, 0.0f) << true;
    QTest::newRow("primecolor1") << GL::GLcolorRGB16b(0x07E0u) << GL::GLcolor(0.0f, 1.0f, 0.0f) << true;
    QTest::newRow("primecolor2") << GL::GLcolorRGB16b(0x001Fu) << GL::GLcolor(0.0f, 0.0f, 1.0f) << true;

    const GLfloat c_inv_63 = (1.0f / 63.0f);
    const GLfloat c_inv_31 = (1.0f / 31.0f);
    QTest::newRow("shade0") << GL::GLcolorRGB16b(0x52AAu) << GL::GLcolor(10.0f * c_inv_31, 21.0f * c_inv_63, 10.0f * c_inv_31) << true;
    QTest::newRow("shade1") << GL::GLcolorRGB16b(0xA554u) << GL::GLcolor(20.0f * c_inv_31, 42.0f * c_inv_63, 20.0f * c_inv_31) << true;
}

} // namespace unit //

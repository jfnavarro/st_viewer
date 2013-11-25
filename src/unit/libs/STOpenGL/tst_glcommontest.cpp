/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include <GLCommon.h>
Q_DECLARE_METATYPE(GL::GLpoint)
Q_DECLARE_METATYPE(GL::GLline)
Q_DECLARE_METATYPE(GL::GLtriangle)
Q_DECLARE_METATYPE(GL::GLrectangle)

#include "tst_glcommontest.h"

namespace unit
{

GLCommonTest::GLCommonTest(QObject *parent) : QObject(parent) { }

void GLCommonTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}
void GLCommonTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void GLCommonTest::testFuzzyEqual()
{
    QFETCH(GLfloat, scalar);
    QFETCH(GLfloat, approx);
    QFETCH(GLfloat, epsilon);
    QFETCH(bool, expected);

    QCOMPARE(GL::fuzzyEqual(scalar, approx, epsilon), expected);
}

void GLCommonTest::testFuzzyEqual_data()
{
    QTest::addColumn<GLfloat>("scalar");
    QTest::addColumn<GLfloat>("approx");
    QTest::addColumn<GLfloat>("epsilon");
    QTest::addColumn<bool>("expected");

    QTest::newRow("equal")  << 1.414f << 1.414f << 10e-5f << true;
    QTest::newRow("not equal") << 1.414f << 3.141f << 10e-5f << false;
    QTest::newRow("approx") << 1.41421356237f << 1.414f << 10e-3f << true;
    QTest::newRow("not approx") << 1.41421356237f << 1.414f << 10e-5f << false;
}

void GLCommonTest::testFuzzyNotEqual()
{
    QFETCH(GLfloat, scalar);
    QFETCH(GLfloat, approx);
    QFETCH(GLfloat, epsilon);
    QFETCH(bool, expected);

    QCOMPARE(GL::fuzzyNotEqual(scalar, approx, epsilon), expected);
}

void GLCommonTest::testFuzzyNotEqual_data()
{
    QTest::addColumn<GLfloat>("scalar");
    QTest::addColumn<GLfloat>("approx");
    QTest::addColumn<GLfloat>("epsilon");
    QTest::addColumn<bool>("expected");

    QTest::newRow("equal") << 1.414f << 1.414f << 10e-5f << false;
    QTest::newRow("not equal") << 1.414f << 3.141f << 10e-5f << true;
    QTest::newRow("approx") << 1.41421356237f << 1.414f << 10e-3f << false;
    QTest::newRow("not approx") << 1.41421356237f << 1.414f << 10e-5f << true;
}

void GLCommonTest::testLineFromPoint()
{
    QFETCH(GL::GLpoint, p0);
    QFETCH(GL::GLpoint, p1);
    QFETCH(GL::GLline, l0);
    QFETCH(bool, expected);

    QCOMPARE(fuzzyEqual(GL::GLline(p0, p1), l0), expected);
}
void GLCommonTest::testLineFromPoint_data()
{
    QTest::addColumn<GL::GLpoint>("p0");
    QTest::addColumn<GL::GLpoint>("p1");
    QTest::addColumn<GL::GLline>("l0");
    QTest::addColumn<bool>("expected");

    QTest::newRow("positive") << GL::GLpoint(1.0f, 1.0f) << GL::GLpoint(3.0f,  2.0f) << GL::GLline(1.0f, 1.0f, 3.0f,  2.0f) << true;
    QTest::newRow("mixed") << GL::GLpoint(-1.0f, 1.0f) << GL::GLpoint(3.0f, -2.0f) << GL::GLline(-1.0f, 1.0f, 3.0f, -2.0f) << true;
}

} // namespace unit //

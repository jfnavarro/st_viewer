/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include <GLQt.h>
#include <GLCommon.h>

Q_DECLARE_METATYPE(GL::GLcolor)

#include <image/GLHeatMap.h>

#include "tst_glheatmaptest.h"

namespace unit
{

GLHeatMapTest::GLHeatMapTest(QObject *parent) : QObject(parent) { }

void GLHeatMapTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}
void GLHeatMapTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void GLHeatMapTest::testHeatMap()
{
    QFETCH(GLfloat, wavelength);
    QFETCH(GL::GLcolor, color);
    QFETCH(bool, expected);
    const GL::GLcolor approximation = GL::GLheatmap::createHeatMapColor(wavelength);
    QCOMPARE(GL::fuzzyEqual(approximation, color), expected);
}
void GLHeatMapTest::testHeatMap_data()
{
    QTest::addColumn<GLfloat>("wavelength");
    QTest::addColumn<GL::GLcolor>("color");
    QTest::addColumn<bool>("expected");
    QTest::newRow("red") << GLfloat(645.0) << GL::GLcolor(GL::Red) << true;
    QTest::newRow("yellow") << GLfloat(580.0) << GL::GLcolor(GL::Yellow) << true;
    QTest::newRow("green") << GLfloat(510.0) << GL::GLcolor(GL::Green) << true;
    QTest::newRow("cyan") << GLfloat(490.0) << GL::GLcolor(GL::Cyan) << true;
    QTest::newRow("blue") << GLfloat(440.0) << GL::GLcolor(GL::Blue) << true;
}

} // namespace unit //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "GLCommon.h"
#include "data/GLHeatMap.h"

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
    QFETCH(qreal, wavelength);
    QFETCH(QColor4ub, color);
    QFETCH(bool, expected);
    const QColor4ub approximation = GL::GLheatmap::createHeatMapColor(wavelength);
    QCOMPARE((approximation == color), expected);
}
void GLHeatMapTest::testHeatMap_data()
{
    QTest::addColumn<qreal>("wavelength");
    QTest::addColumn<QColor4ub>("color");
    QTest::addColumn<bool>("expected");
    QTest::newRow("red") << qreal(645.0) << QColor4ub(Qt::red) << true;
    QTest::newRow("yellow") << qreal(580.0) << QColor4ub(Qt::yellow) << true;
    QTest::newRow("green") << qreal(510.0) << QColor4ub(Qt::green) << true;
    QTest::newRow("cyan") << qreal(490.0) << QColor4ub(Qt::cyan) << true;
    QTest::newRow("blue") << qreal(440.0) << QColor4ub(Qt::blue) << true;
}

} // namespace unit //

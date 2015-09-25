/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "math/Common.h"
#include "color/HeatMap.h"

#include "tst_glheatmaptest.h"

namespace unit
{

GLHeatMapTest::GLHeatMapTest(QObject* parent)
    : QObject(parent)
{
}

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
    QVERIFY2(true, "Empty");
    /*
    QFETCH(qreal, wavelength);
    QFETCH(QColor4ub, color);
    QFETCH(bool, expected);
    const QColor4ub approximation = Heatmap::createHeatMapWaveLenghtColor(wavelength);
    QCOMPARE((approximation == color), expected);*/
}
void GLHeatMapTest::testHeatMap_data()
{
    QVERIFY2(true, "Empty");
    /*
    QTest::addColumn<qreal>("wavelength");
    QTest::addColumn<QColor4ub>("color");
    QTest::addColumn<bool>("expected");
    QTest::newRow("red") << qreal(645.0) << QColor4ub(Qt::red) << true;
    QTest::newRow("yellow") << qreal(580.0) << QColor4ub(Qt::yellow) << true;
    QTest::newRow("green") << qreal(510.0) << QColor4ub(Qt::green) << true;
    QTest::newRow("cyan") << qreal(490.0) << QColor4ub(Qt::cyan) << true;
    QTest::newRow("blue") << qreal(440.0) << QColor4ub(Qt::blue) << true;*/
}

} // namespace unit //

QTEST_MAIN(unit::GLHeatMapTest)
#include "tst_glheatmaptest.moc"

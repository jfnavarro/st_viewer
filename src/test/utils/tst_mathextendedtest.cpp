/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "math/Common.h"
#include "tst_mathextendedtest.h"

namespace unit
{

MathExtendedTest::MathExtendedTest(QObject *parent) : QObject(parent) { }

void MathExtendedTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}

void MathExtendedTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void MathExtendedTest::testFloatMod()
{
    QFETCH(qreal, dividend);
    QFETCH(qreal, divisor);
    QFETCH(qreal, result);
    QFETCH(bool, expected);

    QCOMPARE(qFuzzyCompare(STMath::qMod(dividend, divisor), result), expected);
}
void MathExtendedTest::testFloatMod_data()
{
    QTest::addColumn<qreal>("dividend");
    QTest::addColumn<qreal>("divisor");
    QTest::addColumn<qreal>("result");
    QTest::addColumn<bool>("expected");

    QTest::newRow("mod_one") << (qreal)(42.70) << (qreal)(1.00) << (qreal)(0.70) << true;
    QTest::newRow("mod_half") << (qreal)(1.23) << (qreal)(0.50) << (qreal)(0.23) << true;
    QTest::newRow("-mod_half") << (qreal)(-4.20) << (qreal)(0.50) << (qreal)(0.30) << true;
    QTest::newRow("mod_-half") << (qreal)(1.23) << (qreal)(-0.50) << (qreal)(-0.27) << true;
}

// helper function
bool fuzzyCompare(const QSizeF &s1, const QSizeF &s2)
{
    return qFuzzyCompare(s1.width(), s2.width()) && qFuzzyCompare(s1.height(), s2.height());
}

void MathExtendedTest::testClamp()
{
    QFETCH(QSizeF, size);
    QFETCH(QSizeF, min);
    QFETCH(QSizeF, max);
    QFETCH(QSizeF, result);
    QFETCH(uint, mode);
    QFETCH(bool, expected);

    QCOMPARE(fuzzyCompare(STMath::clamp(size, min, max, (Qt::AspectRatioMode) mode), result), expected);
}
void MathExtendedTest::testClamp_data()
{
    QTest::addColumn<QSizeF>("size");
    QTest::addColumn<QSizeF>("min");
    QTest::addColumn<QSizeF>("max");
    QTest::addColumn<QSizeF>("result");
    QTest::addColumn<uint>("mode");
    QTest::addColumn<bool>("expected");

    QTest::newRow("shrink_ignore_ratio") << QSizeF(4.0, 6.0) << QSizeF(1.0, 1.0) << QSizeF(4.0, 4.0) << QSizeF(4.0, 4.0) << (uint)(Qt::IgnoreAspectRatio) << true;
    QTest::newRow("shrink_keep_ratio") << QSizeF(4.0, 6.0) << QSizeF(1.0, 1.0) << QSizeF(4.0, 4.0) << QSizeF((8.0 / 3.0), 4.0) << (uint)(Qt::KeepAspectRatio)   << true;
    QTest::newRow("expand_ignore_ratio") << QSizeF(0.4, 0.6) << QSizeF(1.0, 1.0) << QSizeF(4.0, 4.0) << QSizeF(1.0, 1.0) << (uint)(Qt::IgnoreAspectRatio) << true;
    QTest::newRow("expand_keep_ratio") << QSizeF(0.4, 0.6) << QSizeF(1.0, 1.0) << QSizeF(4.0, 4.0) << QSizeF(1.0, 1.5) << (uint)(Qt::KeepAspectRatio)   << true;
}

} // namespace unit //

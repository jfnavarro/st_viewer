#include <QtTest/QTest>

#include "math/Common.h"
#include "tst_mathextendedtest.h"

namespace unit
{

MathExtendedTest::MathExtendedTest(QObject *parent)
    : QObject(parent)
{
}

void MathExtendedTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}

void MathExtendedTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}




} // namespace unit //

QTEST_MAIN(unit::MathExtendedTest)
#include "tst_mathextendedtest.moc"

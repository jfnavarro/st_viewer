#include "test_rest.h"

#include <QtTest/QTest>

namespace unit
{

RestTest::RestTest(QObject *parent)
    : QObject(parent)
{
}

void RestTest::initTestCase()
{
    QVERIFY(true);
}

void RestTest::cleanupTestCase()
{
}

} // namespace unit //

QTEST_MAIN(unit::RestTest)
#include "test_rest.moc"

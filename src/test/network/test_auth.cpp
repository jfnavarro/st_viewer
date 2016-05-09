#include "test_auth.h"

#include <QtTest/QTest>

namespace unit
{

AuthTest::AuthTest(QObject *parent)
    : QObject(parent)
{
}

void AuthTest::initTestCase()
{
    QVERIFY(true);
}

void AuthTest::cleanupTestCase()
{
}

} // namespace unit //

QTEST_MAIN(unit::AuthTest)
#include "test_auth.moc"

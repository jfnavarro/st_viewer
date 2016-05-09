#include <QtTest/QTest>

#include "data/ObjectParser.h"

#include "tst_objectparsertest.h"

namespace unit
{

ObjectParserTest::ObjectParserTest(QObject *parent)
    : QObject(parent)
{
}

void ObjectParserTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}

void ObjectParserTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void ObjectParserTest::testParseObject()
{
    QVERIFY2(true, "Empty");
}
void ObjectParserTest::testParseObject_data()
{
}

} // namespace unit //

QTEST_MAIN(unit::ObjectParserTest)
#include "tst_objectparsertest.moc"

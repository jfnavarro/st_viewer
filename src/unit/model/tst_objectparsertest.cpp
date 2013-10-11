/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "model/ObjectParser.h"

#include "tst_objectparsertest.h"

namespace unit
{

    ObjectParserTest::ObjectParserTest(QObject *parent) : QObject(parent) { }

    void ObjectParserTest::initTestCase()
    {
        QVERIFY2( true, "Empty" );
    }

    void ObjectParserTest::cleanupTestCase()
    {
        QVERIFY2( true, "Empty" );
    }

	void ObjectParserTest::testParseObject()
	{
		QVERIFY2( true, "Empty" );
	}
	void ObjectParserTest::testParseObject_data()
	{
	}

} // namespace unit //

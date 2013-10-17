/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QTest>

#include "tst_lineeditcleartest.h"

namespace unit
{

    LineEditClearTest::LineEditClearTest(QObject *parent) : QObject(parent) { }

    void LineEditClearTest::initTestCase()
	{
		QVERIFY2( true, "Empty" );
	}
    void LineEditClearTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty" );
	}

	void LineEditClearTest::testClear()
	{
		QVERIFY(true);
	}


} // namespace unit //


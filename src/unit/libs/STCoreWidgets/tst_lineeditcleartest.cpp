/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QTest>

#include <core/LineEditClear.h>

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
		QFETCH(QString, text);

		QScopedPointer<LineEditClear> lec(new LineEditClear());

		// verify initial state
		QVERIFY( lec->text().isEmpty() );

		// verify assignment
		QTest::keyClicks(lec.data(), text, Qt::ShiftModifier);
		QCOMPARE( lec->text(), text.toUpper() );

		// verify clear
		lec->clear();
		QVERIFY( lec->text().isEmpty() );
	}

	void LineEditClearTest::testClear_data()
	{
		QTest::addColumn<QString>("text");

		QTest::newRow("text") << QString("some text");
	}

} // namespace unit //


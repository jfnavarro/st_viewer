/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include <math/GLMatrix.h>
Q_DECLARE_METATYPE(GL::GLmatrix)

#include "tst_glmatrixtest.h"

namespace unit
{

    GLMatrixTest::GLMatrixTest(QObject *parent) : QObject(parent) { }

    void GLMatrixTest::initTestCase()
	{
		QVERIFY2( true, "Empty" );
	}
    void GLMatrixTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty" );
	}

    void GLMatrixTest::testCompareEqual()
    {
        QFETCH(GL::GLmatrix, m0);
        QFETCH(GL::GLmatrix, m1);
        QFETCH(bool, expected);

        QCOMPARE( fuzzyEqual(m0, m1) , expected);
    }

    void GLMatrixTest::testCompareEqual_data()
    {
        QTest::addColumn<GL::GLmatrix>("m0");
        QTest::addColumn<GL::GLmatrix>("m1");
        QTest::addColumn<bool>("expected");

        GL::GLmatrix identity;
        GL::GLmatrix simple(0,1,2, 3,4,5, 6,7,8);

        QTest::newRow("identity") << identity << identity << true;
        QTest::newRow("simple") << simple << simple << true;
    }

} // namespace unit //

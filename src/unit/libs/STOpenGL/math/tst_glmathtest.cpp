/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include <GLCommon.h>
#include <math/GLMath.h>

#include "tst_glmathtest.h"

namespace unit
{

    GLMathTest::GLMathTest(QObject *parent) : QObject(parent) { }

    void GLMathTest::initTestCase()
	{
		QVERIFY2( true, "Empty" );
	}
    void GLMathTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty" );
	}

    void GLMathTest::testRangeIncl()
    {
        QFETCH(GLfloat, v);
        QFETCH(GLfloat, lo);
        QFETCH(GLfloat, hi);
        QFETCH(bool, expected);

        QCOMPARE( GL::range<GLfloat>::compare(v, lo, hi), expected ); // inclusive [lo, hi]
    }
    void GLMathTest::testRangeIncl_data()
    {
        QTest::addColumn<GLfloat>("v");
        QTest::addColumn<GLfloat>("lo");
        QTest::addColumn<GLfloat>("hi");
        QTest::addColumn<bool>("expected");

        QTest::newRow("simple0") << GLfloat(2.5) << GLfloat(2.0) << GLfloat(3.0) << true;
        QTest::newRow("simple1") << GLfloat(1.5) << GLfloat(1.0) << GLfloat(1.5) << true;
        QTest::newRow("simple2") << GLfloat(2.0) << GLfloat(1.0) << GLfloat(2.0) << true;
        QTest::newRow("simple3") << GLfloat(0.0) << GLfloat(-1.0) << GLfloat(1.0) << true;

        QTest::newRow("false0") << GLfloat(0.0) << GLfloat(0.1) << GLfloat(1.0) << false;
        QTest::newRow("false1") << GLfloat(-1.0) << GLfloat(-3.0) << GLfloat(-2.0) << false;
        QTest::newRow("false2") << GLfloat(1.0) << GLfloat(-2.0) << GLfloat(0.0) << false;
    }

    void GLMathTest::testRangeExcl()
    {
        QFETCH(GLfloat, v);
        QFETCH(GLfloat, lo);
        QFETCH(GLfloat, hi);
        QFETCH(bool, expected);

        // typedef range operator to simplify
        typedef GL::comp_op_gt<GLfloat> op_lo_t;
        typedef GL::comp_op_ls<GLfloat> op_hi_t;
        typedef GL::range<GLfloat,op_lo_t,op_hi_t> range_t;
        QCOMPARE( range_t::compare(v, lo, hi), expected ); // exclusive ]lo, hi[
    }
    void GLMathTest::testRangeExcl_data()
    {
        QTest::addColumn<GLfloat>("v");
        QTest::addColumn<GLfloat>("lo");
        QTest::addColumn<GLfloat>("hi");
        QTest::addColumn<bool>("expected");

        QTest::newRow("simple0") << GLfloat(2.5) << GLfloat(2.0) << GLfloat(3.0) << true;
        QTest::newRow("simple1") << GLfloat(1.5) << GLfloat(1.0) << GLfloat(1.5) << false;
        QTest::newRow("simple2") << GLfloat(2.0) << GLfloat(1.0) << GLfloat(2.0) << false;
        QTest::newRow("simple3") << GLfloat(0.0) << GLfloat(-1.0) << GLfloat(1.0) << true;

        QTest::newRow("false0") << GLfloat(0.0) << GLfloat(0.1)  << GLfloat(1.0)  << false;
        QTest::newRow("false1") << GLfloat(-1.0) << GLfloat(-3.0) << GLfloat(-2.0) << false;
        QTest::newRow("false2") << GLfloat(1.0) << GLfloat(-2.0) << GLfloat(0.0)  << false;
    }

} // namespace unit //

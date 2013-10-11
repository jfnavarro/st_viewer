/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#include <QtTest/QTest>

#include <qdebug.h>

#include <GLCommon.h>
#include <math/GLFloat.h>

#include "tst_glfloattest.h"

namespace unit
{

    GLFloatTest::GLFloatTest(QObject *parent) : QObject(parent) { }

    void GLFloatTest::initTestCase()
	{
		QVERIFY2( true, "Empty" );
	}
    void GLFloatTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty" );
	}

    void GLFloatTest::testBitSet()
    {
        QFETCH(GLfloat, bitFloatSrc);
        QFETCH(GLfloat, bitFloatTar);
        QFETCH(GLuint, bit);
        QFETCH(bool, expected);

        QCOMPARE( GL::bitSet(bitFloatSrc, bit) == bitFloatTar, expected );
    }
    void GLFloatTest::testBitSet_data()
    {
        QTest::addColumn<GLfloat>("bitFloatSrc");
        QTest::addColumn<GLfloat>("bitFloatTar");
        QTest::addColumn<GLuint>("bit");
        QTest::addColumn<bool>("expected");

        QTest::newRow("setbit0") << GLfloat(3.0) << GLfloat( 11.0) << GLuint(3) << true;
        QTest::newRow("setbit2") << GLfloat(161.0) << GLfloat(169.0) << GLuint(3) << true;
        QTest::newRow("setbit3") << GLfloat(239.0) << GLfloat(255.0) << GLuint(4) << true;
        QTest::newRow("setbit3") << GLfloat(42.0) << GLfloat( 58.0) << GLuint(4) << true;

        QTest::newRow("nothing0") << GLfloat(7.0) << GLfloat(7.0) << GLuint(1) << true;
    }

    void GLFloatTest::testBitClear()
    {
        QFETCH(GLfloat, bitFloatSrc);
        QFETCH(GLfloat, bitFloatTar);
        QFETCH(GLuint, bit);
        QFETCH(bool, expected);

        QCOMPARE( GL::bitClear(bitFloatSrc, bit) == bitFloatTar, expected );
    }
    void GLFloatTest::testBitClear_data()
    {
        QTest::addColumn<GLfloat>("bitFloatSrc");
        QTest::addColumn<GLfloat>("bitFloatTar");
        QTest::addColumn<GLuint>("bit");
        QTest::addColumn<bool>("expected");

        QTest::newRow("clearbit0") << GLfloat(3.0) << GLfloat(1.0) << GLuint(1) << true;
        QTest::newRow("clearbit0") << GLfloat(7.0) << GLfloat(5.0) << GLuint(1) << true;
        QTest::newRow("clearbit0") << GLfloat(161.0) << GLfloat(129.0) << GLuint(5) << true;
        QTest::newRow("clearbit0") << GLfloat(255.0) << GLfloat(239.0) << GLuint(4) << true;

        QTest::newRow("nothing0") << GLfloat(5.0) << GLfloat(5.0) << GLuint(1) << true;
    }

    void GLFloatTest::testBitTest()
    {
        QFETCH(GLfloat, bitFloat);
        QFETCH(GLuint, bit);
        QFETCH(bool, expected);

        QCOMPARE( GL::bitTest(bitFloat, bit), expected );
    }
    void GLFloatTest::testBitTest_data()
    {
        QTest::addColumn<GLfloat>("bitFloat");
        QTest::addColumn<GLuint>("bit");
        QTest::addColumn<bool>("expected");

        QTest::newRow("bittest0") << GLfloat(42.0) << GLuint(0) << false;
        QTest::newRow("bittest1") << GLfloat(42.0) << GLuint(1) << true;
        QTest::newRow("bittest2") << GLfloat(42.0) << GLuint(2) << false;
        QTest::newRow("bittest3") << GLfloat(42.0) << GLuint(3) << true;
    }

} // namespace unit //

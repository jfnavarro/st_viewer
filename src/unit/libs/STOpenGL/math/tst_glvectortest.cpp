/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include <QtTest/QTest>

#include <math/GLVector.h>
Q_DECLARE_METATYPE(GL::GLpoint)
Q_DECLARE_METATYPE(GL::GLvector)

#include "tst_glvectortest.h"

namespace unit
{

    GLVectorTest::GLVectorTest(QObject *parent) : QObject(parent) { }

    void GLVectorTest::initTestCase()
	{
		QVERIFY2( true, "Empty" );
	}
    void GLVectorTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty" );
	}

    void GLVectorTest::testCompareEqual()
    {
        QFETCH(GL::GLvector, v0);
        QFETCH(GL::GLvector, v1);
        QFETCH(bool, expected);

        QCOMPARE( v0 == v1, expected );
    }

    void GLVectorTest::testCompareEqual_data()
    {
        QTest::addColumn<GL::GLvector>("v0");
        QTest::addColumn<GL::GLvector>("v1");
        QTest::addColumn<bool>("expected");

        GL::GLvector identity;
        GL::GLvector simple(1.0f, 2.0f);

        QTest::newRow("identity") << identity << identity << true;
        QTest::newRow("simple") << simple << simple   << true;
    }

    void GLVectorTest::testUnary()
    {
        QFETCH(GL::GLvector, v0);
        QFETCH(GL::GLvector, v1);
        QFETCH(bool, expected);

        QCOMPARE( (+v0) == v0, expected );
        QCOMPARE( (-v0) == v1, expected );
    }
    void GLVectorTest::testUnary_data()
    {
        QTest::addColumn<GL::GLvector>("v0");
        QTest::addColumn<GL::GLvector>("v1");
        QTest::addColumn<bool>("expected");

        GL::GLvector simple(1.0f, 2.0f);
        GL::GLvector minus_simple(-1.0f, -2.0f);

        QTest::newRow("simple") << simple << minus_simple << true;
    }

    void GLVectorTest::testAddSub()
    {
        QFETCH(GL::GLvector, v0);
        QFETCH(GL::GLvector, v1);
        QFETCH(GL::GLvector, v2);
        QFETCH(bool, expected);

        QCOMPARE( fuzzyEqual(v0 + v1, v2), expected );
        QCOMPARE( fuzzyEqual(v2 - v1, v0), expected );
    }
    void GLVectorTest::testAddSub_data()
    {
        QTest::addColumn<GL::GLvector>("v0");
        QTest::addColumn<GL::GLvector>("v1");
        QTest::addColumn<GL::GLvector>("v2");
        QTest::addColumn<bool>("expected");

        QTest::newRow("positive") << GL::GLvector( 1.0f,  2.0f) << GL::GLvector( 3.0f,  4.0f) << GL::GLvector( 4.0f,  6.0f) << true;
        QTest::newRow("negative") << GL::GLvector(-1.0f, -2.0f) << GL::GLvector(-3.0f, -4.0f) << GL::GLvector(-4.0f, -6.0f) << true;
        QTest::newRow("mixed") << GL::GLvector(-0.5f,  1.5f) << GL::GLvector(-0.2f, -0.5f) << GL::GLvector(-0.7f,  1.0f) << true;
    }

    void GLVectorTest::testMultDiv()
    {
        QFETCH(GL::GLvector, v0);
        QFETCH(GLfloat, s);
        QFETCH(GL::GLvector, v1);
        QFETCH(bool, expected);

        QCOMPARE( GL::fuzzyEqual(v0 * s, v1), expected );
        QCOMPARE( GL::fuzzyEqual(v1 / s, v0), expected );
    }
    void GLVectorTest::testMultDiv_data()
    {
        QTest::addColumn<GL::GLvector>("v0");
        QTest::addColumn<GLfloat>("s");
        QTest::addColumn<GL::GLvector>("v1");
        QTest::addColumn<bool>("expected");

        QTest::newRow("positive") << GL::GLvector( 1.0f,  2.0f) << (GLfloat)  2.0f << GL::GLvector(2.0f,  4.0f) << true;
        QTest::newRow("negative") << GL::GLvector(-1.0f, -2.0f) << (GLfloat) -0.5f << GL::GLvector(0.5f,  1.0f) << true;
        QTest::newRow("mixed") << GL::GLvector(-1.0f,  4.0f) << (GLfloat) -2.0f << GL::GLvector(2.0f, -8.0f) << true;
    }

    void GLVectorTest::testNormalLength()
    {
        QFETCH(GL::GLvector, v0);
        QFETCH(GL::GLvector, v1);
        QFETCH(GLfloat, s);
        QFETCH(bool, expected);

        QCOMPARE( GL::fuzzyEqual(v0.length(), s), expected );
        QCOMPARE( GL::fuzzyEqual(v0.normalize(), v1), expected );
    }
    void GLVectorTest::testNormalLength_data()
    {
        QTest::addColumn<GL::GLvector>("v0");
        QTest::addColumn<GL::GLvector>("v1");
        QTest::addColumn<GLfloat>("s");
        QTest::addColumn<bool>("expected");

        QTest::newRow("x-axis") << GL::GLvector( 4.0f, 0.0f) << GL::GLvector(1.0f, 0.0f) << (GLfloat) 4.0f << true;
        QTest::newRow("positive") << GL::GLvector( 1.0f, 1.0f) << GL::GLvector((GLfloat) M_SQRT1_2, (GLfloat) M_SQRT1_2) << (GLfloat) M_SQRT2 << true;
        QTest::newRow("mixed") << GL::GLvector(-1.0f, 1.0f) << GL::GLvector((GLfloat)-M_SQRT1_2, (GLfloat) M_SQRT1_2) << (GLfloat) M_SQRT2 << true;
    }

    void GLVectorTest::testQuadrant()
    {
        QFETCH(GL::GLvector, v);
        QFETCH(GLuint, i);
        QFETCH(bool, expected);

        QCOMPARE( (v.quadrant() == i), expected );
    }
    void GLVectorTest::testQuadrant_data()
    {
        QTest::addColumn<GL::GLvector>("v");
        QTest::addColumn<GLuint>("i");
        QTest::addColumn<bool>("expected");

        QTest::newRow("quadrant0") << GL::GLvector( 1.0f, 1.0f) << GLuint(0u) << true;
        QTest::newRow("quadrant1") << GL::GLvector(-1.0f, 1.0f) << GLuint(1u) << true;
        QTest::newRow("quadrant2") << GL::GLvector(-1.0f,-1.0f) << GLuint(2u) << true;
        QTest::newRow("quadrant3") << GL::GLvector( 1.0f,-1.0f) << GLuint(3u) << true;
        QTest::newRow("border0") << GL::GLvector( 1.0f, 0.0f) << GLuint(0u) << true;
        QTest::newRow("border1") << GL::GLvector( 0.0f, 1.0f) << GLuint(0u) << true;
        QTest::newRow("border2") << GL::GLvector(-1.0f, 0.0f) << GLuint(1u) << true;
        QTest::newRow("border3") << GL::GLvector( 0.0f,-1.0f) << GLuint(3u) << true;
        QTest::newRow("zero") << GL::GLvector( 0.0f, 0.0f) << GLuint(0u) << true;
    }

    void GLVectorTest::testFromPoint()
    {
        QFETCH(GL::GLpoint, p0);
        QFETCH(GL::GLpoint, p1);
        QFETCH(GL::GLvector, v0);
        QFETCH(bool, expected);

        QCOMPARE( GL::fuzzyEqual( GL::GLvector::fromPoints(p0, p1), v0), expected );
    }
    void GLVectorTest::testFromPoint_data()
    {
        QTest::addColumn<GL::GLpoint>("p0");
        QTest::addColumn<GL::GLpoint>("p1");
        QTest::addColumn<GL::GLvector>("v0");
        QTest::addColumn<bool>("expected");

        QTest::newRow("positive") << GL::GLpoint( 1.0f, 1.0f) << GL::GLpoint(3.0f,  2.0f) << GL::GLvector(2.0f,  1.0f) << true;
        QTest::newRow("mixed") << GL::GLpoint(-1.0f, 1.0f) << GL::GLpoint(3.0f, -2.0f) << GL::GLvector(4.0f, -3.0f) << true;
    }

} // namespace unit //

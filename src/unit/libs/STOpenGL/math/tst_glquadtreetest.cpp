/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>
#include <QtCore/QTime>
#include <util/random.h>

#include <math/GLQuadTree.h>
Q_DECLARE_METATYPE(QList<GL::GLpoint>)

#include "tst_glquadtreetest.h"

namespace unit
{

    GLQuadTreeTest::GLQuadTreeTest(QObject *parent) : QObject(parent) { }

    void GLQuadTreeTest::initTestCase()
	{
		QVERIFY2( true, "Empty");
	}
    void GLQuadTreeTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty");
	}

    void GLQuadTreeTest::testInsert()
    {
        QFETCH(PointList, points);
        QFETCH(GLint, buckets);
        QFETCH(bool, expected);

        TestQuadTree quadTree(GL::GLpoint(8.0f, 8.0f));

        int size = points.size();
        for (int i=0; i<size; ++i)
        {
            QVERIFY2(quadTree.insert(points[i], i), "Unable to insert point!");
        }

        QCOMPARE( (quadTree.buckets() == buckets), expected );
    }
    void GLQuadTreeTest::testInsert_data()
    {
        QTest::addColumn<PointList>("points");
        QTest::addColumn<GLint>("buckets");
        QTest::addColumn<bool>("expected");

        const GL::GLpoint p[] = {
            GL::GLpoint(1.0f, 1.0f), GL::GLpoint(3.0f, 1.0f), GL::GLpoint(5.0f, 1.0f), GL::GLpoint(7.0f, 1.0f),
            GL::GLpoint(1.0f, 3.0f), GL::GLpoint(3.0f, 3.0f), GL::GLpoint(5.0f, 3.0f), GL::GLpoint(7.0f, 3.0f),
            GL::GLpoint(1.0f, 5.0f), GL::GLpoint(3.0f, 5.0f), GL::GLpoint(5.0f, 5.0f), GL::GLpoint(7.0f, 5.0f),
            GL::GLpoint(1.0f, 7.0f), GL::GLpoint(3.0f, 7.0f), GL::GLpoint(5.0f, 7.0f), GL::GLpoint(7.0f, 7.0f),

            GL::GLpoint(0.5f, 0.5f),
            GL::GLpoint(7.5f, 0.5f)
        };

        //NOTE the point list is constructed incrementally and a copy is made of the point list for each row
        PointList points;
        QTest::newRow("empty") << (points) << 1 << true;
        QTest::newRow("simple0") << (points << p[0] << p[2] << p[8] << p[10]) << 1 << true;
        QTest::newRow("simple1") << (points << p[1] << p[4] << p[5]) << 5 << true;
        QTest::newRow("simple2") << (points << p[3] << p[6] << p[7]) << 5 << true;
        QTest::newRow("simple3") << (points << p[16]) << 9 << true;
        QTest::newRow("simple4") << (points << p[17]) << 13 << true;
    }

} // namespace unit //
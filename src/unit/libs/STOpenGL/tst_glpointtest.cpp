/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include <GLCommon.h>
Q_DECLARE_METATYPE(GL::GLpoint)

#include "tst_glpointtest.h"

namespace unit
{

    GLPointTest::GLPointTest(QObject *parent) : QObject(parent) { }

    void GLPointTest::initTestCase()
	{
		QVERIFY2( true, "Empty");
	}
    void GLPointTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty");
	}

} // namespace unit //

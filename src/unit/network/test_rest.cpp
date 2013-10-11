/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "test_rest.h"

#include <QtTest/QTest>

namespace unit
{
    
    RestTest::RestTest(QObject *parent) : QObject(parent) { }
    
    void RestTest::initTestCase()
    {
        QVERIFY(true);
    }
    
    void RestTest::cleanupTestCase()
    {   
        
        
    }
    
    
} // namespace unit //

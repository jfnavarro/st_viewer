/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "test_auth.h"

#include <QtTest/QTest>

namespace unit
{
    
    AuthTest::AuthTest(QObject *parent) : QObject(parent) { }
    
    void AuthTest::initTestCase()
    {
        QVERIFY(true);
    }
    
    void AuthTest::cleanupTestCase()
    {   

        
    }
    
    
} // namespace unit //

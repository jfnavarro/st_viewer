/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

// unit test lib
#include "QtUnitTest/testsuite.h"

// unit tests
#include "test/controller/data/tst_datastoretest.h"
#include "test/controller/tst_widgets.h"

#include "test/simpleCrypt/tst_simplecryptdevicetest.h"
#include "test/simpleCrypt/tst_simplecrypttest.h"

#include "test/math/tst_glaabbtest.h"
#include "test/math/tst_glquadtreetest.h"
#include "test/math/tst_glheatmaptest.h"

#include "test/model/tst_objectparsertest.h"

#include "test/utils/tst_mathextendedtest.h"

#include "test/network/test_auth.h"
#include "test/network/test_rest.h"

using namespace unit;

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    TestSuite suite;

    suite.addTest(new DataStoreTest, "DataStore");
    //suite.addTest(new WidgetsTest, "Widgets");
    suite.addTest(new GLAABBTest, "GLAABB");
    suite.addTest(new GLQuadTreeTest, "GLQuadTree").dependsOn("GLAABB");
    suite.addTest(new GLHeatMapTest, "GLHeatMap");
    suite.addTest(new ObjectParserTest, "ObjectParser");
    suite.addTest(new MathExtendedTest, "MathExtended");
    suite.addTest(new SimpleCryptTest, "SimpleCrypt");
    suite.addTest(new SimpleCryptDeviceTest, "SimpleCryptDevice").dependsOn("SimpleCrypt");
    suite.addTest(new AuthTest, "Authorization");
    suite.addTest(new RestTest, "REST Services");

    return suite.exec();
}

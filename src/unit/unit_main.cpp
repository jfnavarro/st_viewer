/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

// unit test lib
#include "testsuite.h"

// unit tests
#include "unit/controller/data/tst_datastoretest.h"
#include "unit/controller/tst_widgets.h"

#include "unit/libs/STCore/tst_simplecryptdevicetest.h"
#include "unit/libs/STCore/tst_simplecrypttest.h"

#include "unit/libs/STCoreWidgets/tst_lineeditcleartest.h"

#include "unit/libs/STOpenGL/compression/tst_glcolortest.h"
#include "unit/libs/STOpenGL/math/tst_glaabbtest.h"
#include "unit/libs/STOpenGL/math/tst_glfloattest.h"
#include "unit/libs/STOpenGL/math/tst_glmathtest.h"
#include "unit/libs/STOpenGL/math/tst_glmatrixtest.h"
#include "unit/libs/STOpenGL/math/tst_glquadtreetest.h"
#include "unit/libs/STOpenGL/math/tst_glvectortest.h"
#include "unit/libs/STOpenGL/render/tst_glheatmaptest.h"
#include "unit/libs/STOpenGL/tst_glcommontest.h"
#include "unit/libs/STOpenGL/tst_glpointtest.h"
#include "unit/libs/STOpenGL/tst_gltypetraitstest.h"

#include "unit/model/tst_objectparsertest.h"

#include "unit/utils/tst_mathextendedtest.h"

#include "unit/network/test_auth.h"
#include "unit/network/test_rest.h"

using namespace unit;

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    //QApplication::setApplicationName("stVi_unittest");

    TestSuite suite;

    suite.addTest(new DataStoreTest, "DataStore");
    //suite.addTest(new WidgetsTest, "Widgets");

    suite.addTest(new GLTypeTraitsTest, "GLTypeTraits");
    suite.addTest(new GLCommonTest, "GLCommon");
    suite.addTest(new GLColorTest, "GLColor").dependsOn("GLCommon");
    suite.addTest(new GLFloatTest, "GLFloat").dependsOn("GLCommon");
    suite.addTest(new GLMathTest, "GLMath").dependsOn("GLCommon");
    suite.addTest(new GLAABBTest, "GLAABB").dependsOn("GLMath");
    suite.addTest(new GLQuadTreeTest, "GLQuadTree").dependsOn("GLAABB");
    suite.addTest(new GLMatrixTest, "GLMatrix").dependsOn("GLMath");
    suite.addTest(new GLPointTest, "GLPoint").dependsOn("GLMath");
    suite.addTest(new GLVectorTest, "GLVector").dependsOn("GLPoint");
    suite.addTest(new GLHeatMapTest, "GLHeatMap").dependsOn("GLPoint");

    suite.addTest(new ObjectParserTest, "ObjectParser");

    suite.addTest(new MathExtendedTest, "MathExtended");

    suite.addTest(new SimpleCryptTest, "SimpleCrypt");
    suite.addTest(new SimpleCryptDeviceTest, "SimpleCryptDevice").dependsOn("SimpleCrypt");

    suite.addTest(new AuthTest, "Authorization");
    suite.addTest(new RestTest, "REST Services");
    
    //TODO init application before testing gui objects
    //suite.addTest(new LineEditClearTest, "LineEditClear");

    return suite.exec();
}

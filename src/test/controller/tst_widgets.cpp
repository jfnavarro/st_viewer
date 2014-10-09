/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "config/Configuration.h"
#include "auth/AuthorizationManager.h"
#include "data/DataProxy.h"
#include "network/NetworkManager.h"

#include <QMainWindow>
#include <QApplication>
#include "stVi.h"

#include "tst_widgets.h"

#include <QtTest/QTest>

namespace unit
{

WidgetsTest::WidgetsTest(QObject *parent) : QObject(parent) { }

void WidgetsTest::initTestCase()
{
    // these must not go out of scope
    static int argc = 0;
    static char *argv = 0;

    //test we can create mainwidgets and main application
    QApplication *app = new QApplication(argc, &argv);
    QVERIFY(app != 0);

    //stVi *mainWindow = new stVi();
    //QVERIFY(mainWindow != 0);

    // init configurations
    Configuration *configuration = new Configuration();
    QVERIFY(configuration != 0);

    // init network manager
    NetworkManager* networkManager = new NetworkManager();
    QVERIFY(networkManager != 0);

    // init data proxy
    DataProxy* dataProxy = new DataProxy();
    QVERIFY(dataProxy != 0);

    // auth manager
    AuthorizationManager *authManager = new AuthorizationManager(dataProxy);
    QVERIFY(authManager != 0);
}

void WidgetsTest::cleanupTestCase()
{
    //TODO
}


} // namespace unit //

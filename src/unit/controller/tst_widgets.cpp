/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "utils/config/Configuration.h"
#include "controller/auth/AuthorizationManager.h"
#include "controller/data/DataProxy.h"
#include "controller/error/ErrorManager.h"
#include "controller/network/NetworkManager.h"

#include <QMainWindow>
#include <QApplication>

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

    QMainWindow *mainWindow = new QMainWindow();
    QVERIFY(mainWindow != 0);

    // init authorization //NOTE we cannot include this in the test for now
    //         AuthorizationManager *authManager = AuthorizationManager::getInstance();
    //         authManager->init();
    //         QVERIFY(authManager != 0);

    // init configurations
    Configuration *configuration = Configuration::getInstance();
    configuration->init();
    QVERIFY(configuration != 0);

    // init error manager
    ErrorManager* errorManager = ErrorManager::getInstance();
    errorManager->init();
    QVERIFY(errorManager != 0);

    // init network manager
    NetworkManager* networkManager = NetworkManager::getInstance();
    networkManager->init();
    QVERIFY(networkManager != 0);

    // init data proxy
    DataProxy* dataProxy = DataProxy::getInstance();
    dataProxy->init();
    QVERIFY(dataProxy != 0);
}

void WidgetsTest::cleanupTestCase()
{
    // finalize authentication manager
    AuthorizationManager::getInstance()->finalize();
    AuthorizationManager *authManager = AuthorizationManager::getInstance(true);
    QVERIFY(authManager == 0);

    // finalize network manager
    NetworkManager::getInstance()->finalize();
    NetworkManager *networkManager = NetworkManager::getInstance(true);
    QVERIFY(networkManager == 0);

    // finalize data proxy
    DataProxy::getInstance()->finalize();
    DataProxy *dataProxy = DataProxy::getInstance(true);
    QVERIFY(dataProxy == 0);

    // finalize error manager
    ErrorManager::getInstance()->finalize();
    ErrorManager *errorManager = ErrorManager::getInstance(true);
    QVERIFY(errorManager == 0);

    // finalize configurations
    Configuration::getInstance()->finalize();
    Configuration *configuration = Configuration::getInstance(true);
    QVERIFY(configuration == 0);

}


} // namespace unit //

#include "config/Configuration.h"
#include "auth/AuthorizationManager.h"
#include "data/DataProxy.h"
#include "network/NetworkManager.h"

#include <QMainWindow>
#include <QApplication>
#include "mainWindow.h"

#include "tst_widgets.h"

#include <QtTest/QTest>

namespace unit
{

WidgetsTest::WidgetsTest(QObject *parent)
    : QObject(parent)
{
}

void WidgetsTest::initTestCase()
{
    // init configurations
    Configuration *configuration = new Configuration();
    QVERIFY(configuration != 0);

    // init network manager
    NetworkManager *networkManager = new NetworkManager();
    QVERIFY(networkManager != 0);

    // init data proxy
    QSharedPointer<DataProxy> dataProxy = QSharedPointer<DataProxy>(new DataProxy());
    QVERIFY(dataProxy != 0);

    // auth manager
    AuthorizationManager *authManager = new AuthorizationManager(dataProxy);
    QVERIFY(authManager != 0);
}

void WidgetsTest::cleanupTestCase()
{
    // TODO
}

void WidgetsTest::testDoesNothing()
{
    // This test is a dummy test and always passes. Without it QTest reports use of the
    // test-free WidgetsTest as a failure.
    QVERIFY(true);
}

} // namespace unit

QTEST_MAIN(unit::WidgetsTest)
#include "tst_widgets.moc"

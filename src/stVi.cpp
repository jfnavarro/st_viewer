/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "stVi.h"

#if defined Q_OS_WIN
#include <windows.h>
#include "qt_windows.h"
#endif

#include <QShortcut>
#include <QCloseEvent>

#include "options_cmake.h"

#include <QTranslator>
#include <QMessageBox>
#include <QDebug>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QApplication>
#include <QGLFormat>
#include <QSslSocket>
#include <QJsonParseError>
#include <QJsonDocument>

#include "utils/DebugHelper.h"
#include "utils/Utils.h"
#include "utils/config/Configuration.h"

#include "controller/auth/AuthorizationManager.h"
#include "controller/error/Error.h"
#include "controller/error/ApplicationError.h"
#include "controller/error/ErrorManager.h"
#include "controller/error/ServerError.h"
#include "controller/network/RESTCommandFactory.h"
#include "controller/network/NetworkManager.h"
#include "controller/network/NetworkReply.h"
#include "controller/network/NetworkCommand.h"
#include "controller/data/DataProxy.h"
#include "controller/data/DataStore.h"

#include "view/AboutDialog.h"
#include "MainMenuBar.h"
#include "ExtendedTabWidget.h"
#include "MainStatusBar.h"

stVi::stVi(QWidget* parent): QMainWindow(parent)
{
    actionAbout = 0;
    actionClear_Cache = 0;
    actionExit = 0;
    actionHelp = 0;
    actionPrint = 0;
    actionVersion = 0;
    menubar = 0;
    statusbar = 0;
    centralwidget = 0;
    mainlayout = 0;
    mainTab = 0;
    menuLoad = 0;
    menuHelp = 0;

    //init single instances (this must be done the very very first)       
    initSingleInstances();
}

stVi::~stVi()
{
    finalizeSingleInstances();
}

void stVi::init()
{
    DEBUG_FUNC_NAME
    
    // init style, size and icons
    initStyle();
    
    // create ui widgets
    setupUi();
    
    // create keyboard shortcuts
    createShorcuts();
    
    // lets create some stuff
    createLayouts();
    
    // connections
    createConnections();
    
    // restore settings
    loadSettings();
}

bool stVi::checkSystemRequirements() const
{
    // Test for Basic OpenGL Support
    if (!QGLFormat::hasOpenGL()) {
        QMessageBox::information(0, "OpenGL 2.x Support",
                                 "This system does not support OpenGL.");
        return false;
    }
    // Fail if you do not have OpenGL 2.0 or higher driver
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_2_1) {
        QMessageBox::information(0, "OpenGL 2.x Context",
                                 "This system does not support OpenGL 2.x Contexts");
        return false;
    }
    // Fail if you do not support SSL secure connection
    if (!QSslSocket::supportsSsl()) {
        QMessageBox::information(0, "HTTPS",
                                 "This system does not secure SSL connections");
        return 0;
    }

    //TODO move this network call to dataproxy and add a specific object parser
    
    // check if the version is supported in the server and check for updates
    NetworkCommand *cmd = RESTCommandFactory::getMinVersion();
    NetworkManager *nm = NetworkManager::getInstance();
    NetworkReply *reply = nm->httpRequest(cmd, QVariant(QVariant::Invalid), NetworkManager::Empty);
    
    QEventLoop loop; // I want to wait until this finishes
    connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), &loop, SLOT(quit()));
    loop.exec();
    
    cmd->deleteLater();
    if (reply == 0) {
        QMessageBox::information(0, "MINIMUM VERSION",
                                 "Required version could not be retrieved from the server, try again");
        return false;
    }
    
    //parse reply
    QJsonDocument document = reply->getJSON();
    // if no errors
    if (!reply->hasErrors()) {
        QString min_version = document.toVariant().toMap().find("minSupportedClientVersion").value().toString();
        QString myversion = Globals::VERSION;
        float version_major = Globals::MAJOR.toFloat();
        float version_minor = Globals::MINOR.toFloat();
        float my_version_major = min_version.split(".").at(0).toFloat();
        float my_version_minor = min_version.split(".").at(1).toFloat();
        qDebug() << "[MAIN] Check min version min = " << min_version << " current = " << myversion;
        //TODO I should check that I retrieved the versions correctly
        reply->deleteLater();
        if (my_version_major > version_major || my_version_minor > version_minor) {
            QMessageBox::information(0, "MINIMUM VERSION",
                                     "This version of the software is not supported anymore, please update!");
            return false;
        }
    } else {
        qDebug() << "[MAIN] Network ERROR : Check min version min " << reply->getText();
        QMessageBox::information(0, "MINIMUM VERSION",
                                 "Required version could not be retrieved from the server, try again");

        reply->deleteLater();
        return false;
    }

    return true;
}

void stVi::setupUi()
{
    setObjectName(QStringLiteral("stVi"));
    setWindowModality(Qt::NonModal);
    resize(1217, 706);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());

    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(1024, 768));
    setMouseTracking(false);
    setFocusPolicy(Qt::NoFocus);
    setWindowIcon(QIcon(QStringLiteral(":/images/st_icon.png")));

    actionExit = new QAction(this);
    actionHelp = new QAction(this);
    actionVersion = new QAction(this);
    actionPrint = new QAction(this);
    actionPrint->setIcon(QIcon(QStringLiteral(":/images/print-printer-icone-7769-64.png")));
    actionAbout = new QAction(this);
    actionClear_Cache = new QAction(this);

    centralwidget = new QWidget(this);
    centralwidget->setMouseTracking(false);
    centralwidget->setFocusPolicy(Qt::NoFocus);

    mainlayout = new QVBoxLayout(centralwidget);
    mainTab = new ExtendedTabWidget(centralwidget);
    mainlayout->addWidget(mainTab);
    setCentralWidget(centralwidget);

    statusbar = MainStatusBar::getInstance();
    setStatusBar(statusbar);

    menubar = MainMenuBar::getInstance();
    menubar->setNativeMenuBar(true);
    menubar->setGeometry(QRect(0, 0, 1217, 22));

    menuLoad = new QMenu(menubar);
    menuHelp = new QMenu(menubar);

    setMenuBar(menubar);

    menubar->addAction(menuLoad->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menuLoad->addSeparator();
    menuLoad->addAction(actionExit);
    menuLoad->addAction(actionClear_Cache);
    menuHelp->addAction(actionAbout);

    actionExit->setText(QApplication::translate("MainWindow", "&Exit", 0));
    actionHelp->setText(QApplication::translate("MainWindow", "Help", 0));
    actionVersion->setText(QApplication::translate("MainWindow", "Version", 0));
    actionPrint->setText(QApplication::translate("MainWindow", "Print", 0));
    actionAbout->setText(QApplication::translate("MainWindow", "&About...", 0));
    actionClear_Cache->setText(QApplication::translate("MainWindow", "Clear Cache", 0));
    menuLoad->setTitle(QApplication::translate("MainWindow", "File", 0));
    menuHelp->setTitle(QApplication::translate("MainWindow", "About", 0));

    QMetaObject::connectSlotsByName(this);
}

void stVi::handleMessage(QString message)
{
    emit signalError(new Error("System Error", message));
}

void stVi::showAbout()
{
    QScopedPointer<AboutDialog> about(new AboutDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    about->exec();
}

void stVi::slotExit()
{
    int answer = QMessageBox::warning(
                     this, tr("Exit application"),
                     tr("Are you really sure you want to exit now?"),
                     QMessageBox::No | QMessageBox::Escape, QMessageBox::Yes | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        qDebug() << "[stVi] Info: Exitting the application...";
        saveSettings();
        QApplication::exit();
#if defined Q_OS_LINUX || defined Q_OS_WIN
        QApplication::processEvents(); //TOFIX this hides the mainwindow on MAC platforms
#endif
    }
}

void stVi::slotClearCache()
{
    int answer = QMessageBox::warning(
                     this, tr("Clear the Cache"),
                     tr("Are you really sure you want to clear the cache?"),
                     QMessageBox::No | QMessageBox::Escape, QMessageBox::Yes | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        qDebug() << "[stVi] Info: Cleaaring the cache...";
        DataProxy::getInstance()->cleanAll();
        mainTab->resetStatus();
    }
}

void stVi::createLayouts()
{
    statusBar()->showMessage("Spatial Transcriptomics Viewer");
}

void stVi::initStyle()
{
    // apply stylesheet and configurations
#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_MacPluginApplication, false);
    QApplication::setAttribute(Qt::AA_NativeWindows, true); //NOTE this is actually pretty important
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false); //osx does not show icons on menus
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint)); // no close icon on MAC
#endif

    //TODO move to styleshee.css file
    setStyleSheet("QToolBar {border-bottom: 1px white; border-top: 1px white;}"
                  "QToolButton:checked {background-color: rgb(175,175,175);}QToolButton{background-color: transparent;}"
                  "QToolButton:hover {background-color: rgb(175,175,175);}"
                  "QLineEdit {border: 1px solid gray;background: white;selection-background-color: darkgray;}"
                  "QTableView {background-color: transparent;}"
                  "QHeaderView::section {background-color: rgb(176,196,222);}"
                  "QHeaderView {background-color: rgb(176,196,222);;}"
                  "QTableCornerButton::section{background-color: transparent;}");

    // apply font
    QFont font;
    font.setFamily(font.defaultFamily());
    setFont(font);
}

void stVi::createShorcuts()
{
#if defined(Q_OS_WIN)
    actionExit->setShortcuts(QList<QKeySequence>()
                             << QKeySequence(Qt::ALT | Qt::Key_F4)
                             << QKeySequence(Qt::CTRL | Qt::Key_Q));
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    actionExit->setShortcut(QKeySequence::Quit);
#endif

#if defined Q_OS_MAC
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+M"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(showMinimized()));
    actionExit->setShortcut(QKeySequence("Ctrl+W"));
#endif
}

void stVi::initSingleInstances()
{
    DEBUG_FUNC_NAME

    // init configurations
    Configuration *configuration = Configuration::getInstance();
    configuration->init();

    // init error manager
    ErrorManager* errorManager = ErrorManager::getInstance();
    connect(this, SIGNAL(signalError(Error*)), errorManager, SLOT(slotHandleError(Error*)));
    errorManager->init(this);

    // init data stored
    DataStore* dataStore = DataStore::getInstance();
    dataStore->init();

    // init network manager
    NetworkManager* networkManager = NetworkManager::getInstance();
    networkManager->init();

    // init data proxy
    DataProxy* dataProxy = DataProxy::getInstance();
    dataProxy->init();

    // init MenuBar
    MainMenuBar *menubar = MainMenuBar::getInstance(this);
    menubar->init();

    // init Status Bar
    MainStatusBar *statusbar = MainStatusBar::getInstance(this);
    statusbar->init();

    // inir AuthorizationManager
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    auth->init();
}

void stVi::finalizeSingleInstances()
{
    DEBUG_FUNC_NAME

    // finalize authentication manager
    AuthorizationManager::getInstance()->finalize();
    AuthorizationManager::getInstance(true);

    // finalize network manager
    NetworkManager::getInstance()->finalize();
    NetworkManager::getInstance(true);

    // finalize data proxy
    DataProxy::getInstance()->finalize();
    DataProxy::getInstance(true);

    // finalize data stored
    DataStore::getInstance()->finalize();
    DataStore::getInstance(true);

    // finalize error manager
    ErrorManager::getInstance()->finalize();
    ErrorManager::getInstance(true);

    // finalize configurations
    Configuration::getInstance()->finalize();
    Configuration::getInstance(true);

    // finalize menubar
    MainMenuBar::getInstance()->finalize();
    MainMenuBar::getInstance(true);

    // finalize status bar
    MainStatusBar::getInstance()->finalize();
    MainStatusBar::getInstance(true);
}

void stVi::createConnections()
{
    //exit and print action
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(slotExit()));
    //connect error signals here from child components
    connect(mainTab, SIGNAL(signalError(Error*)), this, SIGNAL(signalError(Error*)));
    //clear cache action
    connect(actionClear_Cache, SIGNAL(triggered(bool)), this, SLOT(slotClearCache()));
    //signal that shows the about dialog
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void stVi::closeEvent(QCloseEvent* event)
{
    event->ignore();
    slotExit();
}

void stVi::loadSettings()
{
    QSettings settings;
    // Retrieve the geometry and state of the main window
    restoreGeometry(settings.value(Globals::SettingsGeometry).toByteArray());
    restoreState(settings.value(Globals::SettingsState).toByteArray());
    //TODO load global settings (menus and status)
}

void stVi::saveSettings() const
{
    QSettings settings;
    // Keep track of the geometry and state of the main window
    QByteArray geometry = saveGeometry();
    settings.setValue(Globals::SettingsGeometry, geometry);
    QByteArray state = saveState();
    // save settings
    settings.setValue(Globals::SettingsState, state);
    //TODO save global settings (menus and status)
}

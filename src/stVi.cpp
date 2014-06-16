/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "stVi.h"

#if defined Q_OS_WIN

#define NOMINMAX
#include <windows.h>
#include "qt_windows.h"
#endif

#include "options_cmake.h"

#include <QShortcut>
#include <QCloseEvent>
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
#include <QMenuBar>
#include <QStatusBar>

#include "utils/Utils.h"
#include "config/Configuration.h"
#include "auth/AuthorizationManager.h"

#include "error/Error.h"
#include "error/ApplicationError.h"
#include "error/ServerError.h"

#include "network/RESTCommandFactory.h"
#include "network/NetworkManager.h"
#include "network/NetworkReply.h"
#include "network/NetworkCommand.h"

#include "data/DataProxy.h"
#include "data/DataStore.h"

#include "dataModel/MinVersionDTO.h"
#include "data/ObjectParser.h"

#include "dialogs/AboutDialog.h"

#include "viewPages/ExtendedTabWidget.h"

namespace {

 bool versionIsGreaterOrEqual(const std::array< qulonglong, 3> &version1,
                                    const std::array< qulonglong, 3> &version2)
{
    int index = 0;
    for(const auto &num : version1) {
        if (num > version2[index]) {
            return true;
        }
        if (num < version2[index]) {
            return false;
        }
        ++index;
    }
    return true;
}

}

stVi::stVi(QWidget* parent): QMainWindow(parent),
    actionExit(nullptr),
    actionHelp(nullptr),
    actionVersion(nullptr),
    actionAbout(nullptr),
    actionClear_Cache(nullptr),
    menuLoad(nullptr),
    menuHelp(nullptr),
    centralwidget(nullptr),
    mainlayout(nullptr),
    mainTab(nullptr)
{
    //init single instances (this must be done the very very first)       
    initSingleInstances();
}

stVi::~stVi()
{
    finalizeSingleInstances();
}

void stVi::init()
{
        
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
        QMessageBox::critical(this->centralWidget(), "OpenGL 2.x Support",
                                 "This system does not support OpenGL.");
        return false;
    }
    // Fail if you do not have OpenGL 2.0 or higher driver
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_2_1) {
        QMessageBox::critical(this->centralWidget(), "OpenGL 2.x Context",
                                 "This system does not support OpenGL 2.x Contexts");
        return false;
    }
    // Fail if you do not support SSL secure connection
    if (!QSslSocket::supportsSsl()) {
        QMessageBox::critical(this->centralWidget(), "HTTPS",
                                 "This system does not secure SSL connections");
        return false;
    }

    // check if the version is supported in the server and check for updates
    NetworkCommand *cmd = RESTCommandFactory::getMinVersion();
    NetworkManager nm;
    NetworkReply *reply =
            nm.httpRequest(cmd, QVariant(QVariant::Invalid), NetworkManager::Empty);
    QEventLoop loop; // I want to wait until this finishes
    connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), &loop, SLOT(quit()));
    loop.exec();
    
    cmd->deleteLater();
    if (reply == nullptr) {
        QMessageBox::critical(this->centralWidget(), "MINIMUM VERSION",
                                 "Required version could not be retrieved from the server, try again");
        return false;
    }
    
    //parse the reply
    const QJsonDocument document = reply->getJSON();
    const QVariant result = document.toVariant();
    reply->deleteLater();

    // if no errors
    if (!reply->hasErrors()) {
        MinVersionDTO dto;
        data::ObjectParser::parseObject(result, &dto);
        qDebug() << "[stVi] Check min version min = "
                 << dto.minSupportedVersion() << " current = " << Globals::VERSION;
        if (!versionIsGreaterOrEqual(Globals::VersionNumbers,
                                     dto.minVersionAsNumber())) {
            QMessageBox::critical(this->centralWidget(), "MINIMUM VERSION",
                                     "This version of the software is not supported anymore, please update!");
            return false;
        }
    } else {
        QMessageBox::critical(this->centralWidget(), "MINIMUM VERSION",
                                 "Required version could not be retrieved from the server, try again");
        return false;
    }

    return true;
}

void stVi::setupUi()
{
    setObjectName(QStringLiteral("stVi"));
    setWindowModality(Qt::NonModal);
    resize(1024, 768);
    setMinimumSize(QSize(1024, 768));
    setWindowIcon(QIcon(QStringLiteral(":/images/st_icon.png")));

    actionExit = new QAction(this);
    actionHelp = new QAction(this);
    actionVersion = new QAction(this);
    actionAbout = new QAction(this);
    actionClear_Cache = new QAction(this);

    centralwidget = new QWidget(this);

    mainlayout = new QVBoxLayout(centralwidget);
    mainTab = new ExtendedTabWidget(centralwidget);
    mainlayout->addWidget(mainTab);
    setCentralWidget(centralwidget);

    QStatusBar *statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    QMenuBar *menubar = new QMenuBar(this);
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
    actionAbout->setText(QApplication::translate("MainWindow", "&About...", 0));
    actionClear_Cache->setText(QApplication::translate("MainWindow", "Clear Cache", 0));
    menuLoad->setTitle(QApplication::translate("MainWindow", "File", 0));
    menuHelp->setTitle(QApplication::translate("MainWindow", "About", 0));

    QMetaObject::connectSlotsByName(this);
}

void stVi::handleMessage(const QString &message)
{
    QMessageBox::critical(this->centralWidget(), "Main Application", message);
}

void stVi::showAbout()
{
    QScopedPointer<AboutDialog> about(new AboutDialog(this,
                                                      Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    about->exec();
}

void stVi::slotExit()
{
    const int answer = QMessageBox::warning(
                     this, tr("Exit application"),
                     tr("Are you really sure you want to exit now?"),
                     QMessageBox::No | QMessageBox::Escape,
                QMessageBox::Yes | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        qDebug() << "[stVi] Info: Exitting the application...";
        saveSettings();
        QApplication::exit();
#if defined Q_OS_LINUX || defined Q_OS_WIN
        // this hides the mainwindow on MAC platforms
        // TODO : this bug is fixed in qt 5.3.1 (test and validate)
        QApplication::processEvents();
#endif
    }
}

void stVi::slotClearCache()
{
    const int answer = QMessageBox::warning(
                     this, tr("Clear the Cache"),
                     tr("Are you really sure you want to clear the cache?"),
                     QMessageBox::No | QMessageBox::Escape,
                QMessageBox::Yes | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        qDebug() << "[stVi] Info: Cleaaring the cache...";
        DataProxy::getInstance()->cleanAll();
        mainTab->resetStatus();
    }
}

void stVi::createLayouts()
{
    statusBar()->showMessage("Spatial Transcriptomics Viewer");
    //TODO make several status bar updates in different parts of the application
}

void stVi::initStyle()
{
    // apply stylesheet and configurations
#ifdef Q_OS_MAC
    setUnifiedTitleAndToolBarOnMac(true);
    QApplication::setAttribute(Qt::AA_MacPluginApplication, false);
    //NOTE this is actually pretty important
    QApplication::setAttribute(Qt::AA_NativeWindows, true);
    //osx does not show icons on menus
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
     // no close icon on MAC
    setWindowFlags(((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
#endif

    //TODO move to styleshee.css file
    setStyleSheet("QToolBar {border-bottom: 1px white; border-top: 1px white;}"
                  "QToolButton:checked {background-color: rgb(175,175,175);}"
                  "QToolButton{background-color: transparent;}"
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
    // init data proxy
    DataProxy* dataProxy = DataProxy::getInstance();
    dataProxy->init();

    // inir AuthorizationManager
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    auth->init();
}

void stVi::finalizeSingleInstances()
{
    // finalize authentication manager
    AuthorizationManager::getInstance()->finalize();
    AuthorizationManager::getInstance(true);

    // finalize data proxy
    DataProxy::getInstance()->finalize();
    DataProxy::getInstance(true);
}

void stVi::createConnections()
{
    //exit and print action
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(slotExit()));
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

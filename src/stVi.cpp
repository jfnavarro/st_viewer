
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
#include <QMenuBar>
#include <QStatusBar>

#include "utils/Utils.h"

#include "error/Error.h"
#include "error/ApplicationError.h"
#include "error/ServerError.h"

#include "network/DownloadManager.h"

#include "data/DataProxy.h"

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
    m_actionExit(nullptr),
    m_actionHelp(nullptr),
    m_actionVersion(nullptr),
    m_actionAbout(nullptr),
    m_actionClear_Cache(nullptr),
    m_menuLoad(nullptr),
    m_menuHelp(nullptr),
    m_centralwidget(nullptr),
    m_mainlayout(nullptr),
    m_mainTab(nullptr),
    m_dataProxy(nullptr)
{
    m_dataProxy = new DataProxy();
}

stVi::~stVi()
{
    if (!m_dataProxy.isNull()) {
        delete m_dataProxy;
    }
    m_dataProxy = nullptr;

    m_actionExit->deleteLater();
    m_actionExit = nullptr;

    m_actionHelp->deleteLater();
    m_actionHelp = nullptr;

    m_actionVersion->deleteLater();
    m_actionVersion = nullptr;

    m_actionAbout->deleteLater();
    m_actionAbout = nullptr;

    m_menuLoad->deleteLater();
    m_menuLoad = nullptr;

    m_menuHelp->deleteLater();
    m_menuHelp = nullptr;

    m_centralwidget->deleteLater();
    m_centralwidget = nullptr;

    m_mainlayout->deleteLater();
    m_mainlayout = nullptr;

    m_mainTab->deleteLater();
    m_mainTab = nullptr;

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
        QMessageBox::critical(this->centralWidget(), tr("OpenGL Support"),
                                 tr("This system does not support OpenGL"));
        return false;
    }

    // Fail if you do not have OpenGL 2.0 or higher driver
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_2_1) {
        QMessageBox::critical(this->centralWidget(), tr("OpenGL 2.x Context"),
                                 tr("This system does not support OpenGL 2.x Contexts"));
        return false;
    }

    // Fail if you do not support SSL secure connection
    if (!QSslSocket::supportsSsl()) {
        QMessageBox::critical(this->centralWidget(), tr("Secure connection"),
                                 tr("This system does not secure SSL connections"));
        return false;
    }

    // Fail if min version is not supported
    async::DataRequest request = m_dataProxy->loadMinVersion();
    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO show the error present in request.getErrors()
        QMessageBox::critical(this->centralWidget(), tr("Minimum Version"),
                                tr("Required version could not be retrieved from the server, try again"));
        return false;
    } else {
        // refresh datasets on the model
        const auto minVersion  = m_dataProxy->getMinVersion();
        if (!versionIsGreaterOrEqual(Globals::VersionNumbers, minVersion)) {
            QMessageBox::critical(this->centralWidget(), tr("Minimum Version"),
                                     tr("This version of the software is not supported anymore, please update!"));
            return false;
        }
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

    m_actionExit = new QAction(this);
    m_actionHelp = new QAction(this);
    m_actionVersion = new QAction(this);
    m_actionAbout = new QAction(this);
    m_actionClear_Cache = new QAction(this);

    m_centralwidget = new QWidget(this);

    m_mainlayout = new QVBoxLayout(m_centralwidget);
    //pass reference to dataProxy to tab manager
    m_mainTab = new ExtendedTabWidget(m_dataProxy, m_centralwidget);
    m_mainlayout->addWidget(m_mainTab);
    setCentralWidget(m_centralwidget);

    QStatusBar *statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    QMenuBar *menubar = new QMenuBar(this);
    menubar->setNativeMenuBar(true);
    menubar->setGeometry(QRect(0, 0, 1024, 22));

    m_menuLoad = new QMenu(menubar);
    m_menuHelp = new QMenu(menubar);

    setMenuBar(menubar);

    menubar->addAction(m_menuLoad->menuAction());
    menubar->addAction(m_menuHelp->menuAction());
    m_menuLoad->addSeparator();
    m_menuLoad->addAction(m_actionExit);
    m_menuLoad->addAction(m_actionClear_Cache);
    m_menuHelp->addAction(m_actionAbout);

    m_actionExit->setText(tr("Exit"));
    m_actionHelp->setText(tr("Help"));
    m_actionVersion->setText(tr("Version"));
    m_actionAbout->setText(tr("About..."));
    m_actionClear_Cache->setText(tr("Clear Cache"));
    m_menuLoad->setTitle(tr("File"));
    m_menuHelp->setTitle(tr("About"));

    QMetaObject::connectSlotsByName(this);
}

void stVi::handleMessage(const QString &message)
{
    QMessageBox::critical(this->centralWidget(), "stVi", message);
}

void stVi::slotShowAbout()
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
        qDebug() << "[stVi] : Cleaning the cache...";
        m_dataProxy->cleanAll();
        m_mainTab->resetStatus();
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
    m_actionExit->setShortcuts(QList<QKeySequence>()
                             << QKeySequence(Qt::ALT | Qt::Key_F4)
                             << QKeySequence(Qt::CTRL | Qt::Key_Q));
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    m_actionExit->setShortcut(QKeySequence::Quit);
#endif

#if defined Q_OS_MAC
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+M"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(showMinimized()));
    m_actionExit->setShortcut(QKeySequence("Ctrl+W"));
#endif
}

void stVi::createConnections()
{
    //exit and print action
    connect(m_actionExit, SIGNAL(triggered(bool)), this, SLOT(slotExit()));
    //clear cache action
    connect(m_actionClear_Cache, SIGNAL(triggered(bool)), this, SLOT(slotClearCache()));
    //signal that shows the about dialog
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(slotShowAbout()));
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

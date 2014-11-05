
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
#include <QMenuBar>
#include <QStatusBar>

#include "utils/Utils.h"
#include "error/Error.h"
#include "error/ApplicationError.h"
#include "error/ServerError.h"
#include "dialogs/AboutDialog.h"
#include "viewPages/ExtendedTabWidget.h"
#include "auth/AuthorizationManager.h"

namespace {

bool versionIsGreaterOrEqual(const std::array<qulonglong, 3> &version1,
                             const std::array<qulonglong, 3> &version2)
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
    m_mainTab(nullptr),
    m_dataProxy(nullptr),
    m_authManager(nullptr)
{
    setUnifiedTitleAndToolBarOnMac(true);

    m_dataProxy = new DataProxy();
    Q_ASSERT(!m_dataProxy.isNull());

    m_authManager = new AuthorizationManager(m_dataProxy);
    Q_ASSERT(!m_authManager.isNull());
}

stVi::~stVi()
{
    if (!m_dataProxy.isNull()) {
        delete m_dataProxy;
    }
    m_dataProxy = nullptr;

    if (!m_authManager.isNull()) {
        delete m_authManager;
    }
    m_authManager = nullptr;


    m_actionExit->deleteLater();
    m_actionExit = nullptr;

    m_actionHelp->deleteLater();
    m_actionHelp = nullptr;

    m_actionVersion->deleteLater();
    m_actionVersion = nullptr;

    m_actionAbout->deleteLater();
    m_actionAbout = nullptr;

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
    m_dataProxy->loadMinVersion();
    connect(m_dataProxy.data(),
            SIGNAL(signalMinVersionDownloaded(DataProxy::DownloadStatus)),
            this, SLOT(slotMinVersionDownloaded(DataProxy::DownloadStatus)));
    return true;
}

void stVi::slotMinVersionDownloaded(DataProxy::DownloadStatus status)
{
    //TODO do something if it failed or aborted?
    if (status == DataProxy::Success) {
        const auto minVersion  = m_dataProxy->getMinVersion();
        if (!versionIsGreaterOrEqual(Globals::VersionNumbers, minVersion)) {
            QMessageBox::critical(this->centralWidget(), tr("Minimum Version"),
                                  tr("This version of the software is not supported anymore,"
                                     "please update!"));
            QApplication::exit(EXIT_FAILURE);
        }
    }
}

void stVi::setupUi()
{
    setObjectName(QStringLiteral("stVi"));
    setWindowModality(Qt::NonModal);
    resize(1024, 768);
    setMinimumSize(QSize(1024, 768));
    setWindowIcon(QIcon(QStringLiteral(":/images/st_icon.png")));

    //create main widget
    QWidget *centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);
    //create main layout
    QVBoxLayout *mainlayout = new QVBoxLayout(centralwidget);

    //create tab manager
    //pass reference to dataProxy and authManager to tab manager
    m_mainTab = new ExtendedTabWidget(m_dataProxy, m_authManager, centralwidget);
    mainlayout->addWidget(m_mainTab);

    //create status bar
    QStatusBar *statusbar = new QStatusBar();
    setStatusBar(statusbar);

    //create menu bar
    QMenuBar *menubar = new QMenuBar();
    menubar->setNativeMenuBar(true);
    menubar->setGeometry(QRect(0, 0, 1024, 22));
    setMenuBar(menubar);

    //create actions
    m_actionExit = new QAction(this);
    m_actionHelp = new QAction(this);
    m_actionVersion = new QAction(this);
    m_actionAbout = new QAction(this);
    m_actionClear_Cache = new QAction(this);
    m_actionExit->setText(tr("Exit"));
    m_actionHelp->setText(tr("Help"));
    m_actionVersion->setText(tr("Version"));
    m_actionAbout->setText(tr("About..."));
    m_actionClear_Cache->setText(tr("Clear Cache"));

    //create menus
    QMenu *menuLoad = new QMenu(menubar);
    QMenu *menuHelp = new QMenu(menubar);
    menuLoad->setTitle(tr("File"));
    menuHelp->setTitle(tr("Help"));
    menuLoad->addAction(m_actionExit);
    menuLoad->addAction(m_actionClear_Cache);
    menuHelp->addAction(m_actionAbout);

    menubar->addAction(menuLoad->menuAction());
    menubar->addAction(menuHelp->menuAction());
}

void stVi::slotShowAbout()
{
    QScopedPointer<AboutDialog> about(new AboutDialog(this,
                                                      Qt::CustomizeWindowHint
                                                      | Qt::WindowTitleHint));
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
        QApplication::exit(EXIT_SUCCESS);
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
    statusBar()->showMessage(tr("Spatial Transcriptomics Viewer"));
}

// apply stylesheet and configurations
void stVi::initStyle()
{
    //TODO move to styleshee.css file
    setStyleSheet("QToolBar {border-bottom: 1px white; border-top: 1px white;}"
                  "QToolButton:checked {background-color: rgb(175,175,175);}"
                  "QToolButton{background-color: transparent;}"
                  "QToolButton:hover {background-color: rgb(175,175,175);}"
                  "QLineEdit {border: 1px solid gray;background: white;selection-background-color: darkgray;}"
                  "QTableView {background-color: transparent;}"
                  "QHeaderView::section {background-color: rgb(176,196,222);}"
                  "QHeaderView {background-color: rgb(176,196,222);}"
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
    // save the geometry and state of the main window
    QByteArray geometry = saveGeometry();
    settings.setValue(Globals::SettingsGeometry, geometry);
    QByteArray state = saveState();
    settings.setValue(Globals::SettingsState, state);
    //TODO save global settings (menus and status)
}

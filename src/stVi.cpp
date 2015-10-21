
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
#include <QFont>
#include "utils/Utils.h"
#include "error/Error.h"
#include "error/ApplicationError.h"
#include "error/ServerError.h"
#include "dialogs/AboutDialog.h"
#include "auth/AuthorizationManager.h"
#include "viewPages/DatasetPage.h"
#include "viewPages/CellViewPage.h"
#include "viewPages/UserSelectionsPage.h"
#include "viewPages/GenesWidget.h"
#include "dataModel/User.h"

using namespace Globals;

namespace
{

bool versionIsGreaterOrEqual(const std::array<qulonglong, 3>& version1,
                             const std::array<qulonglong, 3>& version2)
{
    int index = 0;
    for (const auto& num : version1) {
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

stVi::stVi(QWidget* parent)
    : QMainWindow(parent)
    , m_actionExit(nullptr)
    , m_actionHelp(nullptr)
    , m_actionVersion(nullptr)
    , m_actionAbout(nullptr)
    , m_actionClear_Cache(nullptr)
    , m_actionDatasets(nullptr)
    , m_actionLogOut(nullptr)
    , m_actionSelections(nullptr)
    , m_dataProxy(nullptr)
    , m_authManager(nullptr)
    , m_datasets(nullptr)
    , m_cellview(nullptr)
    , m_user_selections(nullptr)
    , m_genes(nullptr)
{
    setUnifiedTitleAndToolBarOnMac(true);

    m_dataProxy = new DataProxy();
    Q_ASSERT(!m_dataProxy.isNull());

    m_authManager = new AuthorizationManager(m_dataProxy);
    Q_ASSERT(!m_authManager.isNull());

    // We init the views here
    m_datasets = new DatasetPage(m_dataProxy);
    m_cellview = new CellViewPage(m_dataProxy);
    m_user_selections = new UserSelectionsPage(m_dataProxy);
    m_genes = new GenesWidget(m_dataProxy);
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

    if (!m_datasets.isNull()) {
        delete m_datasets;
    }
    m_datasets = nullptr;

    if (!m_cellview.isNull()) {
        delete m_cellview;
    }
    m_cellview = nullptr;

    if (!m_user_selections.isNull()) {
        delete m_user_selections;
    }
    m_user_selections = nullptr;

    if (!m_genes.isNull()) {
        delete m_genes;
    }
    m_genes = nullptr;
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
        QMessageBox::critical(this->centralWidget(),
                              tr("OpenGL Support"),
                              tr("This system does not support OpenGL"));
        return false;
    }

    // Fail if you do not have OpenGL 2.0 or higher driver
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_2_1) {
        QMessageBox::critical(this->centralWidget(),
                              tr("OpenGL 2.x Context"),
                              tr("This system does not support OpenGL 2.x Contexts"));
        return false;
    }

    // Fail if you do not support SSL secure connection
    if (!QSslSocket::supportsSsl()) {
        QMessageBox::critical(this->centralWidget(),
                              tr("Secure connection"),
                              tr("This system does not secure SSL connections"));
        return false;
    }

    // check for min version if supported
    m_dataProxy->loadMinVersion();
    m_dataProxy->activateCurrentDownloads();
    return true;
}

void stVi::slotDownloadFinished(const DataProxy::DownloadStatus status,
                                const DataProxy::DownloadType type)
{
    // TODO do something if it failed or aborted?
    if (type == DataProxy::MinVersionDownloaded && status == DataProxy::Success) {
        const auto minVersion = m_dataProxy->getMinVersion();
        if (!versionIsGreaterOrEqual(Globals::VersionNumbers, minVersion)) {
            QMessageBox::critical(this->centralWidget(),
                                  tr("Minimum Version"),
                                  tr("This version of the software is not supported anymore,"
                                     "please update!"));
            QApplication::exit(EXIT_FAILURE);
        }
    } else if (type == DataProxy::UserDownloaded && status == DataProxy::Success) {
        const auto user = m_dataProxy->getUser();
        Q_ASSERT(!user.isNull());
        if (!user->enabled()) {
            QMessageBox::critical(this,
                                  tr("Authorization Error"),
                                  tr("The current user is disabled"));
            return;
        }
        // show user info in label
        m_cellview->slotSetUserName(user->username());
    }

    // TODO disable busy bar
}

void stVi::setupUi()
{
    // main window
    setObjectName(QStringLiteral("stVi"));
    setWindowModality(Qt::NonModal);
    resize(1024, 768);
    setMinimumSize(QSize(1024, 768));
    setWindowIcon(QIcon(QStringLiteral(":/images/st_icon.png")));

    // create main widget
    QWidget* centralwidget = new QWidget(this);
    // important to set the style to this widget only to avoid propagation
    centralwidget->setObjectName("centralWidget");
    centralwidget->setStyleSheet("QWidget#centralWidget {background-color: rgb(45, 45, 45);}");
    centralwidget->setWindowFlags(Qt::FramelessWindowHint);
    setCentralWidget(centralwidget);

    // create main layout
    QVBoxLayout* mainlayout = new QVBoxLayout(centralwidget);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(0, 0, 0, 0);

    // main view
    mainlayout->addWidget(m_cellview);

    // create status bar
    QStatusBar* statusbar = new QStatusBar();
    setStatusBar(statusbar);

    // create menu bar
    QMenuBar* menubar = new QMenuBar();
    menubar->setNativeMenuBar(true);
    menubar->setGeometry(QRect(0, 0, 1024, 22));
    setMenuBar(menubar);

    // create actions
    m_actionExit = new QAction(this);
    m_actionHelp = new QAction(this);
    m_actionVersion = new QAction(this);
    m_actionAbout = new QAction(this);
    m_actionClear_Cache = new QAction(this);
    m_actionDatasets = new QAction(this);
    m_actionDatasets->setCheckable(true);
    m_actionLogOut = new QAction(this);
    m_actionSelections = new QAction(this);
    m_actionSelections->setCheckable(true);
    m_actionExit->setText(tr("Exit"));
    m_actionHelp->setText(tr("Help"));
    m_actionVersion->setText(tr("Version"));
    m_actionAbout->setText(tr("About..."));
    m_actionClear_Cache->setText(tr("Clear Cache"));
    m_actionDatasets->setText(tr("Datasets"));
    m_actionLogOut->setText(tr("Log out"));
    m_actionSelections->setText(tr("Selections"));

    // create menus
    QMenu* menuLoad = new QMenu(menubar);
    QMenu* menuHelp = new QMenu(menubar);
    QMenu* menuViews = new QMenu(menubar);
    menuLoad->setTitle(tr("File"));
    menuHelp->setTitle(tr("Help"));
    menuViews->setTitle(tr("Views"));
    menuLoad->addAction(m_actionExit);
    menuLoad->addAction(m_actionClear_Cache);
    menuHelp->addAction(m_actionAbout);
    menuViews->addAction(m_actionDatasets);
    menuViews->addAction(m_actionLogOut);
    menuViews->addAction(m_actionSelections);

    // add menus to menu bar
    menubar->addAction(menuLoad->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menubar->addAction(menuViews->menuAction());

    // add gene table as dock widget
    QDockWidget* dock_genes = new QDockWidget(tr("Genes"), this);
    m_genes->setObjectName("Genes");
    dock_genes->setWidget(m_genes);
    dock_genes->setObjectName("GenesDock");
    dock_genes->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    menuViews->addAction(dock_genes->toggleViewAction());
    addDockWidget(Qt::LeftDockWidgetArea, dock_genes);
}

void stVi::slotShowAbout()
{
    QScopedPointer<AboutDialog> about(
                new AboutDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    about->exec();
}

void stVi::slotExit()
{
    const int answer = QMessageBox::warning(this,
                                            tr("Exit application"),
                                            tr("Are you really sure you want to exit now?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        qDebug() << "[stVi] Info: Exitting the application...";
        saveSettings();
        QApplication::exit(EXIT_SUCCESS);
    }
}

void stVi::slotClearCache()
{
    const int answer = QMessageBox::warning(this,
                                            tr("Clear the Cache"),
                                            tr("Are you really sure you want to clear the cache?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        qDebug() << "[stVi] : Cleaning the cache...";
        m_dataProxy->cleanAll();
    }
}

void stVi::createLayouts()
{
    statusBar()->showMessage(tr("Spatial Transcriptomics Viewer"));
}

// apply stylesheet and configurations
void stVi::initStyle()
{
    // TODO move to stylesheet.css file
    setStyleSheet(GENERAL_STYLE);
    m_datasets->setStyleSheet(GENERAL_STYLE);
    m_cellview->setStyleSheet(GENERAL_STYLE);
    m_user_selections->setStyleSheet(GENERAL_STYLE);

    // apply font
    QFont font("Open Sans", 12);
    QApplication::setFont(font);
}

void stVi::createShorcuts()
{
#if defined Q_OS_WIN
    m_actionExit->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::ALT | Qt::Key_F4)
                               << QKeySequence(Qt::CTRL | Qt::Key_Q));
#elif defined Q_OS_LINUX || defined Q_OS_MAC
    m_actionExit->setShortcut(QKeySequence::Quit);
#endif

#if defined Q_OS_MAC
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+M"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(showMinimized()));
    m_actionExit->setShortcut(QKeySequence("Ctrl+W"));
#endif
}

void stVi::createConnections()
{
    // exit and print action
    connect(m_actionExit, SIGNAL(triggered(bool)), this, SLOT(slotExit()));
    // clear cache action
    connect(m_actionClear_Cache, SIGNAL(triggered(bool)), this, SLOT(slotClearCache()));
    // signal that shows the about dialog
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(slotShowAbout()));
    // signal that shows the datasets
    connect(m_actionDatasets, SIGNAL(triggered(bool)), m_datasets.data(), SLOT(show()));
    // signal that shows the log in widget
    connect(m_actionLogOut, SIGNAL(triggered()), this, SLOT(slotLogOutButton()));
    // signal that shows the selections
    connect(m_actionSelections, SIGNAL(triggered(bool)), m_user_selections.data(), SLOT(show()));

    // connect authorization signals
    connect(m_authManager, SIGNAL(signalAuthorize()), this, SLOT(slotAuthorized()));
    connect(m_authManager,
            SIGNAL(signalError(QSharedPointer<Error>)),
            this,
            SLOT(slotAuthorizationError(QSharedPointer<Error>)));

    // connect the data proxy signal
    connect(m_dataProxy.data(),
            SIGNAL(signalDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)),
            this,
            SLOT(slotDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)));

    // connect the open dataset from datasetview -> cellview
    connect(m_datasets.data(),
            SIGNAL(signalDatasetOpen(QString)),
            m_cellview.data(),
            SLOT(slotDatasetOpen(QString)));

    // connect the open dataset from datasetview -> genes table
    connect(m_datasets.data(),
            SIGNAL(signalDatasetOpen(QString)),
            m_genes.data(),
            SLOT(slotLoadModel(QString)));

    // connect genes table signals to cellview
    connect(m_genes.data(),
            SIGNAL(signalSelectionChanged(DataProxy::GeneList)),
            m_cellview.data(),
            SLOT(slotGenesSelected(DataProxy::GeneList)));
    connect(m_genes.data(),
            SIGNAL(signalColorChanged(DataProxy::GeneList)),
            m_cellview.data(),
            SLOT(slotGenesColor(DataProxy::GeneList)));

    // connect gene selection signals from selections view
    connect(m_user_selections.data(),
            SIGNAL(signalClearSelections()),
            m_cellview.data(),
            SLOT(slotClearSelections()));
    connect(m_user_selections.data(),
            SIGNAL(signalShowSelections(QVector<UserSelection>)),
            m_cellview.data(),
            SLOT(slotShowSelection(QVector<UserSelection>)));
    connect(m_cellview.data(),
            SIGNAL(signalUserSelection()),
            m_user_selections.data(),
            SLOT(slotSelectionsUpdated()));

    // connect log out signal from cell view
    connect(m_cellview.data(),
            SIGNAL(signalLogOut()),
            this,
            SLOT(slotLogOutButton()));
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
    // TODO load global settings (menus and status)
}

void stVi::saveSettings() const
{
    QSettings settings;
    // save the geometry and state of the main window
    QByteArray geometry = saveGeometry();
    settings.setValue(Globals::SettingsGeometry, geometry);
    QByteArray state = saveState();
    settings.setValue(Globals::SettingsState, state);
    // TODO save global settings (menus and status)
}

void stVi::startAuthorization()
{
    // start the authorization (quiet if access token exists or interactive otherwise)
    m_authManager->startAuthorization();
}

void stVi::slotAuthorizationError(QSharedPointer<Error> error)
{
    // force clean access token and authorize again
    m_authManager->cleanAccesToken();
    m_authManager->startAuthorization();
    qDebug() << "Error trying to log in " << error->name() << " " << error->description();
}

void stVi::slotAuthorized()
{
    // clean the cache in the dataproxy
    m_dataProxy->clean();

    // load user from network
    // TODO show busy bar
    m_dataProxy->loadUser();
    m_dataProxy->activateCurrentDownloads();
}

void stVi::slotLogOutButton()
{
    qDebug() << "Log in out..";

    // clear user name in label
    m_cellview->slotSetUserName("");

    // clean data proxy cache
    m_dataProxy->clean();

    // clean datasets/selections and main view
    m_datasets->clean();
    m_cellview->clean();
    m_user_selections->clean();
    m_genes->clear();

    // clean access token and authorize
    m_authManager->cleanAccesToken();
    m_authManager->startAuthorization();
}

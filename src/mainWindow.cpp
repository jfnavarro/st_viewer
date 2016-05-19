#include "MainWindow.h"

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
#include "SettingsStyle.h"

using namespace Style;

static const std::array<qulonglong, 3> VersionNumbers = {MAJOR, MINOR, PATCH};

namespace
{

bool versionIsGreaterOrEqual(const std::array<qulonglong, 3> &version1,
                             const std::array<qulonglong, 3> &version2)
{
    int index = 0;
    for (const auto &num : version1) {
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

static const QString SettingsGeometry = QStringLiteral("Geometry");
static const QString SettingsState = QStringLiteral("State");

MainWindow::MainWindow(QWidget *parent)
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
    , m_remote_data(false)
{
    setUnifiedTitleAndToolBarOnMac(true);

    m_dataProxy = QSharedPointer<DataProxy>(new DataProxy());
    Q_ASSERT(!m_dataProxy.isNull());

    m_authManager = QSharedPointer<AuthorizationManager>(new AuthorizationManager(m_dataProxy));
    Q_ASSERT(!m_authManager.isNull());

    // We init the views here
    m_datasets.reset(new DatasetPage(m_dataProxy));
    m_cellview.reset(new CellViewPage(m_dataProxy));
    m_user_selections.reset(new UserSelectionsPage(m_dataProxy));
    m_genes.reset(new GenesWidget(m_dataProxy));
}

MainWindow::~MainWindow()
{
}

void MainWindow::init(const bool remote_data)
{
    m_remote_data = remote_data;

    // init style, size and icons
    initStyle();

    // create ui widgets
    setupUi();

    // create keyboard shortcuts
    createShorcuts();

    // decorate the main window
    createLayouts();

    // make signal connections
    createConnections();

    // restore settings
    loadSettings();
}

bool MainWindow::checkSystemRequirements() const
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

    return true;
}

void MainWindow::setupUi()
{
    // main window
    setObjectName(QStringLiteral("STViewer"));
    setWindowModality(Qt::NonModal);
    resize(1024, 768);
    setMinimumSize(QSize(1024, 768));
    setWindowIcon(QIcon(QStringLiteral(":/images/st_icon.png")));

    // create main widget
    QWidget *centralwidget = new QWidget(this);
    // important to set the style to this widget only to avoid propagation
    centralwidget->setObjectName("centralWidget");
    centralwidget->setStyleSheet("QWidget#centralWidget {background-color: rgb(45, 45, 45);}");
    centralwidget->setWindowFlags(Qt::FramelessWindowHint);
    setCentralWidget(centralwidget);

    // create main layout
    QVBoxLayout *mainlayout = new QVBoxLayout(centralwidget);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(0, 0, 0, 0);

    // main view
    mainlayout->addWidget(m_cellview.data());

    // create status bar
    QStatusBar *statusbar = new QStatusBar();
    setStatusBar(statusbar);

    // create menu bar
    QMenuBar *menubar = new QMenuBar();
    menubar->setNativeMenuBar(true);
    menubar->setGeometry(QRect(0, 0, 1024, 22));
    setMenuBar(menubar);

    // create actions
    m_actionExit.reset(new QAction(this));
    m_actionHelp.reset(new QAction(this));
    m_actionVersion.reset(new QAction(this));
    m_actionAbout.reset(new QAction(this));
    m_actionClear_Cache.reset(new QAction(this));
    m_actionDatasets.reset(new QAction(this));
    m_actionDatasets->setCheckable(true);
    m_actionLogOut.reset(new QAction(this));
    m_actionSelections.reset(new QAction(this));
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
    QMenu *menuLoad = new QMenu(menubar);
    QMenu *menuHelp = new QMenu(menubar);
    QMenu *menuViews = new QMenu(menubar);
    menuLoad->setTitle(tr("File"));
    menuHelp->setTitle(tr("Help"));
    menuViews->setTitle(tr("Views"));
    menuLoad->addAction(m_actionExit.data());
    menuLoad->addAction(m_actionClear_Cache.data());
    menuHelp->addAction(m_actionAbout.data());
    menuViews->addAction(m_actionDatasets.data());
    menuViews->addAction(m_actionLogOut.data());
    menuViews->addAction(m_actionSelections.data());

    // add menus to menu bar
    menubar->addAction(menuLoad->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menubar->addAction(menuViews->menuAction());

    // add gene table as dock widget
    QDockWidget *dock_genes = new QDockWidget(tr("Genes"), this);
    m_genes->setObjectName("Genes");
    dock_genes->setWidget(m_genes.data());
    dock_genes->setObjectName("GenesDock");
    dock_genes->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    menuViews->addAction(dock_genes->toggleViewAction());
    addDockWidget(Qt::LeftDockWidgetArea, dock_genes);
}

void MainWindow::slotShowAbout()
{
    QScopedPointer<AboutDialog> about(
        new AboutDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    about->exec();
}

void MainWindow::slotExit()
{
    const int answer = QMessageBox::warning(this,
                                            tr("Exit application"),
                                            tr("Are you really sure you want to exit now?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        saveSettings();
        QApplication::exit(EXIT_SUCCESS);
    }
}

void MainWindow::slotClearCache()
{
    const int answer = QMessageBox::warning(this,
                                            tr("Clear the Cache"),
                                            tr("Are you really sure you want to clear the cache?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No | QMessageBox::Escape);

    if (answer == QMessageBox::Yes) {
        m_dataProxy->cleanAll();
    }
}

void MainWindow::createLayouts()
{
    statusBar()->showMessage(tr("Spatial Transcriptomics Research Viewer"));
}

// apply stylesheet and configurations
void MainWindow::initStyle()
{
    // TODO move to stylesheet.css file
    setStyleSheet(GENERAL_STYLE);
    m_datasets->setStyleSheet(GENERAL_STYLE);
    m_cellview->setStyleSheet(GENERAL_STYLE);
    m_user_selections->setStyleSheet(GENERAL_STYLE);
}

void MainWindow::createShorcuts()
{
#if defined Q_OS_WIN
    m_actionExit->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::ALT | Qt::Key_F4)
                                                     << QKeySequence(Qt::CTRL | Qt::Key_Q));
#elif defined Q_OS_LINUX || defined Q_OS_MAC
    m_actionExit->setShortcut(QKeySequence::Quit);
#endif

#if defined Q_OS_MAC
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+M"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(showMinimized()));
    m_actionExit->setShortcut(QKeySequence("Ctrl+W"));
#endif
}

void MainWindow::createConnections()
{
    // exit and print action
    connect(m_actionExit.data(), SIGNAL(triggered(bool)), this, SLOT(slotExit()));
    // clear cache action
    connect(m_actionClear_Cache.data(), SIGNAL(triggered(bool)), this, SLOT(slotClearCache()));
    // signal that shows the about dialog
    connect(m_actionAbout.data(), SIGNAL(triggered()), this, SLOT(slotShowAbout()));
    // signal that shows the datasets
    connect(m_actionDatasets.data(), SIGNAL(triggered(bool)), m_datasets.data(), SLOT(show()));
    // signal that shows the log in widget
    connect(m_actionLogOut.data(), SIGNAL(triggered()), this, SLOT(slotLogOutButton()));
    // signal that shows the selections
    connect(m_actionSelections.data(),
            SIGNAL(triggered(bool)),
            m_user_selections.data(),
            SLOT(show()));

    // connect authorization signals
    connect(m_authManager.data(), SIGNAL(signalAuthorize()), this, SLOT(slotAuthorized()));
    connect(m_authManager.data(),
            SIGNAL(signalError(QSharedPointer<Error>)),
            this,
            SLOT(slotAuthorizationError(QSharedPointer<Error>)));

    // connect the open dataset from datasetview -> cellview
    connect(m_datasets.data(),
            SIGNAL(signalDatasetOpen(QString)),
            m_cellview.data(),
            SLOT(slotDatasetOpen(QString)));

    // connect the updated dataset from the datasetview -> cellview
    connect(m_datasets.data(),
            SIGNAL(signalDatasetUpdated(QString)),
            m_cellview.data(),
            SLOT(slotDatasetUpdated(QString)));

    // connect the removed dataset from the datasetview -> cellview
    connect(m_datasets.data(),
            SIGNAL(signalDatasetRemoved(QString)),
            m_cellview.data(),
            SLOT(slotDatasetRemoved(QString)));

    // connect the open dataset from datasetview -> genes table
    connect(m_datasets.data(),
            SIGNAL(signalDatasetOpen(QString)),
            m_genes.data(),
            SLOT(slotDatasetOpen(QString)));

    // connect the updated dataset from the datasetview -> genes table
    connect(m_datasets.data(),
            SIGNAL(signalDatasetUpdated(QString)),
            m_genes.data(),
            SLOT(slotDatasetUpdated(QString)));

    // connect the removed dataset from the datasetview -> genes table
    connect(m_datasets.data(),
            SIGNAL(signalDatasetRemoved(QString)),
            m_genes.data(),
            SLOT(slotDatasetRemoved(QString)));

    // connect genes table signals to cellview
    connect(m_genes.data(),
            SIGNAL(signalSelectionChanged(DataProxy::GeneList)),
            m_cellview.data(),
            SLOT(slotGenesSelected(DataProxy::GeneList)));
    connect(m_genes.data(),
            SIGNAL(signalColorChanged(DataProxy::GeneList)),
            m_cellview.data(),
            SLOT(slotGenesColor(DataProxy::GeneList)));
    connect(m_genes.data(),
            SIGNAL(signalCutOffChanged(DataProxy::GenePtr)),
            m_cellview.data(),
            SLOT(slotGeneCutOff(DataProxy::GenePtr)));

    // connect gene selection signals from selections view
    connect(m_user_selections.data(),
            SIGNAL(signalClearSelections()),
            m_cellview.data(),
            SLOT(slotClearSelections()));
    connect(m_cellview.data(),
            SIGNAL(signalUserSelection()),
            m_user_selections.data(),
            SLOT(slotSelectionsUpdated()));

    // connect log out signal from cell view
    connect(m_cellview.data(), SIGNAL(signalLogOut()), this, SLOT(slotLogOutButton()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    slotExit();
}

void MainWindow::loadSettings()
{
    QSettings settings;
    // Retrieve the geometry and state of the main window
    restoreGeometry(settings.value(SettingsGeometry).toByteArray());
    restoreState(settings.value(SettingsState).toByteArray());
    // TODO load global settings (menus and status)
}

void MainWindow::saveSettings() const
{
    QSettings settings;
    // save the geometry and state of the main window
    QByteArray geometry = saveGeometry();
    settings.setValue(SettingsGeometry, geometry);
    QByteArray state = saveState();
    settings.setValue(SettingsState, state);
    // TODO save global settings (menus and status)
}

void MainWindow::startAuthorization()
{
    if (!m_remote_data) {
        return;
    }

    // clean the cache in the dataproxy
    m_dataProxy->clean();
    // start the authorization (quiet if access token exists or interactive otherwise)
    m_authManager->startAuthorization();
}

void MainWindow::slotAuthorizationError(QSharedPointer<Error> error)
{
    if (!m_remote_data) {
        return;
    }

    // force clean access token and authorize again
    m_authManager->cleanAccesToken();
    m_authManager->startAuthorization();
    qDebug() << "Error trying to log in " << error->name() << " " << error->description();
}

void MainWindow::slotAuthorized()
{
    if (!m_remote_data) {
        return;
    }

    // clean datasets/selections and main view
    m_datasets->clean();
    m_cellview->clean();
    m_user_selections->clean();
    m_genes->clear();

    // check for min version if supported and load user (only in online mode)
    if (m_dataProxy->loadMinVersion()) {
        const auto minVersion = m_dataProxy->getMinVersion();
        if (!versionIsGreaterOrEqual(VersionNumbers, minVersion)) {
            QMessageBox::critical(this->centralWidget(),
                                  tr("Minimum Version"),
                                  tr("This version of the software is not supported anymore,"
                                     "please update!"));
            QApplication::exit(EXIT_FAILURE);
        }
    } else {
        // TODO exit here?
        qDebug() << "Min version could not be downloaded..";
    }

    if (m_dataProxy->loadUser()) {
        const auto user = m_dataProxy->getUser();
        Q_ASSERT(user);
        if (!user->enabled()) {
            QMessageBox::critical(this,
                                  tr("Authorization Error"),
                                  tr("The current user is disabled"));
        } else {
            // show user info in label
            m_cellview->slotSetUserName(user->username());
        }
    } else {
        // TODO exit here?
        qDebug() << "User information could not be downloaded..";
    }
}

void MainWindow::slotLogOutButton()
{
    if (!m_remote_data) {
        return;
    }
    // clear user name in label
    m_cellview->slotSetUserName("");
    // clean the cache in the dataproxy
    m_dataProxy->clean();
    // clean access token and start authorization
    m_authManager->cleanAccesToken();
    m_authManager->startAuthorization();
}

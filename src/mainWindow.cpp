#include "mainWindow.h"

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
#include <QMenuBar>
#include <QStatusBar>
#include <QFont>
#include <QDir>
#include <QFileDialog>

#include "dialogs/AboutDialog.h"
#include "viewPages/DatasetPage.h"
#include "viewPages/CellViewPage.h"
#include "viewPages/UserSelectionsPage.h"
#include "viewPages/GenesWidget.h"
#include "viewPages/SpotsWidget.h"
#include "config/Configuration.h"
#include "SettingsStyle.h"

using namespace Style;
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
    , m_actionSelections(nullptr)
    , m_datasets(nullptr)
    , m_cellview(nullptr)
    , m_user_selections(nullptr)
    , m_genes(nullptr)
    , m_spots(nullptr)
{
    setUnifiedTitleAndToolBarOnMac(true);

    // Create the main views
    m_genes.reset(new GenesWidget());
    Q_ASSERT(m_genes);
    m_spots.reset(new SpotsWidget());
    Q_ASSERT(m_spots);
    m_datasets.reset(new DatasetPage());
    Q_ASSERT(m_datasets);
    m_user_selections.reset(new UserSelectionsPage());
    Q_ASSERT(m_user_selections);
    m_cellview.reset(new CellViewPage(m_spots, m_genes, m_user_selections));
    Q_ASSERT(m_cellview);
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
    // init style, size and icons
    initStyle();

    // create ui widgets
    setupUi();

    // create keyboard shortcuts
    createShorcuts();

    // create connections
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

    // Fail if there is not configuration file
    Configuration config;
    if (!config.is_valid()) {
        QMessageBox::critical(this->centralWidget(),
                              tr("Configuration file"),
                              tr("Configuration file could not be loaded"));
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
    m_actionSelections.reset(new QAction(this));
    m_actionSelections->setCheckable(true);
    m_actionExit->setText(tr("Exit"));
    m_actionHelp->setText(tr("Help"));
    m_actionVersion->setText(tr("Version"));
    m_actionAbout->setText(tr("About..."));
    m_actionClear_Cache->setText(tr("Clear Cache"));
    m_actionDatasets->setText(tr("Datasets"));
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
    menuViews->addAction(m_actionSelections.data());
    // add menus to menu bar
    menubar->addAction(menuLoad->menuAction());
    menubar->addAction(menuHelp->menuAction());
    menubar->addAction(menuViews->menuAction());

    // add genes table as dock widget
    QDockWidget *dock_genes = new QDockWidget(tr("Genes"), this);
    m_genes->setObjectName("Genes");
    dock_genes->setWidget(m_genes.data());
    dock_genes->setObjectName("GenesDock");
    dock_genes->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    menuViews->addAction(dock_genes->toggleViewAction());
    addDockWidget(Qt::LeftDockWidgetArea, dock_genes);

    // add spots table as dock widget
    QDockWidget *dock_spots = new QDockWidget(tr("Spots"), this);
    m_genes->setObjectName("Spots");
    dock_spots->setWidget(m_spots.data());
    dock_spots->setObjectName("SpotsDock");
    dock_spots->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    menuViews->addAction(dock_spots->toggleViewAction());
    addDockWidget(Qt::LeftDockWidgetArea, dock_spots);

    // App's name
    statusBar()->showMessage(tr("Spatial Transcriptomics Research Viewer"));
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
        //NOTE nothing for now
    }
}

void MainWindow::initStyle()
{
    // apply stylesheet and configurations
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
    connect(m_actionExit.data(), &QAction::triggered, this, &MainWindow::slotExit);
    // clear cache action
    connect(m_actionClear_Cache.data(), &QAction::triggered, this, &MainWindow::slotClearCache);
    // signal that shows the about dialog
    connect(m_actionAbout.data(), &QAction::triggered, this, &MainWindow::slotShowAbout);
    // signal that shows the datasets
    connect(m_actionDatasets.data(), &QAction::triggered, m_datasets.data(), &DatasetPage::show);
    // signal that shows the selections
    connect(m_actionSelections.data(), &QAction::triggered, m_user_selections.data(),
            &UserSelectionsPage::show);

    // when the user opens a dataset
    connect(m_datasets.data(),
            &DatasetPage::signalDatasetOpen,
            this,
            &MainWindow::slotDatasetOpen);
    // when the users edits a dataset
    connect(m_datasets.data(),
            &DatasetPage::signalDatasetUpdated,
            this,
            &MainWindow::slotDatasetUpdated);
    // when the user removes the currently opened dataset
    connect(m_datasets.data(),
            &DatasetPage::signalDatasetRemoved,
            this,
            &MainWindow::slotDatasetRemoved);
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

void MainWindow::slotDatasetOpen(const QString &datasetname)
{
    const auto dataset = m_datasets->getCurrentDataset();
    qDebug() << "Dataset opened " << datasetname;
    m_cellview->loadDataset(*(dataset.data()));
}

void MainWindow::slotDatasetUpdated(const QString &datasetname)
{
    const auto dataset = m_datasets->getCurrentDataset();
    qDebug() << "Dataset updated " << datasetname;
    m_cellview->loadDataset(*(dataset.data()));
}

void MainWindow::slotDatasetRemoved(const QString &datasetname)
{
    qDebug() << "Dataset removed " << datasetname;
    m_genes->clear();
    m_spots->clear();
    m_cellview->clear();
}

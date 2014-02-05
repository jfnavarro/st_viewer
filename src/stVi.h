/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef stVi_H
#define stVi_H

#include <QMainWindow>

class QSettings;
class QCloseEvent;
class QAction;
class QStatusBar;
//class MainMenuBar;
class QMenu;
class Error;
class QVBoxLayout;
class ExtendedTabWidget;
class QWidget;
//class MainStatusBar;

class stVi : public QMainWindow
{
    Q_OBJECT

public:

    explicit stVi(QWidget* parent = 0);
    virtual ~stVi();

    //initialize main visual components
    void init();

    // check that the host has all the required hardware (1 yes : 0 no )
    bool checkSystemRequirements() const;

    //settings
    void loadSettings();
    void saveSettings() const;

signals:
    //we emit this when there is an error
    void signalError(Error* error);

public slots:

    // handlemessage is intented to be called from another instance trying to run at the same time
    void handleMessage(QString);
    // opens pop up static widget
    void showAbout();

private slots:

    // exit the application
    void slotExit();

    //clear the cache and files
    void slotClearCache();

private:

    // create all the widgets
    void setupUi();
    // initialize and configure layout
    void createLayouts();
    // load style sheets
    void initStyle();
    // initialize singleton objects
    void initSingleInstances();
    // create keyboard shortcuts
    void createShorcuts();
    // finalize and destroy singleton objects
    void finalizeSingleInstances();
    // create some connections
    void createConnections();
    // overloaded close Event function to handle the exit
    void closeEvent(QCloseEvent* event);

    QAction *actionExit = nullptr;
    QAction *actionHelp = nullptr;
    QAction *actionVersion = nullptr;
    QAction *actionPrint = nullptr;
    QAction *actionAbout = nullptr;
    QAction *actionClear_Cache = nullptr;

    //    MainStatusBar *statusbar;
    //    MainMenuBar *menubar;

    QMenu *menuLoad = nullptr;
    QMenu *menuHelp = nullptr;

    QWidget *centralwidget = nullptr;
    QVBoxLayout *mainlayout = nullptr;
    ExtendedTabWidget *mainTab = nullptr;
};

#endif // stVi_H

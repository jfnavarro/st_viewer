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
class MainMenuBar;
class QMenu;
class Error;
class QVBoxLayout;
class ExtendedTabWidget;
class QWidget;
class MainStatusBar;

class stVi : public QMainWindow
{
    Q_OBJECT

public:

    // constructor we only need the parameters object and the parent if there is any
    explicit stVi(QWidget* parent = 0);

    //initialize main visual components
    void init();

    int checkSystemRequirements();

    //settings
    void loadSettings();
    void saveSettings() const;

    //destructor
    ~stVi();

signals:

    //we emit this when there is an error
    void signalError(Error* error);

    // slots are functions that are going to be assigned to signals and events
public slots:

    void handleMessage(QString);

    void showAbout();

private slots:

    // exit the application
    void slotExit();

    //clear the cache and files
    void slotClearCache();

private:

    QAction *actionExit;
    QAction *actionHelp;
    QAction *actionVersion;
    QAction *actionPrint;
    QAction *actionAbout;
    QAction *actionClear_Cache;

    MainStatusBar *statusbar;
    MainMenuBar *menubar;

    QMenu *menuLoad;
    QMenu *menuHelp;

    QWidget *centralwidget;
    QVBoxLayout *mainlayout;
    ExtendedTabWidget *mainTab;

    //create all the widgets
    void setupUi();
    // initialize and configure layout
    void createLayouts();
    //style
    void initStyle();
    // initialize objects
    void initSingleInstances();
    // create keyboard shortcuts
    void createShorcuts();
    // finalize objects
    void finalizeSingleInstances();
    // create some connections
    void createConnections();
    // overloaded close Event function to handle the exit
    void closeEvent(QCloseEvent* event);
};

#endif // stVi_H

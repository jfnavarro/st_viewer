#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSettings;
class QCloseEvent;
class QAction;
class QStatusBar;
class QMenu;
class QVBoxLayout;
class QWidget;
class DatasetPage;
class CellViewPage;
class UserSelectionsPage;
class SpotsWidget;
class GenesWidget;

// This class represents the main window of the application
// it is composed of a tool bar, the cell main view and the gene tables
// it allows the user to show the datasets and the selections windows as well
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Initialize main visual components
    void init();

    // Check that the host has all the required hardware (true yes : false no )
    bool checkSystemRequirements() const;

    // System settings
    void loadSettings();
    void saveSettings() const;

private slots:

    // exit the application
    void slotExit();

    // clear the cache and local stored files
    void slotClearCache();

    // open pop up static widget to show info about the application
    void slotShowAbout();

    // a dataset has been opened
    void slotDatasetOpen(const QString &datasetname);
    // a dataset has been edited
    void slotDatasetUpdated(const QString &datasetname);
    // a dataset has been removed (the current open)
    void slotDatasetRemoved(const QString &datasetname);
    // the user wants to load a file with spot's colors
    void slotLoadSpotColors();
    // the user wants to create a selection
    void slotCreateSelection();

private:
    // create all the widgets
    void setupUi();
    // load style sheets
    void initStyle();
    // create keyboard shortcuts
    void createShorcuts();
    // create some connections
    void createConnections();

    // overloaded close Event function to handle the exit
    void closeEvent(QCloseEvent *event) override;

    // reference to some UI elements
    QScopedPointer<QAction> m_actionExit;
    QScopedPointer<QAction> m_actionHelp;
    QScopedPointer<QAction> m_actionVersion;
    QScopedPointer<QAction> m_actionAbout;
    QScopedPointer<QAction> m_actionClear_Cache;
    QScopedPointer<QAction> m_actionDatasets;
    QScopedPointer<QAction> m_actionSelections;
    QScopedPointer<QAction> m_actionSpotColors;

    // different views
    QScopedPointer<DatasetPage> m_datasets;
    QScopedPointer<CellViewPage> m_cellview;
    QScopedPointer<UserSelectionsPage> m_user_selections;
    QScopedPointer<GenesWidget> m_genes;
    QScopedPointer<SpotsWidget> m_spots;
};

#endif // MAINWINDOW_H

#ifndef stVi_H
#define stVi_H

#include <QMainWindow>
#include "data/DataProxy.h"

class QSettings;
class QCloseEvent;
class QAction;
class QStatusBar;
class QMenu;
class Error;
class QVBoxLayout;
class QWidget;
class AuthorizationManager;
class DatasetPage;
class CellViewPage;
class UserSelectionsPage;
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

    // initialize main visual components
    void init();

    // check that the host has all the required hardware (true yes : false no )
    bool checkSystemRequirements() const;

    // system settings
    void loadSettings();
    void saveSettings() const;

    // tries to find a cached access token otherwise it will show a log in dialog
    void startAuthorization();

private slots:

    // exit the application
    void slotExit();

    // clear the cache and local stored files
    void slotClearCache();

    // open pop up static widget to show info about the application
    void slotShowAbout();

    // to handle different authorization call backs (success and error)
    void slotAuthorizationError(QSharedPointer<Error> error);
    void slotAuthorized();
    // when user clicks to log out, shows log in dialog
    void slotLogOutButton();

private:
    // create all the widgets
    void setupUi();
    // initialize and configure layout
    void createLayouts();
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
    QScopedPointer<QAction> m_actionLogOut;
    QScopedPointer<QAction> m_actionSelections;

    // stVi owns dataProxy and AuthorizationManager
    QSharedPointer<DataProxy> m_dataProxy;
    QSharedPointer<AuthorizationManager> m_authManager;

    // different views
    QScopedPointer<DatasetPage> m_datasets;
    QScopedPointer<CellViewPage> m_cellview;
    QScopedPointer<UserSelectionsPage> m_user_selections;
    QScopedPointer<GenesWidget> m_genes;
};

#endif // stVi_H

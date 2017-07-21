#ifndef USERSELECTIONSPAGE_H
#define USERSELECTIONSPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <memory>

class UserSelectionsItemModel;
class QSortFilterProxyModel;
class UserSelection;

namespace Ui
{
class UserSelections;
} // namespace Ui //

// UserSelections page contains a table with the selection made by the user.
// Users can interact here to edit/remove selections and to perform analysis
// like the DEA, PCA, etc..

// TODO add option to show right click with mouse in selection (open, copy, edit...)
// TODO implement DEA analysis
class UserSelectionsPage : public QWidget
{
    Q_OBJECT

public:
    UserSelectionsPage(QWidget *parent = 0);
    virtual ~UserSelectionsPage();

    // clear the loaded content
    void clean();

    // to add a new selection to the list
    void addSelection(const UserSelection& selection);

signals:

public slots:

private slots:

    // slot that handles when the user selects a selection
    void slotSelectionSelected(QModelIndex index);
    // slots to handle when the user wants to export the selection to a file
    void slotExportSelection();
    void slotExportSelection(QModelIndex);
    // slot to handle when the user wants to remove a selection
    void slotRemoveSelection();
    void slotRemoveSelection(QModelIndex);
    // slot to handle when the user wants to edit a selection
    void slotEditSelection();
    void slotEditSelection(QModelIndex);
    // this slot will init and show the D.E.A. dialog (requires two selected
    // selections)
    void slotPerformDEA();
    // slot to perform a correlation analysis between two selections
    void slotPerformCorrelation();
    // slot to show the aggregated gene counts of the selection in a table
    void slotShowGenes();
    // slot to show the aggregated spot counts of the selection in a table
    void slotShowSpots();
    // to import a selection from a file
    void slotImportSelection();

protected:
    void showEvent(QShowEvent *event);

private:

    // internal functions for basic operations
    void removeSelections(const QList<UserSelection> &selections);
    void editSelection(const UserSelection &selection);
    void exportSelection(const UserSelection &selection);

    // internal function to check if the name exists
    bool nameExist(const QString &name);

    // internal clear focus and default status for the buttons
    void clearControls();
    // internal function to update the selections in the model
    void selectionsUpdated();

    // to retrieve the table's model
    QSortFilterProxyModel *selectionsProxyModel();
    UserSelectionsItemModel *selectionsModel();

    // Ui object
    QScopedPointer<Ui::UserSelections> m_ui;
    // the list of selections objects
    QList<UserSelection> m_selections;

    Q_DISABLE_COPY(UserSelectionsPage)
};

#endif /* USERSELECTIONSPAGE_H */

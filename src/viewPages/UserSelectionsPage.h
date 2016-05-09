#ifndef USERSELECTIONSPAGE_H
#define USERSELECTIONSPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <memory>
#include "data/DataProxy.h"

class UserSelectionsItemModel;
class QSortFilterProxyModel;
class AnalysisDEA;
class SelectionsWidget;
class WaitingSpinnerWidget;

namespace Ui
{
class UserSelections;
} // namespace Ui //

// UserSelections page contains a table with the selection made by the user.
// Users can interact here to edit/remove selections and to perform analysis on
// them
// like the DEA, PCA, etc..

// TODO factor out the confirmation dialog
// TODO add option to show right click with mouse in selection (open, copy,
// edit...)
// TODO implement clustering options to compute cell types (spot type)
// TODO add import selections option
// TODO add posibility to edit and save objects in the table
class UserSelectionsPage : public QWidget
{
    Q_OBJECT

public:
    UserSelectionsPage(QSharedPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~UserSelectionsPage();

    // clear the loaded content
    void clean();

signals:

    // to notify the cell view when the user wants to show or hide selections
    void signalClearSelections();
    void signalShowSelections(const QVector<UserSelection> &selections);

public slots:

    // to notify when the user has made a new selection
    void slotSelectionsUpdated();

private slots:

    // slot that handles when the user selects a selection
    void slotSelectionSelected(QModelIndex index);
    // slots to handle when the user wants to export the selection to a file
    void slotExportSelection();
    // slot to handle when the user wants to remove a selection
    void slotRemoveSelection();
    // slot to handle when the user wants to edit a selection
    void slotEditSelection();
    // this slot will init and show the DEA dialog (requires two selected
    // selections)
    void slotPerformDEA();
    // this slot will get the selection's image and create dialog to show it
    void slotShowTissue();
    // to save a selection in the cloud
    void slotSaveSelection();
    // to show the aggregated gene counts of the selection in a table
    void slotShowTable();
    // to import a selection from a file
    void slotImportSelection();

protected:
    void showEvent(QShowEvent *event);

private:
    // internal function to invoke the download of genes selections
    void loadSelections();

    // internal clear focus and default status for the buttons
    void clearControls();

    // to retrieve the table's model
    QSortFilterProxyModel *selectionsProxyModel();
    UserSelectionsItemModel *selectionsModel();

    // Ui object
    QScopedPointer<Ui::UserSelections> m_ui;
    // reference to dataProxy
    QSharedPointer<DataProxy> m_dataProxy;
    // selections widget where the aggregated genes can be shown in a table
    QScopedPointer<SelectionsWidget> m_selectionsWidget;
    // waiting spinner
    QScopedPointer<WaitingSpinnerWidget> m_waiting_spinner;

    Q_DISABLE_COPY(UserSelectionsPage)
};

#endif /* USERSELECTIONSPAGE_H */

#ifndef USERSELECTIONSPAGE_H
#define USERSELECTIONSPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <memory>

#include "viewPages/SelectionGenesWidget.h"
#include "viewPages/SelectionSpotsWidget.h"
#include "ext/QtWaitingSpinner/waitingspinnerwidget.h"

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

// TODO factor out the confirmation dialog
// TODO add option to show right click with mouse in selection (open, copy,
// edit...)
// TODO implement tsne clustering
// TODO implement DEA analysis
// TODO add import/export selections option
class UserSelectionsPage : public QWidget
{
    Q_OBJECT

public:
    UserSelectionsPage(QWidget *parent = 0);
    virtual ~UserSelectionsPage();

    // clear the loaded content
    void clean();

signals:

public slots:

    // slot to add a new selection to the list
    void slotAddSelection(const UserSelection& selection);

private slots:

    // slot that handles when the user selects a selection
    void slotSelectionSelected(QModelIndex index);
    // slots to handle when the user wants to export the selection to a file
    void slotExportSelection();
    // slot to handle when the user wants to remove a selection
    void slotRemoveSelection();
    // slot to handle when the user wants to edit a selection
    void slotEditSelection();
    // this slot will init and show the D.E.A. dialog (requires two selected
    // selections)
    void slotPerformDEA();
    // slot to perform a correlation analysis between two selections
    void slotPerformCorrelation();
    // slot to perform a tSNE analysis on 1 or more selections
    void slotPerformClustering();
    // this slot will get the selection's image and create dialog to show it
    void slotShowTissue();
    // slot to show the aggregated gene counts of the selection in a table
    void slotShowGenes();
    // slot to show the aggregated spot counts of the selection in a table
    void slotShowSpots();
    // to import a selection from a file
    void slotImportSelection();

protected:
    void showEvent(QShowEvent *event);

private:

    // internal clear focus and default status for the buttons
    void clearControls();

    // to retrieve the table's model
    QSortFilterProxyModel *selectionsProxyModel();
    UserSelectionsItemModel *selectionsModel();

    // Ui object
    QScopedPointer<Ui::UserSelections> m_ui;
    // the list of selections objects
    QList<UserSelection> m_selections;
    // selections widget where the aggregated genes can be shown in a table
    QScopedPointer<SelectionGenesWidget> m_genesWidget;
    // selections widget where the aggregated genes can be shown in a table
    QScopedPointer<SelectionSpotsWidget> m_spotsWidget;
    // waiting spinner
    QScopedPointer<WaitingSpinnerWidget> m_waiting_spinner;

    Q_DISABLE_COPY(UserSelectionsPage)
};

#endif /* USERSELECTIONSPAGE_H */

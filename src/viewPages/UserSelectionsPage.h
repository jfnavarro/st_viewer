/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

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
// Users can interact here to edit/remove selections and to perform analysis on them
// like the DEA, PCA, etc..
// TODO factor out the retrieval of selected items from the table
// TODO factor out the confirmation dialog
// TODO add option to remove multiple selections
// TODO add option to show selection/s in the cell view
// TODO add option to show right click with mouse in selection (open, copy, edit...)
// TODO add initial clustering option
// TODO add import selections option
// TODO refactor file explorers to get a file
// TODO geneExport and featureExport should be one slot and take a type
// FeaturesExporter and GenesExporter should be one class that can take a type or
// make them inherit of a common class
// TODO add ST icon to the widget
// TODO allow to change color of multiple selections (maybe right click mouse)
// TODO add posibility to edit and save objects in the table
class UserSelectionsPage : public QWidget
{
    Q_OBJECT

public:
    UserSelectionsPage(QPointer<DataProxy> dataProxy, QWidget* parent = 0);
    virtual ~UserSelectionsPage();

    // clear the loaded content
    void clean();

signals:

    // to notify the cell view
    void signalClearSelections();
    void signalShowSelections(const QVector<UserSelection>& selections);

public slots:

    // to notify when the user has made a new selection
    void slotSelectionsUpdated();

private slots:

    // slot that handles when the user selects a selection
    void slotSelectionSelected(QModelIndex index);
    // slots to handle actions from buttons
    void slotExportSelection();
    // void slotExportSelectionFeatures();
    void slotRemoveSelection();
    void slotEditSelection();
    // this slot will init and show the DEA dialog
    void slotPerformDEA();
    // this slot will get the selection's image and create dialog to show it
    void slotShowTissue();
    // used to be notified when the genes selections has been downloaded or updated from network
    // status contains the status of the operation (ok, abort, error)
    // type contains the type of download request
    void slotDownloadFinished(const DataProxy::DownloadStatus status,
                              const DataProxy::DownloadType type);
    // to save export the selection to a file
    void slotSaveSelection();
    // to show the genes in the selection in a table
    void slotShowTable();
    // to export the aggregated genes to file (show file dialog)
    void slotExportGenes();
    // to import a selection from file
    void slotImportSelection();

protected:
    void showEvent(QShowEvent* event);

private:
    // internal function to invoke the download of genes selections
    void loadSelections();

    // internal clear focus and default status for the buttons
    void clearControls();

    // to retrieve the table's model
    QSortFilterProxyModel* selectionsProxyModel();
    UserSelectionsItemModel* selectionsModel();

    std::unique_ptr<Ui::UserSelections> m_ui;
    // reference to dataProxy
    QPointer<DataProxy> m_dataProxy;
    // selections widget where the aggregated genes can be shown in a table
    QPointer<SelectionsWidget> m_selectionsWidget;
    // waiting spinner
    QPointer<WaitingSpinnerWidget> m_waiting_spinner;

    Q_DISABLE_COPY(UserSelectionsPage)
};

#endif /* USERSELECTIONSPAGE_H */

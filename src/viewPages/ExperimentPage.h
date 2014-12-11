/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTPAGE_H
#define EXPERIMENTPAGE_H

#include <memory>
#include <QModelIndex>
#include "data/DataProxy.h"
#include "Page.h"

class ExperimentsItemModel;
class QSortFilterProxyModel;
class AnalysisDEA;

namespace Ui{
class Experiments;
} // namespace Ui //

//Experiments page contains a table with the genes selection made by the user
//Users can interact here to edit/remove selections and to perform analysis on them
//like the DEA
class ExperimentPage : public Page
{
    Q_OBJECT

public:

    ExperimentPage(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~ExperimentPage();

public slots:

    void onEnter() override;
    void onExit() override;

private slots:

    //slot that handles when the user selects a selection
    void slotSelectionSelected(QModelIndex index);

    //slots to handle actions from buttons
    void slotExportSelection();
    void slotRemoveSelection();
    void slotEditSelection();
    void slotPerformDDA();

    //used to be notified when the genes selections has been downloaded or updated from network
    //status contains the status of the operation (ok, abort, error)
    //type contains the type of download request
    void slotDownloadFinished(const DataProxy::DownloadStatus status,
                              const DataProxy::DownloadType type);

private:

    //internal function to invoke the download of genes selections
    void loadSelections();

    //internal clear focus and default status for the buttons
    void clearControls();

    //to retrieve the table's model
    QSortFilterProxyModel *selectionsProxyModel();
    ExperimentsItemModel *selectionsModel();

    std::unique_ptr<Ui::Experiments> m_ui;

    Q_DISABLE_COPY(ExperimentPage)
};

#endif  /* EXPERIMENTPAGE_H */


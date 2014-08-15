/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DatasetPage.h"

#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QMessageBox>

#include "data/DataProxy.h"
#include "data/DataStore.h"

#include "network/DownloadManager.h"

#include "error/Error.h"

#include "model/DatasetItemModel.h"

#include "utils/Utils.h"

#include "dialogs/EditDatasetDialog.h"

#include "ui_datasets.h"

DatasetPage::DatasetPage(QPointer<DataProxy> dataProxy, QWidget *parent) :
    Page(parent),
    m_ui(nullptr),
    m_dataProxy(dataProxy)
{
    Q_ASSERT(!m_dataProxy.isNull());

    // create UI
    m_ui = new Ui::DataSets();
    m_ui->setupUi(this);

    //connect signals
    connect(m_ui->filterLineEdit, SIGNAL(textChanged(QString)), datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->datasets_tableview, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotDatasetSelected(QModelIndex)));
    connect(m_ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(m_ui->next, SIGNAL(clicked(bool)), this, SIGNAL(moveToNextPage()));
    connect(m_ui->refresh, SIGNAL(clicked(bool)), this, SLOT(slotRefreshDatasets()));
    connect(m_ui->deleteDataset, SIGNAL(clicked(bool)), this, SLOT(slotRemoveDataset()));
    connect(m_ui->editDataset, SIGNAL(clicked(bool)), this, SLOT(slotEditDataset()));
    connect(m_ui->openDataset, SIGNAL(clicked(bool)), this, SLOT(slotOpenDataset()));
}

DatasetPage::~DatasetPage()
{
    if (m_ui != nullptr) {
        delete m_ui;
    }
    m_ui = nullptr;
}

QSortFilterProxyModel *DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel *datasetsProxyModel =
            qobject_cast<QSortFilterProxyModel*>(m_ui->datasets_tableview->model());
    Q_ASSERT(datasetsProxyModel);
    return datasetsProxyModel;
}

DatasetItemModel *DatasetPage::datasetsModel()
{
    DatasetItemModel *model =
            qobject_cast<DatasetItemModel*>(datasetsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void DatasetPage::onEnter()
{
    slotLoadDatasets();

    //clear selection/focus
    m_ui->datasets_tableview->clearSelection();
    m_ui->datasets_tableview->clearFocus();
    m_ui->back->clearFocus();
    m_ui->refresh->clearFocus();
    m_ui->next->clearFocus();
    m_ui->deleteDataset->clearFocus();
    m_ui->editDataset->clearFocus();
    m_ui->openDataset->clearFocus();

    //controls disable by default
    m_ui->deleteDataset->setEnabled(false);
    m_ui->editDataset->setEnabled(false);
    m_ui->openDataset->setEnabled(false);
}

void DatasetPage::onExit()
{

}

void DatasetPage::slotDatasetSelected(QModelIndex index)
{
    const auto selected = m_ui->datasets_tableview->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);
    if (currentDataset.empty() || currentDataset.first().isNull()) {
        return;
    }

    m_ui->deleteDataset->setEnabled(index.isValid());
    m_ui->editDataset->setEnabled(index.isValid());
    m_ui->openDataset->setEnabled(index.isValid());
}

void DatasetPage::slotLoadDatasets()
{
    setWaiting(true);
    async::DataRequest request = m_dataProxy->loadDatasets();
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO show the error present in request.getErrors()
        showError(tr("Data Error"), tr("Error loading the datasets"));
    } else {
        // refresh datasets on the model
        datasetsModel()->loadDatasets(m_dataProxy->getDatasetList());
    }

    m_ui->datasets_tableview->clearSelection();
    m_ui->datasets_tableview->clearFocus();
    m_ui->deleteDataset->setEnabled(false);
    m_ui->editDataset->setEnabled(false);
    m_ui->openDataset->setEnabled(false);
}

void DatasetPage::slotRefreshDatasets()
{
    slotLoadDatasets();
}

void DatasetPage::slotEditDataset()
{
    const auto selected = m_ui->datasets_tableview->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);

    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    //currentDataset should only have one element
    auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    QScopedPointer<EditDatasetDialog> editdataset(new EditDatasetDialog(this,
                                                                        Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editdataset->setName(dataset->name());
    editdataset->setComment(dataset->statComments());

    if (editdataset->exec() == EditDatasetDialog::Accepted) {
        if ((editdataset->getName() != dataset->name()
                || editdataset->getComment() != dataset->statComments())
                && !editdataset->getName().isEmpty() && !editdataset->getName().isNull()) {

            const QString name = dataset->name();
            const QString comment = dataset->statComments();
            dataset->name(editdataset->getName());
            dataset->statComments(editdataset->getComment());

            //update the dataset
            setWaiting(true, "Updating dataset...");
            async::DataRequest request = m_dataProxy->updateDataset(dataset);
            setWaiting(false);

            if (request.return_code() == async::DataRequest::CodeError
                    || request.return_code() == async::DataRequest::CodeAbort) {
                //TODO get error from request
                dataset->name(name);
                dataset->statComments(comment);
                showError(tr("Update Dataset"), tr("Error updating the dataset"));
            } else {
                showInfo(tr("Update Dataset"), tr("Dataset updated successfully"));
            }

            //refresh dataset list
            slotLoadDatasets();
        }
    }
}

void DatasetPage::slotOpenDataset()
{
    const auto selected = m_ui->datasets_tableview->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);

    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    //currentDataset should only have one element
    auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());
    Q_ASSERT(!dataset->id().isEmpty());

    //updates state of DataProxy and move to next page
    m_dataProxy->setSelectedDataset(dataset->id());
    emit moveToNextPage();
}

void DatasetPage::slotRemoveDataset()
{
    const auto selected = m_ui->datasets_tableview->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);

    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    const int answer = QMessageBox::warning(
                this, tr("Remove Dataset"),
                tr("Are you really sure you want to remove the dataset?"),
                QMessageBox::No | QMessageBox::Escape,
                QMessageBox::Yes | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    //currentDataset should only have one element
    auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    //sets enabled to false
    //TODO changing the name is a temp hack so we can edit datasets
    //with the same name as the deleted one
    const QString datasetName = dataset->name();
    dataset->enabled(false);
    dataset->name(datasetName + "_REMOVED_FROM_STVI");

    //update the dataset
    setWaiting(true, "Removing dataset...");
    async::DataRequest request = m_dataProxy->updateDataset(dataset);
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO get error from request
        dataset->enabled(true);
        dataset->name(datasetName);
        showError(tr("Remove Dataset"), tr("Error removing the dataset"));
    } else {

        //set enable to false for the selections made on the dataset
        const auto& datasetSelections = m_dataProxy->getGeneSelections();
        bool errorDeletingSelections = false;
        for(auto selection : datasetSelections) {
            if (selection->datasetId() == dataset->id()) {
                const QString selName = selection->name();
                selection->enabled(false);
                selection->name(selName + "_REMOVED_FROM_STVI");
                async::DataRequest request = m_dataProxy->updateGeneSelection(selection);
                if (request.return_code() == async::DataRequest::CodeError
                        || request.return_code() == async::DataRequest::CodeAbort) {
                    selection->enabled(true);
                    selection->name(selName);
                    errorDeletingSelections = true;
                }
            }
        }

        if (errorDeletingSelections) {
            showInfo(tr("Remove Dataset"),
                     tr("Dataset removed successfully but\n there was an error removing its selections"));
        } else {
            showInfo(tr("Remove Dataset"), tr("Dataset removed successfully"));
        }
    }

    //refresh dataset list
    slotLoadDatasets();
    //TODO temp hack to reset the current selected dataset in
    //case we are removing the current selected dataset
    //this logic should be handled in dataProxy
    if (dataset->id() == m_dataProxy->getSelectedDataset()) {
        m_dataProxy->setSelectedDataset(QString());
    }
}

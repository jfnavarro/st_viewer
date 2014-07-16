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

    m_ui->deleteDataset->setEnabled(false);
    m_ui->editDataset->setEnabled(false);
    m_ui->openDataset->setEnabled(false);
}

void DatasetPage::onExit()
{

}

void DatasetPage::slotDatasetSelected(QModelIndex index)
{
    //TODO check selected dataset is valid
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
}

void DatasetPage::slotRefreshDatasets()
{
    //TODO make sure dataset cache does not need to be cleaned here
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
        if (editdataset->getName() != dataset->name()
                && editdataset->getComment() != dataset->statComments()) {

            //TODO check that name is not empty

            dataset->name(editdataset->getName());
            dataset->statComments(editdataset->getComment());

            //update the dataset
            setWaiting(true);
            async::DataRequest request = m_dataProxy->updateDataset(dataset);
            setWaiting(false);

            if (request.return_code() == async::DataRequest::CodeError
                    || request.return_code() == async::DataRequest::CodeAbort) {
                //TODO get error from request
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

    //updates state of  DataProxy and move to next page
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
    dataset->enabled(false);

    //update the dataset
    setWaiting(true);
    async::DataRequest request = m_dataProxy->updateDataset(dataset);
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO get error from request
        showError(tr("Remove Dataset"), tr("Error removing the dataset"));
    } else {
        showInfo(tr("Remove Dataset"), tr("Dataset removed successfully"));
    }

    //TODO remove selections performs in the datasets
    //TODO update selection status (buttoms)

    //refresh dataset list
    slotLoadDatasets();
}

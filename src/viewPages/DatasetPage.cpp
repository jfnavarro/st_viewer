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
#include "error/Error.h"
#include "model/DatasetItemModel.h"
#include "utils/Utils.h"
#include "dialogs/EditDatasetDialog.h"

#include "ui_datasets.h"

DatasetPage::DatasetPage(QPointer<DataProxy> dataProxy, QWidget *parent) :
    Page(parent),
    m_ui(new Ui::DataSets()),
    m_dataProxy(dataProxy)
{
    Q_ASSERT(!m_dataProxy.isNull());

    m_ui->setupUi(this);

    //connect signals
    connect(m_ui->filterLineEdit, SIGNAL(textChanged(QString)), datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->datasets_tableview, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotDatasetSelected(QModelIndex)));
    connect(m_ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(m_ui->next, SIGNAL(clicked(bool)), this, SIGNAL(moveToNextPage()));
    connect(m_ui->refresh, SIGNAL(clicked(bool)), this, SLOT(slotLoadDatasets()));
    connect(m_ui->deleteDataset, SIGNAL(clicked(bool)), this, SLOT(slotRemoveDataset()));
    connect(m_ui->editDataset, SIGNAL(clicked(bool)), this, SLOT(slotEditDataset()));
    connect(m_ui->openDataset, SIGNAL(clicked(bool)), this, SLOT(slotOpenDataset()));

    //connect abort signal
    connect(this, SIGNAL(signalDownloadCancelled()),
            m_dataProxy.data(), SLOT(slotAbortActiveDownloads()));

    //connect data proxy signals
    connect(m_dataProxy.data(),
            SIGNAL(signalDatasetsDownloaded(DataProxy::DownloadStatus)),
            this, SLOT(slotDatasetsDownloaded(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalDatasetsModified(DataProxy::DownloadStatus)),
            this, SLOT(slotDatasetsModified(DataProxy::DownloadStatus)));
}

DatasetPage::~DatasetPage()
{
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
}

void DatasetPage::onExit()
{

}

void DatasetPage::clearControls()
{
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
    m_dataProxy->loadDatasets();
}

void DatasetPage::slotDatasetsDownloaded(DataProxy::DownloadStatus status)
{
    setWaiting(false);

    //if error we reset the selected dataset...this is why we allow free navigation
    //among pages
    if (status == DataProxy::Failed) {
        m_dataProxy->resetSelectedDataset();
    }

    //if download was not aborted we reset the datasets model
    if (status != DataProxy::Aborted) {
        datasetsModel()->loadDatasets(m_dataProxy->getDatasetList());
    }

    clearControls();
}

void DatasetPage::slotEditDataset()
{
    const auto selected = m_ui->datasets_tableview->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);

    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    //currentDataset should only have one element
    Q_ASSERT(!currentDataset.first().isNull());
    Dataset dataset(*currentDataset.first());

    QScopedPointer<EditDatasetDialog> editdataset(new EditDatasetDialog(this,
                                                                        Qt::CustomizeWindowHint
                                                                        | Qt::WindowTitleHint));
    editdataset->setName(dataset.name());
    editdataset->setComment(dataset.statComments());

    if (editdataset->exec() == EditDatasetDialog::Accepted
            && (editdataset->getName() != dataset.name()
                || editdataset->getComment() != dataset.statComments())
            && !editdataset->getName().isEmpty()
            && !editdataset->getName().isNull()) {

        dataset.name(editdataset->getName());
        dataset.statComments(editdataset->getComment());

        //update the dataset
        m_dataProxy->updateDataset(dataset);
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
    m_dataProxy->setSelectedDataset(dataset);
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
    const auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    //remove the dataset
    m_dataProxy->removeDataset(dataset->id());
}

void DatasetPage::slotDatasetsModified(DataProxy::DownloadStatus status)
{
    if (status == DataProxy::Success) {
        slotLoadDatasets();
    }
}

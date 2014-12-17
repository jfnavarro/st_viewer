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

#include "error/Error.h"
#include "model/DatasetItemModel.h"
#include "utils/Utils.h"
#include "dialogs/EditDatasetDialog.h"
#include "ui_datasets.h"

using namespace Globals;

DatasetPage::DatasetPage(QPointer<DataProxy> dataProxy, QWidget *parent) :
    Page(dataProxy, parent),
    m_ui(new Ui::DataSets())
{
    m_ui->setupUi(this);

    //setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    setWindowFlags(Qt::FramelessWindowHint);
    m_ui->DatasetPageWidget->setStyleSheet("QWidget#DatasetPageWidget " + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    //connect signals
    connect(m_ui->filterLineEdit, SIGNAL(textChanged(QString)), datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->datasetsTableView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotDatasetSelected(QModelIndex)));
    connect(m_ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(m_ui->next, SIGNAL(clicked(bool)), this, SIGNAL(moveToNextPage()));
    connect(m_ui->refresh, SIGNAL(clicked(bool)), this, SLOT(slotLoadDatasets()));
    connect(m_ui->deleteDataset, SIGNAL(clicked(bool)), this, SLOT(slotRemoveDataset()));
    connect(m_ui->editDataset, SIGNAL(clicked(bool)), this, SLOT(slotEditDataset()));
    connect(m_ui->openDataset, SIGNAL(clicked(bool)), this, SLOT(slotOpenDataset()));

    //connect data proxy signal
    connect(m_dataProxy.data(),
            SIGNAL(signalDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)),
            this, SLOT(slotDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)));

    clearControls();
}

DatasetPage::~DatasetPage()
{
}

QSortFilterProxyModel *DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel *datasetsProxyModel =
            qobject_cast<QSortFilterProxyModel*>(m_ui->datasetsTableView->model());
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
    clearControls();
}

void DatasetPage::clearControls()
{
    //clear selection/focus
    m_ui->datasetsTableView->clearSelection();
    m_ui->datasetsTableView->clearFocus();
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
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
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
    if (!m_dataProxy->userLogIn()) {
        return;
    }

    //download datasets (enable blocking loading bar)
    setWaiting(true);
    m_dataProxy->loadDatasets();
    m_dataProxy->activateCurrentDownloads();
}

void DatasetPage::datasetsDownloaded(const DataProxy::DownloadStatus status)
{
    //disable blocking loading bar
    setWaiting(false);

    //if error we reset the selected dataset...this is because we allow free navigation
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
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);

    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    //currentDataset should only have one element
    Q_ASSERT(!currentDataset.first().isNull());
    Dataset dataset(*currentDataset.first());

    QScopedPointer<EditDatasetDialog> editdataset(new EditDatasetDialog(this,
                                                                        Qt::Dialog
                                                                        | Qt::CustomizeWindowHint
                                                                        | Qt::WindowTitleHint));
    editdataset->setWindowIcon(QIcon());
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
        m_dataProxy->activateCurrentDownloads();
    }
}

void DatasetPage::slotOpenDataset()
{
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
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
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);

    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    const int answer = QMessageBox::warning(
                this, tr("Remove Dataset"),
                tr("Are you really sure you want to remove the dataset?"),
                QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    //currentDataset should only have one element
    const auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    //remove the dataset
    m_dataProxy->removeDataset(dataset->id());
    m_dataProxy->activateCurrentDownloads();
}

void DatasetPage::slotDownloadFinished(const DataProxy::DownloadStatus status,
                                       const DataProxy::DownloadType type)
{
    if (type == DataProxy::DatasetDownloaded) {
        datasetsDownloaded(status);
    } else if (type == DataProxy::DatasetModified && status == DataProxy::Success) {
        slotLoadDatasets();
    }
}

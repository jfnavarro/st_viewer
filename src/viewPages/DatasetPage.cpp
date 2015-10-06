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
#include "dataModel/Dataset.h"
#include "error/Error.h"
#include "model/DatasetItemModel.h"
#include "utils/Utils.h"
#include "dialogs/EditDatasetDialog.h"
#include "QtWaitingSpinner/waitingspinnerwidget.h"

#include "ui_datasetsPage.h"

using namespace Globals;

DatasetPage::DatasetPage(QPointer<DataProxy> dataProxy, QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DataSets())
    , m_dataProxy(dataProxy)
    , m_waiting_spinner(nullptr)
{
    m_ui->setupUi(this);

    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    m_ui->DatasetPageWidget->setStyleSheet("QWidget#DatasetPageWidget " + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // initialize waiting spinner
    m_waiting_spinner = new WaitingSpinnerWidget(this, true, true);
    m_waiting_spinner->setRoundness(70.0);
    m_waiting_spinner->setMinimumTrailOpacity(15.0);
    m_waiting_spinner->setTrailFadePercentage(70.0);
    m_waiting_spinner->setNumberOfLines(12);
    m_waiting_spinner->setLineLength(20);
    m_waiting_spinner->setLineWidth(10);
    m_waiting_spinner->setInnerRadius(20);
    m_waiting_spinner->setRevolutionsPerSecond(1);
    m_waiting_spinner->setColor(QColor(0,155,60));

    // connect signals
    connect(m_ui->filterLineEdit,
            SIGNAL(textChanged(QString)),
            datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->datasetsTableView,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(slotDatasetSelected(QModelIndex)));
    connect(m_ui->datasetsTableView,
            SIGNAL(doubleClicked(QModelIndex)),
            this,
            SLOT(slotSelectAndOpenDataset(QModelIndex)));
    connect(m_ui->refresh, SIGNAL(clicked(bool)), this, SLOT(slotLoadDatasets()));
    connect(m_ui->deleteDataset, SIGNAL(clicked(bool)), this, SLOT(slotRemoveDataset()));
    connect(m_ui->editDataset, SIGNAL(clicked(bool)), this, SLOT(slotEditDataset()));
    connect(m_ui->openDataset, SIGNAL(clicked(bool)), this, SLOT(slotOpenDataset()));

    // connect data proxy signal
    connect(m_dataProxy.data(),
            SIGNAL(signalDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)),
            this,
            SLOT(slotDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)));

    clearControls();
}

DatasetPage::~DatasetPage()
{
}

void DatasetPage::clean()
{
    datasetsModel()->clear();
    clearControls();
}

QSortFilterProxyModel* DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel* datasetsProxyModel
        = qobject_cast<QSortFilterProxyModel*>(m_ui->datasetsTableView->model());
    Q_ASSERT(datasetsProxyModel);
    return datasetsProxyModel;
}

DatasetItemModel* DatasetPage::datasetsModel()
{
    DatasetItemModel* model = qobject_cast<DatasetItemModel*>(datasetsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void DatasetPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    clearControls();
    slotLoadDatasets();
}

void DatasetPage::clearControls()
{
    // clear selection/focus
    m_ui->datasetsTableView->clearSelection();
    m_ui->datasetsTableView->clearFocus();
    m_ui->refresh->clearFocus();
    m_ui->deleteDataset->clearFocus();
    m_ui->editDataset->clearFocus();
    m_ui->openDataset->clearFocus();

    // controls disable by default
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

void DatasetPage::slotSelectAndOpenDataset(QModelIndex index)
{
    slotDatasetSelected(index);
    slotOpenDataset();
}

void DatasetPage::slotLoadDatasets()
{
    if (!m_dataProxy->userLogIn()) {
        return;
    }

    // download datasets
    m_waiting_spinner->start();
    m_dataProxy->loadDatasets();
    m_dataProxy->activateCurrentDownloads();
}

void DatasetPage::datasetsDownloaded(const DataProxy::DownloadStatus status)
{
    // if error we reset the selected dataset...this is because we allow free navigation
    // among pages
    if (status == DataProxy::Failed) {
        m_dataProxy->resetSelectedDataset();
    }

    // if download was not aborted we reset the datasets model
    if (status != DataProxy::Aborted) {
        datasetsModel()->loadDatasets(m_dataProxy->getDatasetList());
    }

    m_waiting_spinner->stop();
    clearControls();
}

void DatasetPage::slotEditDataset()
{
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);
    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    // currentDataset should only have one element
    auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    QScopedPointer<EditDatasetDialog> editdataset(
        new EditDatasetDialog(this, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editdataset->setWindowIcon(QIcon());
    editdataset->setName(dataset->name());
    editdataset->setComment(dataset->statComments());

    if (editdataset->exec() == EditDatasetDialog::Accepted
        && (editdataset->getName() != dataset->name()
            || editdataset->getComment() != dataset->statComments())
        && !editdataset->getName().isEmpty()
        && !editdataset->getName().isNull()) {

        dataset->name(editdataset->getName());
        dataset->statComments(editdataset->getComment());

        // update the dataset
        m_waiting_spinner->start();
        m_dataProxy->updateDataset(*dataset);
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

    // currentDataset should only have one element
    auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    // update data proxy with selected dataset
    m_dataProxy->setSelectedDataset(dataset);

    // Now we proceed to download the dataset content
    // TODO when caching is on we should not download
    // the dataset content if it is already downloaded
    // and in sync with the server

    // first we need to download the image alignment
    m_waiting_spinner->start();
    m_dataProxy->loadImageAlignment();
    m_dataProxy->activateCurrentDownloads();
}

void DatasetPage::slotRemoveDataset()
{
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDataset = datasetsModel()->getDatasets(selected);
    if (currentDataset.empty() || currentDataset.size() > 1) {
        return;
    }

    // currentDataset should only have one element
    const auto dataset = currentDataset.first();
    Q_ASSERT(!dataset.isNull());

    const int answer
        = QMessageBox::warning(this,
                               tr("Remove Dataset"),
                               tr("Are you really sure you want to remove the dataset?"),
                               QMessageBox::Yes,
                               QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    // remove the dataset
    m_waiting_spinner->start();
    m_dataProxy->removeDataset(dataset->id());
    m_dataProxy->activateCurrentDownloads();
}

void DatasetPage::slotDownloadFinished(const DataProxy::DownloadStatus status,
                                       const DataProxy::DownloadType type)
{
    const bool isSuccess = status == DataProxy::Success;
    const bool isLastDownload = m_dataProxy->getActiveDownloads() == 0;

    if (type == DataProxy::DatasetsDownloaded) {
        // We have downloaded datasets, update the view
        datasetsDownloaded(status);
    } else if (type == DataProxy::DatasetModified && isSuccess) {
        // We have edited a dataset, re-upload them
        slotLoadDatasets();
    } else if (type == DataProxy::ImageAlignmentDownloaded) {
        // We are downloading dataset content, first is to download image alignment
        if (isSuccess) {
            // download the rest of the content
            m_dataProxy->loadDatasetContent();
            m_dataProxy->activateCurrentDownloads();
        } else {
            // something happened downloading the image alignment
            m_waiting_spinner->stop();
            QMessageBox::critical(this,
                                  tr("Image alignment"),
                                  tr("There was an error downloading the image alignment!"));
        }
    } else if (type == DataProxy::ChipDownloaded
               || type == DataProxy::TissueImageDownloaded
               || type == DataProxy::FeaturesDownloaded) {
        if (!isSuccess && isLastDownload) {
            // so this was the last download processed
            // of the dataset content but it failed or aborted
            m_waiting_spinner->stop();
            QMessageBox::critical(this,
                                  tr("Dataset content"),
                                  tr("There was an error downloading the dataset content!"));
        } else if (isSuccess && isLastDownload) {
            // so this was the last download of dataset content and it was OK
            // updates state of DataProxy and notify that dataset is open
            // TODO we should get the dataset Id in a different way
            m_waiting_spinner->stop();
            emit signalDatasetOpen(m_dataProxy->getSelectedDataset()->id());
        }
    }
}

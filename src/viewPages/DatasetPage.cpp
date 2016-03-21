#include "DatasetPage.h"

#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QUuid>
#include "dataModel/Dataset.h"
#include "error/Error.h"
#include "model/DatasetItemModel.h"
#include "utils/Utils.h"
#include "dialogs/EditDatasetDialog.h"
#include "QtWaitingSpinner/waitingspinnerwidget.h"
#include "data/DatasetImporter.h"
#include "dataModel/Chip.h"
#include "dataModel/ImageAlignment.h"
#include "dataModel/Dataset.h"
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
    connect(m_ui->importDataset, SIGNAL(clicked(bool)), this, SLOT(slotImportDataset()));

    // connect data proxy signal
    connect(m_dataProxy.data(),
            SIGNAL(signalDatasetsDownloaded(DataProxy::DownloadStatus)),
            this,
            SLOT(slostDatasetsDownloaded(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalDatasetModified(DataProxy::DownloadStatus)),
            this,
            SLOT(slotDatasetModified(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalDatasetRemoved(DataProxy::DownloadStatus)),
            this,
            SLOT(slotDatasetModified(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalImageAlignmentDownloaded(DataProxy::DownloadStatus)),
            this,
            SLOT(slotImageAlignmentDownloaded(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalChipDownloaded(DataProxy::DownloadStatus)),
            this,
            SLOT(slotDatasetContentDownloaded(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalFeaturesDownloaded(DataProxy::DownloadStatus)),
            this,
            SLOT(slotDatasetContentDownloaded(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalTissueImageDownloaded(DataProxy::DownloadStatus)),
            this,
            SLOT(slotDatasetContentDownloaded(DataProxy::DownloadStatus)));

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

void DatasetPage::slostDatasetsDownloaded(const DataProxy::DownloadStatus status)
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

        // update the dataset on the cloud if it is downloaded
        if (dataset->downloaded()) {
            m_waiting_spinner->start();
            m_dataProxy->updateDataset(*dataset);
            m_dataProxy->activateCurrentDownloads();
        }
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
    // if it is downloaded from the cloud, otherwise just import the dataset
    m_waiting_spinner->start();
    if (dataset->downloaded()) {
        // first we need to download the image alignment
        m_dataProxy->loadImageAlignment();
        m_dataProxy->activateCurrentDownloads();
    } else {
        const auto importer = m_importedDatasets.value(dataset->id());
        Q_ASSERT(importer);
        bool parsedOk = true;
        //TODO feature and image files could be empty (make a check)
        Chip chip;
        const QRect chip_rect = importer->chipDimensions();
        const int x1 = chip_rect.topLeft().x();
        const int y1 = chip_rect.topLeft().y();
        const int x2 = chip_rect.bottomRight().x();
        const int y2 = chip_rect.bottomRight().y();
        chip.id(QUuid::createUuid().toString());
        chip.name(chip.id());
        chip.x1(x1);
        chip.x1Border(x1 - 1);
        chip.x1Total(x1 - 2);
        chip.y1(y1);
        chip.y1Border(y1 - 1);
        chip.y1Total(y1 - 2);
        chip.x2(x2);
        chip.x2Border(x2 + 1);
        chip.x2Total(x2 + 2);
        chip.y2(y2);
        chip.y2Border(y2 + 1);
        chip.y2Total(y2 + 2);
        m_dataProxy->loadChip(chip);

        parsedOk &= m_dataProxy->parseFeatures(importer->featuresFile());

        ImageAlignment alignment;
        alignment.id(QUuid::createUuid().toString());
        alignment.chipId(chip.id());
        alignment.name(alignment.id());
        const QString mainImageName = QUuid::createUuid().toString();
        const QString secondImageName = QUuid::createUuid().toString();
        alignment.figureBlue(mainImageName);
        alignment.figureRed(secondImageName);
        alignment.alignment(importer->alignmentMatrix());
        m_dataProxy->loadImageAlignment(alignment);

        parsedOk &= m_dataProxy->parseCellTissueImage(importer->mainImageFile(), mainImageName);
        parsedOk &= m_dataProxy->parseCellTissueImage(importer->secondImageFile(), secondImageName);

        m_waiting_spinner->stop();
        if (parsedOk) {
            emit signalDatasetOpen(m_dataProxy->getSelectedDataset()->id());
        } else {
            QMessageBox::critical(this,
                                  tr("Dataset content"),
                                  tr("Error loading dataset content"));
        }
    }
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

    // remove the dataset on the cloud if it is downloaded
    //TODO do something when the dataset being removed is the currently opened
    if (dataset->downloaded()) {
        m_waiting_spinner->start();
        m_dataProxy->removeDataset(dataset->id());
        m_dataProxy->activateCurrentDownloads();
    } else {
        m_dataProxy->parseRemoveDataset(dataset->id());
        datasetsModel()->loadDatasets(m_dataProxy->getDatasetList());
    }
}

void DatasetPage::slotDatasetModified(const DataProxy::DownloadStatus status)
{
    if (status == DataProxy::Success) {
        // We have edited a dataset, re-upload them
        slotLoadDatasets();
    }
}

void DatasetPage::slotImageAlignmentDownloaded(const DataProxy::DownloadStatus status)
{
    // We are downloading dataset content, first is to download image alignment
    if (status == DataProxy::Success) {
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
}

void DatasetPage::slotDatasetContentDownloaded(const DataProxy::DownloadStatus status)
{
    const bool isSuccess = status == DataProxy::Success;
    const bool isLastDownload = m_dataProxy->getActiveDownloads() == 0;
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

void DatasetPage::slotImportDataset()
{
    QPointer<DatasetImporter> importer = new DatasetImporter();
    const int result = importer->exec();
    if (result == QDialog::Accepted) {
        const DataProxy::DatasetList& datasets = m_dataProxy->getDatasetList();
        const bool sameName = std::find_if(datasets.begin(), datasets.end(),
                                           [=](DataProxy::DatasetPtr dataset)
        {return dataset->name() == importer->datasetName();}) != datasets.end();
        if (sameName) {
            QMessageBox::critical(this,
                                  tr("Import dataset"),
                                  tr("There is a dataset with the same name!"));
        } else {
            Dataset dataset;
            dataset.id(QUuid::createUuid().toString());
            dataset.name(importer->datasetName());
            dataset.downloaded(false);
            //TODO add more fields for dataset to importer
            //TODO set created an modified as current date
            m_importedDatasets.insert(dataset.id(), importer);
            m_dataProxy->addDataset(dataset);
            datasetsModel()->loadDatasets(m_dataProxy->getDatasetList());
        }
    }
}

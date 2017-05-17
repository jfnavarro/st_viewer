#include "DatasetPage.h"

#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QUuid>
#include <QDateTime>

#include "ext/QtWaitingSpinner/waitingspinnerwidget.h"

#include "model/DatasetItemModel.h"
#include "dialogs/EditDatasetDialog.h"
#include "data/DatasetImporter.h"
#include "data/Dataset.h"
#include "SettingsStyle.h"

#include "ui_datasetsPage.h"

using namespace Style;

DatasetPage::DatasetPage(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::DataSets())
    , m_waiting_spinner(nullptr)
{
    m_ui->setupUi(this);

    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    m_ui->DatasetPageWidget->setStyleSheet("QWidget#DatasetPageWidget " + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // initialize waiting spinner
    m_waiting_spinner.reset(new WaitingSpinnerWidget(this, true, true));
    m_waiting_spinner->setRoundness(70.0);
    m_waiting_spinner->setMinimumTrailOpacity(15.0);
    m_waiting_spinner->setTrailFadePercentage(70.0);
    m_waiting_spinner->setNumberOfLines(12);
    m_waiting_spinner->setLineLength(20);
    m_waiting_spinner->setLineWidth(10);
    m_waiting_spinner->setInnerRadius(20);
    m_waiting_spinner->setRevolutionsPerSecond(1);
    m_waiting_spinner->setColor(QColor(0, 155, 60));

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

QSortFilterProxyModel *DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel *datasetsProxyModel
        = qobject_cast<QSortFilterProxyModel *>(m_ui->datasetsTableView->model());
    Q_ASSERT(datasetsProxyModel);
    return datasetsProxyModel;
}

DatasetItemModel *DatasetPage::datasetsModel()
{
    DatasetItemModel *model = qobject_cast<DatasetItemModel *>(datasetsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void DatasetPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    clearControls();
}

void DatasetPage::clearControls()
{
    // clear selection
    m_ui->datasetsTableView->clearSelection();

    // controls disable by default
    m_ui->deleteDataset->setEnabled(false);
    m_ui->editDataset->setEnabled(false);
    m_ui->openDataset->setEnabled(false);
}

void DatasetPage::slotDatasetSelected(QModelIndex index)
{
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDatasets = datasetsModel()->getDatasets(selected);
    // Check if the selection is valid
    if (!index.isValid() || currentDatasets.empty()) {
        return;
    }
    // Enable only remove if more than one selected
    const bool more_than_one = currentDatasets.size() > 1;
    m_ui->deleteDataset->setEnabled(true);
    m_ui->editDataset->setEnabled(!more_than_one);
    m_ui->openDataset->setEnabled(!more_than_one);
}

void DatasetPage::slotSelectAndOpenDataset(QModelIndex index)
{
    slotDatasetSelected(index);
    slotOpenDataset();
}

void DatasetPage::slotEditDataset()
{/*
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDatasets = datasetsModel()->getDatasets(selected);
    // Can only edit 1 valid dataset
    if (currentDatasets.size() != 1) {
        return;
    }
    const auto dataset = currentDatasets.front();

    if (dataset->downloaded()) {
        QScopedPointer<EditDatasetDialog> editdataset(
            new EditDatasetDialog(this,
                                  Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint));
        editdataset->setWindowIcon(QIcon());
        editdataset->setName(dataset->name());
        editdataset->setComment(dataset->statComments());

        if (editdataset->exec() == EditDatasetDialog::Accepted
            && (editdataset->getName() != dataset->name()
                || editdataset->getComment() != dataset->statComments())
            && !editdataset->getName().isEmpty() && !editdataset->getName().isNull()) {

            // TODO maybe check that the name does not exist
            dataset->name(editdataset->getName());
            dataset->statComments(editdataset->getComment());

            // update the dataset in the database
            m_waiting_spinner->start();
            m_dataProxy->updateDataset(*dataset);
            m_waiting_spinner->stop();
            // after we edit a dataset we reload all of them
            // NOTE do not think this is necessary
            // slotDatasetsUpdated();
        }
    } else {
        const auto importer = m_importedDatasets.value(dataset->id());
        Q_ASSERT(importer);
        const int result = importer->exec();
        if (result == QDialog::Accepted) {
            // TODO maybe check that the name does not exist
            // TODO should reload the dataset automatically if it is the currently opened
        }
    }*/
}

void DatasetPage::slotOpenDataset()
{/*
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDatasets = datasetsModel()->getDatasets(selected);
    // Can only open 1 valid dataset
    if (currentDatasets.size() != 1) {
        return;
    }
    const auto dataset = currentDatasets.front();

    m_waiting_spinner->start();
    if (dataset->downloaded()) {
        if (m_dataProxy->loadDatasetContent(dataset)) {
            emit signalDatasetOpen(dataset->id());
        } else {
            QMessageBox::critical(this, tr("Dataset content"),
                                  tr("Error loading dataset content"));
            //TODO clear data in data proxy
        }
    } else {

        const auto importer = m_importedDatasets.value(dataset->id());
        Q_ASSERT(importer);
        bool parsedOk = true;

        // Create a chip with the dimensions given
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

        // add the features
        const QByteArray &featuresFile = importer->featuresFile();
        Q_ASSERT(!featuresFile.isNull() && !featuresFile.isEmpty());
        parsedOk &= m_dataProxy->loadFeatures(featuresFile);

        // creates an image alignment with the previous chip and the images
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

        // add the images
        const QByteArray &mainImageFile = importer->mainImageFile();
        Q_ASSERT(!mainImageFile.isNull() && !mainImageFile.isEmpty());
        parsedOk &= m_dataProxy->loadCellTissueImage(mainImageFile, mainImageName);
        const QByteArray &secondImageFile = importer->secondImageFile();
        if (!secondImageFile.isEmpty() && !secondImageFile.isNull()) {
            parsedOk &= m_dataProxy->loadCellTissueImage(secondImageFile, secondImageName);
        }

        if (parsedOk) {
            emit signalDatasetOpen(dataset->id());
        } else {
            QMessageBox::critical(this, tr("Dataset content"), tr("Error loading dataset content"));
            // TODO clear up the content in dataProxy
        }
    }

    m_waiting_spinner->stop();*/
}

void DatasetPage::slotRemoveDataset()
{
    /*
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDatasets = datasetsModel()->getDatasets(selected);
    // Can remove multiple datasets
    if (currentDatasets.empty()) {
        return;
    }

    const int answer
        = QMessageBox::warning(this,
                               tr("Remove Dataset"),
                               tr("Are you really sure you want to remove the dataset/s?"),
                               QMessageBox::Yes,
                               QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    m_waiting_spinner->start();
    for (const auto &dataset : currentDatasets) {
        m_dataProxy->removeDataset(dataset->id(), dataset->downloaded());
        //TODO A signal must be send to notify cell view in case the
        //dataset removed is currently opened
    }
    m_waiting_spinner->stop();
    // after we remove a dataset we refresh the model
    slotDatasetsUpdated();
    */
}

void DatasetPage::slotImportDataset()
{
    QPointer<DatasetImporter> importer = new DatasetImporter();
    const int result = importer->exec();
    // TODO check for dataset name if exists
    if (result == QDialog::Accepted) {
        // TODO maybe check that the name does not exist
        Dataset dataset;
        dataset.name(importer->datasetName());
        dataset.statComments(importer->comments());
        dataset.statSpecies(importer->species());
        dataset.statTissue(importer->tissue());
        dataset.dataFile(importer->STDataFile());
        dataset.imageAlignment(importer->alignmentMatrix());
        //TODO load spot coordiantes map
        //dataset.spotcoordinates(importer->spotsMapCoordinates());
        // add dataset and update model
        m_importedDatasets.append(dataset);
        slotDatasetsUpdated();
    } else {
        //TODO better error description
        QMessageBox::critical(this, tr("Datasert import"), tr("Error importing dataset"));
    }
}

void DatasetPage::slotDatasetsUpdated()
{
    // update model and clear controls
    datasetsModel()->loadDatasets(m_importedDatasets);
    clearControls();
}

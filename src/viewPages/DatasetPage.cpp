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
    m_importedDatasets.clear();
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
{
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDatasets = datasetsModel()->getDatasets(selected);
    // Can only edit 1 valid dataset
    if (currentDatasets.size() != 1) {
        return;
    }
    const auto dataset = currentDatasets.front();
    DatasetImporter importer(dataset);
    // Launch the dialog
    const int result = importer.exec();
    if (result == QDialog::Accepted) {
        // Check that the name does not exist
        const QString datasetName = importer.datasetName();
        if (nameExist(datasetName) && datasetName != dataset.name()) {
            QMessageBox::critical(this, tr("Datasert import"),
                                  tr("There is another dataset with the same name"));
        } else {
            const int index = m_importedDatasets.indexOf(dataset);
            Q_ASSERT(index != -1);
            Dataset updated_dataset(importer);
            m_importedDatasets.replace(index, updated_dataset);
            if (dataset == m_open_dataset) {
                m_open_dataset = updated_dataset;
                //TODO should only update if the data or image was altered
                emit signalDatasetUpdated(updated_dataset.name());
            }
            slotDatasetsUpdated();
        }
    } else {
        QMessageBox::critical(this, tr("Datasert import"), tr("Error importing dataset"));
    }
}

void DatasetPage::slotOpenDataset()
{
    const auto selected = m_ui->datasetsTableView->datasetsTableItemSelection();
    const auto currentDatasets = datasetsModel()->getDatasets(selected);
    // Can only open 1 valid dataset
    if (currentDatasets.size() != 1) {
        return;
    }
    auto dataset = currentDatasets.front();
    m_waiting_spinner->start();
    if (!dataset.load_data()) {
        QMessageBox::critical(this, tr("Datasert import"), tr("Error opening ST data file"));
    }
    m_waiting_spinner->stop();
    // Set selected dataset
    m_open_dataset = dataset;
    // Notify that the dataset was open
    emit signalDatasetOpen(dataset.name());
}

void DatasetPage::slotRemoveDataset()
{
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

    for (auto dataset: currentDatasets) {
        //TODO check the they were removed
        m_importedDatasets.removeOne(dataset);
        if (m_open_dataset == dataset) {
            emit signalDatasetRemoved(dataset.name());
            m_open_dataset = Dataset();
        }
    }

    slotDatasetsUpdated();
}

void DatasetPage::slotImportDataset()
{
    DatasetImporter importer;
    // Launch the dialog
    const int result = importer.exec();
    if (result == QDialog::Accepted) {
        // Check that the name does not exist
        const QString datasetName = importer.datasetName();
        if (nameExist(datasetName)) {
            QMessageBox::critical(this, tr("Datasert import"),
                                  tr("There is another dataset with the same name"));
        } else {
            Dataset dataset(importer);
            m_importedDatasets.append(dataset);
            slotDatasetsUpdated();
        }
    } else {
        QMessageBox::critical(this, tr("Datasert import"), tr("Error importing dataset"));
    }
}

const Dataset &DatasetPage::currentDataset() const
{
    return m_open_dataset;
}

void DatasetPage::slotDatasetsUpdated()
{
    // update model and clear controls
    datasetsModel()->loadDatasets(m_importedDatasets);
    clearControls();
}

bool DatasetPage::nameExist(const QString &name)
{
    return std::find_if(m_importedDatasets.begin(), m_importedDatasets.end(),
                        [&name](const Dataset& dataset)
    {return dataset.name() == name;}) != m_importedDatasets.end();
}

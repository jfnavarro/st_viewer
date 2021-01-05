#include "DatasetPage.h"

#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QUuid>
#include <QDateTime>

#include "model/DatasetItemModel.h"
#include "dialogs/EditDatasetDialog.h"
#include "data/DatasetImporter.h"
#include "SettingsStyle.h"

#include "ui_datasetsPage.h"

using namespace Style;

DatasetPage::DatasetPage(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::DataSets())
    , m_importedDatasets()
    , m_open_dataset()
{
    m_ui->setupUi(this);

    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    m_ui->DatasetPageWidget->setStyleSheet("QWidget#DatasetPageWidget " + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // connect signals
    connect(m_ui->filterLineEdit,
            &QLineEdit::textChanged,
            datasetsProxyModel(),
            &QSortFilterProxyModel::setFilterFixedString);
    connect(m_ui->datasetsTableView,
            &DatasetsTableView::clicked,
            this,
            &DatasetPage::slotDatasetSelected);
    connect(m_ui->datasetsTableView,
            &DatasetsTableView::doubleClicked,
            this,
            &DatasetPage::slotSelectAndOpenDataset);
    connect(m_ui->deleteDataset, SIGNAL(clicked(bool)), this, SLOT(slotRemoveDataset()));
    connect(m_ui->editDataset, SIGNAL(clicked(bool)), this, SLOT(slotEditDataset()));
    connect(m_ui->openDataset, SIGNAL(clicked(bool)), this, SLOT(slotOpenDataset()));
    connect(m_ui->importDataset, &QPushButton::clicked, this, &DatasetPage::slotImportDataset);
    connect(m_ui->datasetsTableView, SIGNAL(signalOpen(QModelIndex)),
            this, SLOT(slotOpenDataset(QModelIndex)));
    connect(m_ui->datasetsTableView, SIGNAL(signalEdit(QModelIndex)),
            this, SLOT(slotEditDataset(QModelIndex)));
    connect(m_ui->datasetsTableView, SIGNAL(signalDelete(QModelIndex)),
            this, SLOT(slotRemoveDataset(QModelIndex)));

    // reset controls
    clearControls();
}

DatasetPage::~DatasetPage()
{
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
    DatasetItemModel *model =
            qobject_cast<DatasetItemModel *>(datasetsProxyModel()->sourceModel());
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

    // check if the selection is valid
    if (!index.isValid() || currentDatasets.empty()) {
        return;
    }

    // enable only remove if more than one selected
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

    editDataset(currentDatasets.front());
}

void DatasetPage::slotEditDataset(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const auto currentDatasets = datasetsModel()->getDatasets(QItemSelection(index, index));

    // Can only open 1 valid dataset
    if (currentDatasets.size() != 1) {
        return;
    }

    editDataset(currentDatasets.front());
}

void DatasetPage::editDataset(const Dataset &dataset)
{
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
            if (dataset == *(m_open_dataset.data())
                    && (dataset.dataFile() != updated_dataset.dataFile()
                        || dataset.imageFile() != updated_dataset.imageFile()
                        || dataset.spotsFile() != updated_dataset.spotsFile())) {
                m_open_dataset = QSharedPointer<Dataset>(new Dataset(updated_dataset));
                emit signalDatasetUpdated(updated_dataset.name());
            }
            slotDatasetsUpdated();
        }
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

    openDataset(currentDatasets.front());
}

void DatasetPage::slotOpenDataset(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const auto currentDatasets = datasetsModel()->getDatasets(QItemSelection(index, index));

    // Can only open 1 valid dataset
    if (currentDatasets.size() != 1) {
        return;
    }

    openDataset(currentDatasets.front());
}

void DatasetPage::openDataset(const Dataset &dataset)
{
    // Set selected dataset
    m_open_dataset = QSharedPointer<Dataset>(new Dataset(dataset));

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

    removeDatasets(currentDatasets);
}

void DatasetPage::slotRemoveDataset(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const auto currentDatasets = datasetsModel()->getDatasets(QItemSelection(index, index));
    removeDatasets(currentDatasets);
}

void DatasetPage::removeDatasets(const QList<Dataset> &datasets)
{
    const int answer
            = QMessageBox::warning(this,
                                   tr("Remove Dataset"),
                                   tr("Are you really sure you want to remove the dataset/s?"),
                                   QMessageBox::Yes,
                                   QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    for (const auto &dataset: datasets) {
        Q_ASSERT(m_importedDatasets.removeOne(dataset));
        if (!m_open_dataset.isNull() && *(m_open_dataset.data()) == dataset) {
            emit signalDatasetRemoved(dataset.name());
            m_open_dataset = nullptr;
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
    }
}

QSharedPointer<Dataset> DatasetPage::getCurrentDataset() const
{
    return m_open_dataset;
}

void DatasetPage::slotDatasetsUpdated()
{
    // update model and clear controls
    datasetsModel()->loadData(m_importedDatasets);
    clearControls();
}

bool DatasetPage::nameExist(const QString &name)
{
    return std::find_if(m_importedDatasets.begin(), m_importedDatasets.end(),
                        [&name](const Dataset& dataset)
    {return dataset.name() == name;}) != m_importedDatasets.end();
}

#ifndef DATASETPAGE_H
#define DATASETPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <memory>
#include "data/DataProxy.h"

class QItemSelectionModel;
class QItemSelection;
class Error;
class DatasetItemModel;
class QSortFilterProxyModel;
class WaitingSpinnerWidget;
class DatasetImporter;
namespace Ui
{
class DataSets;
} // namespace Ui //

// This is the definition of the datasets view which contains a table of datasets to be selected.
// It gets updated everytime we show the view.
// It uses the dataProxy object to load the data.
// It has a toolbar with basic functionalities

// TODO add option to highlight the currently opened dataset
// TODO add right click support (copy, open, save, delete...)
// TODO add multiple dataset remove option
// TODO add ST icon to the widget
// TODO add posibility to edit and save objects in the table
// TODO add option to get more info (stats) for the dataset
class DatasetPage : public QWidget
{
    Q_OBJECT

public:
    DatasetPage(QPointer<DataProxy> dataProxy, QWidget* parent = 0);
    virtual ~DatasetPage();

    // clear the loaded content
    void clean();

public slots:

private slots:

    // Changes the selected dataset to index.
    void slotDatasetSelected(QModelIndex index);

    // Selects the indexed dataset and opens it.
    void slotSelectAndOpenDataset(QModelIndex index);

    // some slots for the actions of the toolbar
    void slotLoadDatasets();
    void slotOpenDataset();
    void slotRemoveDataset();
    void slotEditDataset();
    void slotImportDataset();

    // slots to process when the datasets have been downloaded/edit/removed
    // and also when the dataset's content is downloaded
    void slostDatasetsDownloaded(const DataProxy::DownloadStatus status);
    void slotDatasetModified(const DataProxy::DownloadStatus status);
    void slotDatasetContentDownloaded(const DataProxy::DownloadStatus status);
    void slotImageAlignmentDownloaded(const DataProxy::DownloadStatus status);
    void slotDatasetRemoved(const DataProxy::DownloadStatus status);

signals:

    void signalDatasetOpen(const QString datasetId);
    void signalDatasetRemoved(const QString datasetId);
    void signalDatasetUpdated(const QString datasetId);

protected:

    void showEvent(QShowEvent* event) override;

private:

    // get selected dataset from the table
    DataProxy::DatasetPtr getSelectedDataset();
    // clear focus and resets to default
    void clearControls();

    // to get the model from the table
    QSortFilterProxyModel* datasetsProxyModel();
    DatasetItemModel* datasetsModel();

    std::unique_ptr<Ui::DataSets> m_ui;
    // reference to dataProxy
    QPointer<DataProxy> m_dataProxy;
    // waiting spinner
    QPointer<WaitingSpinnerWidget> m_waiting_spinner;
    // List of imported datasets
    QHash<QString, QPointer<DatasetImporter> > m_importedDatasets;

    Q_DISABLE_COPY(DatasetPage)
};

#endif /* DATASETPAGE_H */

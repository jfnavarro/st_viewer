/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

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

namespace Ui
{
class DataSets;
} // namespace Ui //

// This is the definition of the datasets page which contains a table of datasets to be selected.
// It gets updated everytime we enter the page and by selecting a dataset.
// It uses the dataProxy object to load the data.
// It has a toolbar with basic functionalities

// TODO add option to highlight the currently opened dataset
// TODO add right click support (copy, open, save, delete...)
// TODO add multiple dataset remove option
// TODO add ST icon to the widget
// TODO add posibility to edit and save objects in the table
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

    // some slots for the actions buttons of the view
    // TODO add support to remove multiple datasets
    void slotLoadDatasets();
    void slotOpenDataset();
    void slotRemoveDataset();
    void slotEditDataset();

    // slot used to be notified when the datasets have been downloaded,
    // updated or removed from network
    // status contains the status of the operation (ok, abort, error)
    // type contain the type of download request
    void slotDownloadFinished(const DataProxy::DownloadStatus status,
                              const DataProxy::DownloadType type);

signals:

    void signalDatasetOpen(QString datasetId);

protected:

    void showEvent(QShowEvent* event) override;

private:
    // internal function to process when the datasets have been downloaded
    void datasetsDownloaded(const DataProxy::DownloadStatus status);

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

    Q_DISABLE_COPY(DatasetPage)
};

#endif /* DATASETPAGE_H */

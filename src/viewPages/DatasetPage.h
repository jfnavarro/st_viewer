#ifndef DATASETPAGE_H
#define DATASETPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <memory>
#include "data/Dataset.h"

class QItemSelectionModel;
class QItemSelection;
class DatasetItemModel;
class QSortFilterProxyModel;
class WaitingSpinnerWidget;
class DatasetImporter;

namespace Ui
{
class DataSets;
} // namespace Ui //

// This is the definition of the datasets view which contains
// a table with datasets (imported locally)
// It has a toolbar with basic functionalities (such as open, edit, export, import, etc..)

// TODO add option to highlight the currently opened dataset
// TODO add right click support (copy, open, save, delete...)
class DatasetPage : public QWidget
{
    Q_OBJECT

public:
    DatasetPage(QWidget *parent = 0);
    virtual ~DatasetPage();

    // clear the loaded content
    void clean();

    // the currently open dataset
    const Dataset &currentDataset() const;

public slots:

private slots:

    // changes the selected dataset to index.
    void slotDatasetSelected(QModelIndex index);

    // selects the indexed dataset and opens it.
    void slotSelectAndOpenDataset(QModelIndex index);

    // update the MVC table model
    void slotDatasetsUpdated();

    // some slots for the actions of the toolbar
    void slotOpenDataset();
    void slotRemoveDataset();
    void slotEditDataset();
    void slotImportDataset();

signals:

    // to notify about dataset/s action/s
    void signalDatasetOpen(const QString dataset);
    void signalDatasetRemoved(const QString dataset);
    void signalDatasetUpdated(const QString dataset);

protected:
    void showEvent(QShowEvent *event) override;

private:
    // clear focus and resets to default all buttons status
    void clearControls();
    // check if a dataset with the same name exists
    bool nameExist(const QString &name);

    // to get the data model from the table
    QSortFilterProxyModel *datasetsProxyModel();
    DatasetItemModel *datasetsModel();

    // reference to ui object
    QScopedPointer<Ui::DataSets> m_ui;
    // waiting (loading) spinner
    QScopedPointer<WaitingSpinnerWidget> m_waiting_spinner;
    // List of imported datasets (from files)
    QList<Dataset> m_importedDatasets;
    // Currently open dataset
    Dataset m_open_dataset;

    Q_DISABLE_COPY(DatasetPage)
};

#endif /* DATASETPAGE_H */

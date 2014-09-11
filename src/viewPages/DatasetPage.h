/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETPAGE_H
#define DATASETPAGE_H

#include "Page.h"

#include <memory>

#include <QModelIndex>

#include "data/DataProxy.h"

class QItemSelectionModel;
class QItemSelection;
class Error;
class DatasetItemModel;
class QSortFilterProxyModel;

namespace Ui {
class DataSets;
} // namespace Ui //

// this is the definition of the datasets page which contains a list of datasets to be selected
// it gets updated everytime we enter the page and by selecting a dataset
// we invoke the dataproxy to load the data
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.
class DatasetPage : public Page
{
    Q_OBJECT

public:

    DatasetPage(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~DatasetPage();

public slots:

    void onEnter() override;
    void onExit() override;

protected slots:

    void slotDatasetSelected(QModelIndex index);
    void slotRefreshDatasets();
    void slotLoadDatasets();
    void slotOpenDataset();
    void slotRemoveDataset();
    void slotEditDataset();

private:

    void clearControls();

    QSortFilterProxyModel *datasetsProxyModel();
    DatasetItemModel *datasetsModel();

    std::unique_ptr<Ui::DataSets> m_ui;
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(DatasetPage)
};

#endif  /* DATASETPAGE_H */


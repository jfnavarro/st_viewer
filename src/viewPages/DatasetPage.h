/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETPAGE_H
#define DATASETPAGE_H

#include "data/DataProxy.h"
#include "Page.h"

class QItemSelectionModel;
class QItemSelection;

namespace Ui
{
class DataSets;
} // namespace Ui //

// this is the definition of the datasets page which contains a list of datasets to be selected
// it gets updated everytime we enter the page and by selecting a dataset we invoke the dataproxy to load the data
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.

class Error;
class DatasetItemModel;
class QSortFilterProxyModel;

class DatasetPage : public Page
{
    Q_OBJECT

public:

    explicit DatasetPage(QWidget *parent = 0);
    virtual ~DatasetPage();

public slots:

    void onInit();
    void onEnter();
    void onExit();

protected slots:

    void datasetSelected(DataProxy::DatasetPtr);
    void refreshDatasets();
    void loadDatasets();

protected:

    void setWaiting(bool waiting = true);

private:
    QSortFilterProxyModel *datasetsProxyModel();
    DatasetItemModel *datasetsModel();
    Ui::DataSets *ui;

    Q_DISABLE_COPY(DatasetPage)
};

#endif  /* DATASETPAGE_H */


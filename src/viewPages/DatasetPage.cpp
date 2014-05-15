/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DatasetPage.h"

#include <QDebug>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include "data/DataProxy.h"
#include "data/DataStore.h"

#include "network/DownloadManager.h"

#include "error/Error.h"

#include "model/DatasetItemModel.h"

#include "utils/Utils.h"

#include "ui_datasets.h"

DatasetPage::DatasetPage(QWidget *parent) :
    Page(parent)
{
    onInit();
}

DatasetPage::~DatasetPage()
{
    delete ui;
}

QSortFilterProxyModel *DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel* datasetsProxyModel =
        qobject_cast<QSortFilterProxyModel*>(ui->datasets_tableview->model());
    Q_ASSERT(datasetsProxyModel);
    return datasetsProxyModel;
}

DatasetItemModel *DatasetPage::datasetsModel()
{
    DatasetItemModel *model =
        qobject_cast<DatasetItemModel*>(datasetsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void DatasetPage::onInit()
{
    // create UI
    ui = new Ui::DataSets;
    ui->setupUi(this);
    
    //connect signals
    connect(ui->datasetsFilterLineEdit, SIGNAL(textChanged(QString)), datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(datasetsModel(), SIGNAL(datasetSelected(DataProxy::DatasetPtr)),
            this, SLOT(datasetSelected(DataProxy::DatasetPtr)), Qt::UniqueConnection);
    connect(ui->backtodatasets, SIGNAL(clicked(bool)), this,
            SIGNAL(moveToPreviousPage()), Qt::UniqueConnection);
    connect(ui->refresh, SIGNAL(clicked(bool)), this, SLOT(refreshDatasets()), Qt::UniqueConnection);
}

void DatasetPage::onEnter()
{
    loadDatasets();
    //clear selection/focus
    ui->datasets_tableview->clearSelection();
    ui->datasets_tableview->clearFocus();
    ui->backtodatasets->clearFocus();
    ui->refresh->clearFocus();
}

void DatasetPage::onExit()
{
}

void DatasetPage::datasetSelected(DataProxy::DatasetPtr item)
{
    if (item.isNull() || item->id().isEmpty()) {       
        Error *error = new Error("Dataset Error", "Error loading the selected dataset.", this);
        emit signalError(error);
    } else {       
        DataProxy *dataProxy = DataProxy::getInstance();
        dataProxy->setSelectedDataset(item->id());
        emit moveToNextPage();
    }
}

void DatasetPage::loadDatasets()
{
    setWaiting(true);
    async::DataRequest request = DataProxy::getInstance()->loadDatasets();
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        Error *error = new Error("Data Error", "Error loading the datasets.", this);
        emit signalError(error);
    } else {
        // refresh datasets on the model
        datasetsModel()->loadDatasets();
    }
}

void DatasetPage::refreshDatasets()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    dataProxy->clean(); //clean the cache
    loadDatasets();
}

void DatasetPage::setWaiting(bool waiting)
{
    ui->datasets_tableview->setEnabled(!waiting);
    ui->backtodatasets->setEnabled(!waiting);
    ui->refresh->setEnabled(!waiting);
    Page::setWaiting(waiting);
}

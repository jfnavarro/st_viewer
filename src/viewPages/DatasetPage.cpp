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
    ui = nullptr;
}

QSortFilterProxyModel *DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel *datasetsProxyModel =
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
    connect(ui->filterLineEdit, SIGNAL(textChanged(QString)), datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(datasetsModel(), SIGNAL(datasetSelected(DataProxy::DatasetPtr)),
            this, SLOT(datasetSelected(DataProxy::DatasetPtr)));
    connect(ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(ui->next, SIGNAL(clicked(bool)), this, SIGNAL(moveToNextPage()));
    connect(ui->refresh, SIGNAL(clicked(bool)), this, SLOT(refreshDatasets()));
}

void DatasetPage::onEnter()
{
    loadDatasets();
    //clear selection/focus
    ui->datasets_tableview->clearSelection();
    ui->datasets_tableview->clearFocus();
    ui->back->clearFocus();
    ui->refresh->clearFocus();
    ui->next->clearFocus();
}

void DatasetPage::onExit()
{
    //clear selection/focus
    ui->datasets_tableview->clearSelection();
    ui->datasets_tableview->clearFocus();
    ui->back->clearFocus();
    ui->refresh->clearFocus();
    ui->next->clearFocus();
}

void DatasetPage::datasetSelected(DataProxy::DatasetPtr item)
{
    if (item.isNull() || item->id().isEmpty()) {
        showError("Data Error", "Error loading the selected dataset.");
    } else {       
        DataProxy *dataProxy = DataProxy::getInstance();
        dataProxy->setSelectedDataset(item->id());
        emit moveToNextPage();
    }
}

void DatasetPage::loadDatasets()
{
    //setWaiting(true);
    DataProxy *dataProxy = DataProxy::getInstance();
    async::DataRequest request = dataProxy->loadDatasets();
    //setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO show the error present in request.getErrors()
        showError("Data Error", "Error loading the datasets.");
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

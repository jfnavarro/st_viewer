/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DatasetPage.h"

#include <QDebug>
#include "utils/DebugHelper.h"
#include <QModelIndex>

#include "data/DataProxy.h"
#include "data/DataStore.h"
#include "error/Error.h"
#include "model/DatasetItemModel.h"
#include "utils/Utils.h"

#include "ui_datasets.h"

DatasetPage::DatasetPage(QWidget *parent) : Page(parent)
{
    onInit();
}

DatasetPage::~DatasetPage()
{
    delete ui;
}

void DatasetPage::onInit()
{
    DEBUG_FUNC_NAME
    
    // create UI
    ui = new Ui::DataSets;
    ui->setupUi(this);
    
    //connect signals
    DatasetItemModel *model = qobject_cast<DatasetItemModel*>(ui->datasets_tableview->model());
    connect(model, SIGNAL(datasetSelected(DataProxy::DatasetPtr)),
            this, SLOT(datasetSelected(DataProxy::DatasetPtr)), Qt::UniqueConnection);
    connect(ui->backtodatasets, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()), Qt::UniqueConnection);
    connect(ui->refresh, SIGNAL(clicked(bool)), this, SLOT(refreshDatasets()), Qt::UniqueConnection);
}

void DatasetPage::onEnter()
{
    DEBUG_FUNC_NAME
    // refresh datasets on the model and clear selection/focus
    DatasetItemModel *model = qobject_cast<DatasetItemModel*>(ui->datasets_tableview->model());
    model->loadDatasets();
    
    ui->datasets_tableview->clearSelection();
    ui->datasets_tableview->clearFocus();
    ui->abort->clearFocus();
    ui->backtodatasets->clearFocus();
    ui->refresh->clearFocus();
}

void DatasetPage::onExit()
{
    DEBUG_FUNC_NAME
}

void DatasetPage::slotDataError(Error *error)
{
    setWaiting(false);
    emit signalError(error);
}

void DatasetPage::datasetSelected(DataProxy::DatasetPtr item)
{
    if (item.isNull() || item->id().isEmpty()) {       
        emit signalError(new Error("Dataset Error", "Error loading the selected dataset."));        
    } else {       
        DataProxy *dataProxy = DataProxy::getInstance();
        dataProxy->setSelectedDataset(item->id());
        loadData();
    }
}

void DatasetPage::loadData()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());

    if (dataset.isNull()) {
        qDebug() << "[DatasetPage] Error: loadData() empty dataset!";
        Error *error = new Error("Dataset Error", "Error loading the selected dataset.");
        emit signalError(error);
        return;
    }

    async::DataRequest *request = dataProxy->loadDatasetContent(dataset);
    Q_ASSERT_X(request, "DatasetPage", "DataRequest object is null");

    if (request->return_code() == async::DataRequest::CodeError) {
        qDebug() << "[DatasetPage] Error: loading data";
        Error *error = new Error("Data loading Error", "Error loading the content of the selected dataset.");
        emit signalError(error);
    } else if (request->return_code() == async::DataRequest::CodePresent) {
        emit moveToNextPage();
    } else {
        connect(ui->abort, SIGNAL(clicked(bool)), request, SLOT(slotAbort()));
        connect(request, SIGNAL(signalFinished()), this, SLOT(dataLoaded()));
        connect(request, SIGNAL(signalError(Error*)), this, SLOT(slotDataError(Error*)));
        setWaiting(true);   
    }
}

void DatasetPage::dataLoaded()
{
    async::DataRequest *request = reinterpret_cast<async::DataRequest*>(sender());
    Q_ASSERT_X(request, "DatasetPage", "DataRequest object is null");
    setWaiting(false);
    if (request->return_code() == async::DataRequest::CodeSuccess) {
        emit moveToNextPage();
    }
}

void DatasetPage::refreshDatasets()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    dataProxy->clean(); //clean the cache

    async::DataRequest *request = dataProxy->loadDatasets();
    Q_ASSERT_X(request, "DatasetPage", "DataRequest object is null");

    DatasetItemModel *model = qobject_cast<DatasetItemModel*>(ui->datasets_tableview->model());

    if (request->return_code() == async::DataRequest::CodeError) {   
        qDebug() << "[DatasetPage] Error: loading data";
        Error *error = new Error("Data Error", "Error loading the datasets.");
        emit signalError(error);
        
    } else if (request->return_code() == async::DataRequest::CodePresent) {
        //NOTE this should not happen (dataproxy was cleaned)
        model->loadDatasets();
        
    } else {
        connect(request, SIGNAL(signalFinished()), model, SLOT(loadDatasets()));
        connect(request, SIGNAL(signalError(Error*)), this, SLOT(slotDataError(Error*)));
        //no need to wait here, if error it will load nothing, no abort signal neither
    }
}

void DatasetPage::setWaiting(bool waiting)
{
    ui->datasets_tableview->setEnabled(!waiting);
    ui->datasets_tableview->setWaiting(waiting);
    ui->backtodatasets->setEnabled(!waiting);
    ui->refresh->setEnabled(!waiting);
    ui->abort->setEnabled(waiting);
    ui->abort->setVisible(waiting);
    Page::setWaiting(waiting);
}

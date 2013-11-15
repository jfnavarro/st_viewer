/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DatasetPage.h"

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QModelIndex>

#include "controller/data/DataProxy.h"
#include "controller/data/DataStore.h"
#include "controller/error/Error.h"

#include "view/controllers/DatasetItemModel.h"
#include "view/delegates/DatasetsViewItemDelegate.h"

#include "utils/Utils.h"

#include "ui_datasets.h"

DatasetPage::DatasetPage(QWidget *parent) : Page(parent), m_datasetModel(0)
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

    ui = new Ui::DataSets;
    ui->setupUi(this);
    
    //crearte model for the table view
    m_datasetModel = new DatasetItemModel(this);
    //create selection model for table view
    ui->datasets_tableview->setModel(m_datasetModel);
    
    //NOTE item delegate is not finished yet
    //ui->datasets_tableview->setItemDelegate(new DatasetsViewItemDelegate(this));
    ui->datasets_tableview->setSortingEnabled(false);
    ui->datasets_tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->datasets_tableview->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->datasets_tableview->horizontalHeader()->setStretchLastSection(true);
    ui->datasets_tableview->setShowGrid(true);
    ui->datasets_tableview->setWordWrap(true);
    ui->datasets_tableview->setAlternatingRowColors(true);
    ui->datasets_tableview->resizeColumnsToContents();
    ui->datasets_tableview->resizeRowsToContents();
    ui->datasets_tableview->horizontalHeader()->setSortIndicatorShown(false);
    ui->datasets_tableview->verticalHeader()->setSortIndicatorShown(false);
    ui->datasets_tableview->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->datasets_tableview->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    ui->datasets_tableview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->datasets_tableview->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    ui->datasets_tableview->model()->submit(); //support for caching (speed up)
    ui->datasets_tableview->verticalHeader()->hide();
    ui->datasets_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->datasets_tableview->setSelectionMode(QAbstractItemView::SingleSelection);
    m_datasetModel->sort(0, Qt::AscendingOrder);
    
    //notify the model I have selected a dataset TODO should be a better way to do this
    connect(ui->datasets_tableview, SIGNAL(doubleClicked(QModelIndex)),
            m_datasetModel ,SLOT(datasetSelected(QModelIndex)));
    
    connect(m_datasetModel, SIGNAL(datasetSelected(DataProxy::DatasetPtr)),
            this, SLOT(datasetSelected(DataProxy::DatasetPtr)), Qt::UniqueConnection);
    
    connect(ui->backtodatasets, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()), Qt::UniqueConnection);
    connect(ui->refresh, SIGNAL(clicked(bool)), this, SLOT(refreshDatasets()), Qt::UniqueConnection);
}

void DatasetPage::onEnter()
{
    DEBUG_FUNC_NAME

    // refresh datasets on the model and clear selection/focus
    m_datasetModel->loadDatasets();
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
    if(item.isNull() || item->id().isEmpty())
    {
        emit signalError(new Error("Dataset Error", "Error loading the selected dataset."));
    }
    else
    {
        DataProxy *dataProxy = DataProxy::getInstance();
        dataProxy->setSelectedDataset(item->id());
        loadData();
    }
    
}

void DatasetPage::loadData()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());

    if (dataset.isNull())
    {
        qDebug() << "[DatasetPage] Error: loadData() empty dataset!";
        Error *error = new Error("Dataset Error", "Error loading the selected dataset.");
        emit signalError(error);
        return;
    }
    
    async::DataRequest *request = dataProxy->loadDatasetContent(dataset);
    Q_ASSERT_X(request, "DatasetPage", "DataRequest object is null");
    
    if(request->return_code() == async::DataRequest::CodeError)
    {
        qDebug() << "[DatasetPage] Error: loading data";
        Error *error = new Error("Data loading Error", "Error loading the content of the selected dataset.");
        emit signalError(error);
    }
    else if(request->return_code() == async::DataRequest::CodePresent)
    {
        emit moveToNextPage();
    }
    else
    {
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

    if(request->return_code() == async::DataRequest::CodeSuccess) //ignore when abort/timedout or error
    {
        emit moveToNextPage();
    }
}

void DatasetPage::refreshDatasets()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    dataProxy->clean(); //clean the cache
    
    async::DataRequest *request = dataProxy->loadDatasets();
    Q_ASSERT_X(request, "DatasetPage", "DataRequest object is null");
    
    if(request->return_code() == async::DataRequest::CodeError)
    {
        qDebug() << "[DatasetPage] Error: loading data";
        Error *error = new Error("Data Error", "Error loading the datasets.");
        emit signalError(error);
    }
    else if(request->return_code() == async::DataRequest::CodePresent)
    {
        //NOTE this should not happen (dataproxy was cleaned)
        m_datasetModel->loadDatasets();
    }
    else
    {
        connect(request, SIGNAL(signalFinished()), m_datasetModel, SLOT(loadDatasets()));
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

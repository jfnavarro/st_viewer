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

DatasetPage::DatasetPage(QPointer<DataProxy> dataProxy, QWidget *parent) :
    Page(parent),
    m_ui(nullptr),
    m_dataProxy(dataProxy)
{
    Q_ASSERT(!m_dataProxy.isNull());

    // create UI
    m_ui = new Ui::DataSets();
    m_ui->setupUi(this);

    //connect signals
    connect(m_ui->filterLineEdit, SIGNAL(textChanged(QString)), datasetsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(datasetsModel(), SIGNAL(datasetSelected(DataProxy::DatasetPtr)),
            this, SLOT(datasetSelected(DataProxy::DatasetPtr)));
    connect(m_ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(m_ui->next, SIGNAL(clicked(bool)), this, SIGNAL(moveToNextPage()));
    connect(m_ui->refresh, SIGNAL(clicked(bool)), this, SLOT(refreshDatasets()));
}

DatasetPage::~DatasetPage()
{
    if (m_ui != nullptr) {
        delete m_ui;
    }
    m_ui = nullptr;
}

QSortFilterProxyModel *DatasetPage::datasetsProxyModel()
{
    QSortFilterProxyModel *datasetsProxyModel =
        qobject_cast<QSortFilterProxyModel*>(m_ui->datasets_tableview->model());
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

void DatasetPage::onEnter()
{
    loadDatasets();
    //clear selection/focus
    m_ui->datasets_tableview->clearSelection();
    m_ui->datasets_tableview->clearFocus();
    m_ui->back->clearFocus();
    m_ui->refresh->clearFocus();
    m_ui->next->clearFocus();
}

void DatasetPage::onExit()
{

}

void DatasetPage::datasetSelected(DataProxy::DatasetPtr item)
{
    if (item.isNull() || item->id().isEmpty()) {
        showError("Data Error", "Error loading the selected dataset.");
    } else {
        m_dataProxy->setSelectedDataset(item->id());
        emit moveToNextPage();
    }
}

void DatasetPage::loadDatasets()
{
    setWaiting(true);
    async::DataRequest request = m_dataProxy->loadDatasets();
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO show the error present in request.getErrors()
        showError("Data Error", "Error loading the datasets.");
    } else {
        // refresh datasets on the model
        datasetsModel()->loadDatasets(m_dataProxy->getDatasetList());
    }
}

void DatasetPage::refreshDatasets()
{
    //clean the cache TODO this should not happen here, loadDatasets should clear the previous
    //datasets
    //m_dataProxy->clean();
    loadDatasets();
}

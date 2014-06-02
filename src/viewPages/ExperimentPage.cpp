/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ExperimentPage.h"
#include "ui_experiments.h"

#include <QDebug>
#include <QSortFilterProxyModel>

#include "model/ExperimentsItemModel.h"

#include "network/DownloadManager.h"

ExperimentPage::ExperimentPage(QWidget *parent)
    : Page(parent), ui(new Ui::Experiments)
{
    onInit();
}

ExperimentPage::~ExperimentPage()
{
    delete ui;
}

QSortFilterProxyModel *ExperimentPage::selectionsProxyModel()
{
    QSortFilterProxyModel* selectionsProxyModel =
        qobject_cast<QSortFilterProxyModel*>(ui->experiments_tableView->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}

ExperimentsItemModel *ExperimentPage::selectionsModel()
{
    ExperimentsItemModel *model =
        qobject_cast<ExperimentsItemModel*>(selectionsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void ExperimentPage::onInit()
{
    // create UI
    ui = new Ui::Experiments;
    ui->setupUi(this);

    //connect signals
    connect(ui->filterLineEdit, SIGNAL(textChanged(QString)), selectionsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(ui->back, SIGNAL(clicked(bool)), this,
            SIGNAL(moveToPreviousPage()), Qt::UniqueConnection);
}

void ExperimentPage::onEnter()
{
    loadSelections();
    //clear selection/focus
    ui->experiments_tableView->clearSelection();
    ui->experiments_tableView->clearFocus();
    ui->back->clearFocus();
}

void ExperimentPage::onExit()
{
}

void ExperimentPage::loadSelections()
{
    setWaiting(true);
    async::DataRequest request = DataProxy::getInstance()->loadGeneSelections();
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO use the text in reques.getErrors(
        showError("Data Error", "Error loading the selections.");
    } else {
        // refresh gene selections on the model
        selectionsModel()->loadSelectedGenes();
    }
}

void ExperimentPage::setWaiting(bool waiting)
{
    ui->experiments_tableView->setEnabled(!waiting);
    ui->back->setEnabled(!waiting);
    Page::setWaiting(waiting);
}

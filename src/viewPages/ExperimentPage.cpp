/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ExperimentPage.h"
#include "ui_experiments.h"

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>

#include "io/GeneExporter.h"

#include "model/ExperimentsItemModel.h"

#include "network/DownloadManager.h"

ExperimentPage::ExperimentPage(QWidget *parent)
    : Page(parent),
      ui(nullptr)
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
    connect(ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(ui->removeSelections, SIGNAL(clicked(bool)), this, SLOT(slotRemoveSelections()));
    connect(ui->exportSelections, SIGNAL(clicked(bool)), this, SLOT(slotExportSelections()));
    connect(ui->experiments_tableView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotSelectionSelected(QModelIndex)));
}

void ExperimentPage::onEnter()
{
    slotLoadSelections();
    //clear selection/focus
    ui->experiments_tableView->clearSelection();
    ui->experiments_tableView->clearFocus();
    ui->back->clearFocus();
    //remove and export not enable by default
    ui->removeSelections->setEnabled(false);
    ui->exportSelections->setEnabled(false);
}

void ExperimentPage::onExit()
{
}

void ExperimentPage::slotLoadSelections()
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

void ExperimentPage::slotSelectionSelected(QModelIndex index)
{
    //TODO check that the proper dataset object is selected and available
    ui->removeSelections->setEnabled(index.isValid());
    ui->exportSelections->setEnabled(index.isValid());
}

void ExperimentPage::slotRemoveSelections()
{
    const int answer = QMessageBox::warning(
                     this, tr("Exit application"),
                     tr("Are you really sure you want to remove the selections?"),
                     QMessageBox::No | QMessageBox::Escape,
                QMessageBox::Yes | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    const auto selected = ui->experiments_tableView->selectionModel()->selection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty()) {
        return;
    }

    setWaiting(true);
    for (auto selection : currentSelection) {
        Q_ASSERT(selection);
        DataProxy *dataProxy = DataProxy::getInstance();
        async::DataRequest request = dataProxy->removeGeneSelectionById(selection->id());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO show the error present in request.getErrors()
            showError("Data Error", QString("Error Remove the selection %1").arg(selection->name()));
        }
    }
    setWaiting(false);

    slotLoadSelections();
}

void ExperimentPage::slotExportSelections()
{
    const auto selected = ui->experiments_tableView->selectionModel()->selection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty()) {
        return;
    }

    QString filename =
            QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(),
                                         QString("%1").arg(tr("Text Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }
    // append default extension
    QRegExp regex("^.*\\.(txt)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".txt");
    }

    //create file
    QFile textFile(filename);

    //export selection
    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        GeneExporter exporter = GeneExporter(GeneExporter::SimpleFull,
                                             GeneExporter::TabDelimited);
        //TODO make sure gene exporter allows multiple writes
        for (auto geneSelection : currentSelection) {
            Q_ASSERT(geneSelection);
            exporter.exportItem(textFile, geneSelection->selectedItems());
        }
    }

    textFile.close();
}

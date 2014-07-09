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

ExperimentPage::ExperimentPage(QPointer<DataProxy> dataProxy, QWidget *parent)
    : Page(parent),
      m_ui(nullptr),
      m_dataProxy(dataProxy)
{
    Q_ASSERT(!m_dataProxy.isNull());

    // create UI
    m_ui = new Ui::Experiments();
    m_ui->setupUi(this);

    //connect signals
    connect(m_ui->filterLineEdit, SIGNAL(textChanged(QString)), selectionsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(m_ui->removeSelections, SIGNAL(clicked(bool)), this, SLOT(slotRemoveSelection()));
    connect(m_ui->exportSelections, SIGNAL(clicked(bool)), this, SLOT(slotExportSelection()));
    connect(m_ui->experiments_tableView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotSelectionSelected(QModelIndex)));
}

ExperimentPage::~ExperimentPage()
{
    if (m_ui != nullptr) {
        delete m_ui;
    }
    m_ui = nullptr;
}

QSortFilterProxyModel *ExperimentPage::selectionsProxyModel()
{
    QSortFilterProxyModel* selectionsProxyModel =
        qobject_cast<QSortFilterProxyModel*>(m_ui->experiments_tableView->model());
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

void ExperimentPage::onEnter()
{
    slotLoadSelections();
    //clear selection/focus
    m_ui->experiments_tableView->clearSelection();
    m_ui->experiments_tableView->clearFocus();
    m_ui->back->clearFocus();
    //remove and export not enable by default
    m_ui->removeSelections->setEnabled(false);
    m_ui->exportSelections->setEnabled(false);
    m_ui->ddaAnalysis->setEnabled(false);
}

void ExperimentPage::onExit()
{
}

void ExperimentPage::slotLoadSelections()
{
    setWaiting(true);
    async::DataRequest request = m_dataProxy->loadGeneSelections();
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO use the text in reques.getErrors(
        showError("Data Error", "Error loading the selections.");
    } else {
        // refresh gene selections on the model
        selectionsModel()->loadSelectedGenes(m_dataProxy->getGeneSelections());
    }
}

void ExperimentPage::slotSelectionSelected(QModelIndex index)
{
    //TODO check that the proper dataset object is selected and available
    m_ui->removeSelections->setEnabled(index.isValid());
    m_ui->exportSelections->setEnabled(index.isValid());
    m_ui->ddaAnalysis->setEnabled(index.isValid());
}

//TODO make it work for only unique selections
void ExperimentPage::slotRemoveSelection()
{
    const int answer = QMessageBox::warning(
                     this, tr("Exit application"),
                     tr("Are you really sure you want to remove the selections?"),
                     QMessageBox::No | QMessageBox::Escape,
                QMessageBox::Yes | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    const auto selected = m_ui->experiments_tableView->selectionModel()->selection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty()) {
        return;
    }

    //setWaiting(true);
    //TODO set the enabled field to false and send an update request
    //setWaiting(false);

    slotLoadSelections();
}

//TODO make it work for only unique selections
void ExperimentPage::slotExportSelection()
{
    const auto selected = m_ui->experiments_tableView->selectionModel()->selection();
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

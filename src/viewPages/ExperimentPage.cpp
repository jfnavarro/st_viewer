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

#include "dialogs/CreateSelectionDialog.h"

#include "analysis/ScatterPlot.h"

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
    connect(m_ui->ddaAnalysis, SIGNAL(clicked(bool)), this, SLOT(slotPerformDDA()));
    connect(m_ui->experiments_tableView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotSelectionSelected(QModelIndex)));
    connect(m_ui->editSelection, SIGNAL(clicked(bool)), this, SLOT(slotEditSelection()));
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
    //load selections
    slotLoadSelections();

    //clear selection/focus
    m_ui->experiments_tableView->clearSelection();
    m_ui->experiments_tableView->clearFocus();
    m_ui->removeSelections->clearFocus();
    m_ui->exportSelections->clearFocus();
    m_ui->ddaAnalysis->clearFocus();
    m_ui->editSelection->clearFocus();
    m_ui->back->clearFocus();

    //remove, edit, dda and export not enable by default
    m_ui->removeSelections->setEnabled(false);
    m_ui->exportSelections->setEnabled(false);
    m_ui->ddaAnalysis->setEnabled(false);
    m_ui->editSelection->setEnabled(false);
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
        showError(tr("Data Error"), tr("Error loading the selections"));
    } else {
        // refresh gene selections on the model
        selectionsModel()->loadSelectedGenes(m_dataProxy->getGeneSelections());
    }
}

void ExperimentPage::slotSelectionSelected(QModelIndex index)
{
    //TODO check that the proper dataset object is selected and available
    //TODO two items select = activate DDA, one item activates the others

    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    const bool enableDDA = index.isValid() && currentSelection.size() == 2;
    const bool enableRest = index.isValid() && currentSelection.size() == 1;

    m_ui->removeSelections->setEnabled(enableRest);
    m_ui->exportSelections->setEnabled(enableRest);
    m_ui->ddaAnalysis->setEnabled(enableDDA);
    m_ui->editSelection->setEnabled(enableRest);
}

void ExperimentPage::slotRemoveSelection()
{
    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    const int answer = QMessageBox::warning(
                this, tr("Remove Selection"),
                tr("Are you really sure you want to remove the selection?"),
                QMessageBox::No | QMessageBox::Escape,
                QMessageBox::Yes | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    //currentSelection should only have one element
    auto selectionItem = currentSelection.first();
    Q_ASSERT(!selectionItem.isNull());

    //sets enabled to false
    selectionItem->enabled(false);

    //update the selection object
    setWaiting(true, "Removing selection....");
    async::DataRequest request = m_dataProxy->updateGeneSelection(selectionItem);
    setWaiting(false);

    if (request.return_code() == async::DataRequest::CodeError
            || request.return_code() == async::DataRequest::CodeAbort) {
        //TODO get error from request
        showError(tr("Remove Gene Selection"), tr("Error removing the gene selection"));
        return;
    }

    showInfo(tr("Remove Gene Selection"), tr("Gene selection removed successfully"));

    //refresh selection list
    slotLoadSelections();
}

void ExperimentPage::slotExportSelection()
{
    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty() || currentSelection.size() > 1) {
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

    //currentSelection should only have one element
    auto selectionItem = currentSelection.first();
    Q_ASSERT(!selectionItem.isNull());

    //export selection
    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        GeneExporter exporter = GeneExporter(GeneExporter::SimpleFull,
                                             GeneExporter::TabDelimited);
        exporter.exportItem(textFile, selectionItem->selectedItems());
        showInfo(tr("Export Gene Selection"), tr("Gene selection was exported successfully"));
    }

    textFile.close();
}

void ExperimentPage::slotEditSelection()
{
    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    //currentSelection should only have one element
    auto selectionItem = currentSelection.first();
    Q_ASSERT(!selectionItem.isNull());

    QScopedPointer<CreateSelectionDialog> createSelection(new CreateSelectionDialog(this,
                                                                                    Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    createSelection->setName(selectionItem->name());
    createSelection->setComment(selectionItem->comment());

    if (createSelection->exec() == CreateSelectionDialog::Accepted) {
        if (createSelection->getName() != selectionItem->name()
                && createSelection->getComment() != selectionItem->comment()) {

            //TODO check that name is not empty

            selectionItem->name(createSelection->getName());
            selectionItem->comment(createSelection->getComment());

            //update the dataset
            setWaiting(true, "Updating selection...");
            async::DataRequest request = m_dataProxy->updateGeneSelection(selectionItem);
            setWaiting(false);

            if (request.return_code() == async::DataRequest::CodeError
                    || request.return_code() == async::DataRequest::CodeAbort) {
                //TODO get error from request
                showError(tr("Update Gene Selection"), tr("Error updating the gene selection"));
                return;
            }

            showInfo(tr("Update Gene Selection"), tr("Gene Selection updated successfully"));

            //refresh selection list
            slotLoadSelections();
        }
    }
}

void ExperimentPage::slotPerformDDA()
{
    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty() || currentSelection.size() != 2) {
        return;
    }

    // get the two selection objects
    auto selectionObject1 = currentSelection.at(0);
    Q_ASSERT(!selectionObject1.isNull());
    auto selectionObject2 = currentSelection.at(1);
    Q_ASSERT(!selectionObject2.isNull());

    // get the selection items lists
    auto selectionItem1 = selectionObject1->selectedItems();
    auto selectionItem2 = selectionObject2->selectedItems();

    // sort the selection lists by name
    qSort(selectionItem1);
    qSort(selectionItem2);

    // get the size of the biggest list
    const int biggestSize = qMax(selectionItem1.size(), selectionItem2.size());

    // compute correlation (TODO)

    // get the plotting data
    //TODO should take into account different genes
    QVector<qreal> x(biggestSize);
    QVector<qreal> y(biggestSize);
    for (int i = 0; i < biggestSize; ++i) {
        x[i] = selectionItem1.size() > i ? selectionItem1.at(i).normalizedReads : 0.0;
        y[i] = selectionItem2.size() > i ? selectionItem2.at(i).normalizedReads : 0.0;
    }

    //TODO send correlation to show in the header
    ScatterPlot *scatterPlot = new ScatterPlot();
    scatterPlot->plot(x, y, selectionObject1->name() + " - (tpm+1)",
                      selectionObject2->name() + " - (tpm+1)");

    //TODO memmory leak solve this
    //scatterPlot->deleteLater();
}

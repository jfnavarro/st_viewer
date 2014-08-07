/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ExperimentPage.h"

#include "ui_experiments.h"

#include <cmath>

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>

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
                && createSelection->getComment() != selectionItem->comment()
                && !createSelection->getName().isNull() && !createSelection->getName().isEmpty()) {

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

//TODO add waiting cursor
//TODO move computations to AnalysisDDA class
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
    auto selectionItems1 = selectionObject1->selectedItems();
    auto selectionItems2 = selectionObject2->selectedItems();

    // get the size of the biggest list
    const int selection1Size = selectionItems1.size();
    const int selection2Size = selectionItems2.size();
    const int biggestSize = qMax(selection1Size, selection2Size);

    //take into account different genes that some genes might be present in only one selection
    QHash<QString, QPair<qreal,qreal> > genesToNormalizedReads;
    for (int i = 0; i < biggestSize; ++i) {
        if (selection1Size > i) {
            const auto& selection1 = selectionItems1.at(i);
            if (!genesToNormalizedReads.contains(selection1.name)) {
                genesToNormalizedReads[selection1.name] = QPair<qreal,qreal>(-1.0, -1.0);
            }
            genesToNormalizedReads[selection1.name].first = selection1.normalizedReads;
        }
        if (selection2Size > i) {
            const auto& selection2 = selectionItems2.at(i);
            if (!genesToNormalizedReads.contains(selection2.name)) {
                genesToNormalizedReads[selection2.name] = QPair<qreal,qreal>(-1.0, -1.0);
            }
            genesToNormalizedReads[selection2.name].second = selection2.normalizedReads;
        }
    }

    //TODO send values to ScatterPlot to be whoen in a widget
    //TODO rename ScatterPlot
    ScatterPlot *scatterPlot = new ScatterPlot(biggestSize);

    QVector<qreal> x;
    QVector<qreal> y;
    QVector<qreal> logX;
    QVector<qreal> logY;
    qreal sumSelection1 = 0.0;
    qreal sumSelection2 = 0.0;
    qreal sumSquaredSelection1 = 0.0;
    qreal sumSquaredSelection2 = 0.0;
    qreal sumSquaredSelection12 = 0.0;
    int countOnly1 = 0;
    int countOnly2 = 0;
    int countBoth = 0;
    int index = 0;
    QHash<QString, QPair<qreal,qreal> >::const_iterator it = genesToNormalizedReads.begin();
    QHash<QString, QPair<qreal,qreal> >::const_iterator end = genesToNormalizedReads.end();
    for ( ; it != end; ++it) {

        const bool geneNotInSelection1 = it.value().first == -1;
        const bool geneNotInSelection2 = it.value().second == -1;

        scatterPlot->getTable()->setItem(index, 0, new QTableWidgetItem(it.key()));
        if (!geneNotInSelection1) {
            scatterPlot->getTable()->setItem(index, 1,  new QTableWidgetItem(QString::number(it.value().first)));
        } else {
            scatterPlot->getTable()->setItem(index, 1,  new QTableWidgetItem("None"));
        }
        if (!geneNotInSelection2) {
            scatterPlot->getTable()->setItem(index, 2, new QTableWidgetItem(QString::number(it.value().second)));
        } else {
            scatterPlot->getTable()->setItem(index, 2,  new QTableWidgetItem("None"));
        }
        index++;

        //TODO validate 1.0 is a good value to assign when no gene present
        const qreal valueSelection1 = !geneNotInSelection1 ? it.value().first : 1.0;
        const qreal valueSelection2 = !geneNotInSelection2 ? it.value().second : 1.0;
        if (geneNotInSelection1) {
            countOnly2++;
        } else if (geneNotInSelection2) {
            countOnly1++;
        } else {
            countBoth++;
        }

        x.push_back(valueSelection1);
        y.push_back(valueSelection2);

        logX.push_back(std::log10(valueSelection1));
        logY.push_back(std::log10(valueSelection2));

        sumSelection1 += valueSelection1;
        sumSelection2 += valueSelection2;
        sumSquaredSelection1 += valueSelection1 * valueSelection1;
        sumSquaredSelection2 += valueSelection2 * valueSelection2;
        sumSquaredSelection12 += valueSelection1 * valueSelection2;
        //TODO consider use log values for correlation
    }

    scatterPlot->getTable()->setSortingEnabled(true);

    const qreal meanSelection1 = sumSelection1 / biggestSize;
    const qreal meanSelection2 = sumSelection2 / biggestSize;
    const qreal stdDevSelection1 =
            std::sqrt(sumSquaredSelection1 / biggestSize - meanSelection1 * meanSelection1);
    const qreal stdDevSelection2 =
            std::sqrt(sumSquaredSelection2 / biggestSize - meanSelection2 * meanSelection2);
    const qreal correlation = (biggestSize * sumSquaredSelection12 - sumSelection1 * sumSelection2)
            / std::sqrt((biggestSize * sumSquaredSelection2 - sumSelection1 * sumSelection1)
                        * (biggestSize * sumSquaredSelection2 - sumSelection2 * sumSelection2));

    //TODO use string formatter for this
    //TODO the idea is to do the computation in AnalysisDDA class and pass that object
    //to the visualization widget
    scatterPlot->setNumberGenes(QString::number(selection1Size) , QString::number(selection2Size));
    scatterPlot->setMean(QString::number(meanSelection1), QString::number(meanSelection2));
    scatterPlot->setStdDev(QString::number(stdDevSelection1), QString::number(stdDevSelection2));
    scatterPlot->setCorrelation(QString::number(correlation));
    scatterPlot->setOverlapping(QString::number(countBoth), QString::number(countOnly1), QString::number(countOnly2));
    scatterPlot->setSelection(selectionObject1->name(), selectionObject2->name());
    scatterPlot->plot(x, y, selectionObject1->name() + " - (tpm+1)",
                      selectionObject2->name() + " - (tpm+1)");

    //TODO memmory leak solve this
    //scatterPlot->deleteLater();
}

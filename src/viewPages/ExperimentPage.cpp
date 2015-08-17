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
#include <QTableWidgetItem>
#include <QScrollArea>

#include "io/GeneExporter.h"
#include "model/ExperimentsItemModel.h"
#include "dialogs/CreateSelectionDialog.h"
#include "analysis/AnalysisDEA.h"

using namespace Globals;

ExperimentPage::ExperimentPage(QPointer<DataProxy> dataProxy, QWidget *parent)
    : Page(dataProxy, parent),
      m_ui(new Ui::Experiments())
{
    m_ui->setupUi(this);
    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    setWindowFlags(Qt::FramelessWindowHint);
    m_ui->experimentsPageWidget->setStyleSheet("QWidget#experimentsPageWidget " + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // connect signals
    connect(m_ui->filterLineEdit, SIGNAL(textChanged(QString)), selectionsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->back, SIGNAL(clicked(bool)), this, SIGNAL(moveToPreviousPage()));
    connect(m_ui->removeSelections, SIGNAL(clicked(bool)), this, SLOT(slotRemoveSelection()));
    connect(m_ui->exportSelections, SIGNAL(clicked(bool)), this, SLOT(slotExportSelection()));
    connect(m_ui->ddaAnalysis, SIGNAL(clicked(bool)), this, SLOT(slotPerformDEA()));
    connect(m_ui->experiments_tableView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotSelectionSelected(QModelIndex)));
    connect(m_ui->editSelection, SIGNAL(clicked(bool)), this, SLOT(slotEditSelection()));
    connect(m_ui->showTissue, SIGNAL(clicked(bool)), this, SLOT(slotShowTissue()));

    // connect data proxy signal
    connect(m_dataProxy.data(),
            SIGNAL(signalDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)),
            this, SLOT(slotDownloadFinished(DataProxy::DownloadStatus, DataProxy::DownloadType)));

    clearControls();
}

ExperimentPage::~ExperimentPage()
{

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
    // load selections
    loadSelections();
}

void ExperimentPage::onExit()
{
    clearControls();
}

void ExperimentPage::clearControls()
{
    // clear selection/focus
    m_ui->experiments_tableView->clearSelection();
    m_ui->experiments_tableView->clearFocus();
    m_ui->removeSelections->clearFocus();
    m_ui->exportSelections->clearFocus();
    m_ui->ddaAnalysis->clearFocus();
    m_ui->editSelection->clearFocus();
    m_ui->back->clearFocus();

    // remove, edit, dda and export not enable by default
    m_ui->removeSelections->setEnabled(false);
    m_ui->exportSelections->setEnabled(false);
    m_ui->ddaAnalysis->setEnabled(false);
    m_ui->editSelection->setEnabled(false);
    m_ui->showTissue->setEnabled(false);
}

void ExperimentPage::loadSelections()
{
    if (!m_dataProxy->userLogIn()) {
        return;
    }

    // load selections and enable the blocking loading bar
    setWaiting(true);
    m_dataProxy->loadGeneSelections();
    m_dataProxy->activateCurrentDownloads();
}

void ExperimentPage::slotDownloadFinished(const DataProxy::DownloadStatus status,
                                          const DataProxy::DownloadType type)
{
    if (type == DataProxy::GenesSelectionsDownloaded) {
        // disable blocking loading bar
        setWaiting(false);
        if (status == DataProxy::Success) {
            // refresh gene selections on the model and controls
            selectionsModel()->loadSelectedGenes(m_dataProxy->getGenesSelectionsList());
            clearControls();
        }
    } else if (type == DataProxy::GenesSelectionsModified && status == DataProxy::Success) {
        // re-upload genes selections after an update
        loadSelections();
    }
}

void ExperimentPage::slotSelectionSelected(QModelIndex index)
{
    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    const bool enableDDA = index.isValid() && currentSelection.size() == 2;
    const bool enableRest = index.isValid() && currentSelection.size() == 1;
    // configure UI controls if we select 1 or 2 selections
    m_ui->removeSelections->setEnabled(enableRest);
    m_ui->exportSelections->setEnabled(enableRest);
    m_ui->ddaAnalysis->setEnabled(enableDDA);
    m_ui->editSelection->setEnabled(enableRest);
    m_ui->showTissue->setEnabled(enableRest);
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
                QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    // currentSelection should only have one element
    const auto selectionItem = currentSelection.first();
    Q_ASSERT(!selectionItem.isNull());

    // remove the selection object
    m_dataProxy->removeSelection(selectionItem->id());
    m_dataProxy->activateCurrentDownloads();
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

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        showError(tr("Export Genes Selection"), tr("The directory is not writable"));
        return;
    }

    // create file
    QFile textFile(filename);

    // currentSelection should only have one element
    auto selectionItem = currentSelection.first();
    Q_ASSERT(!selectionItem.isNull());

    // export selection
    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        GeneExporter exporter = GeneExporter(GeneExporter::SimpleFull,
                                             GeneExporter::TabDelimited);
        exporter.exportItem(textFile, selectionItem->selectedItems());
        showInfo(tr("Export Genes Selection"), tr("Genes selection was exported successfully"));
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

    // currentSelection should only have one element
    Q_ASSERT(!currentSelection.first().isNull());
    GeneSelection selectionItem(*currentSelection.first());

    QScopedPointer<CreateSelectionDialog>
            createSelection(new CreateSelectionDialog(this,
                                                      Qt::CustomizeWindowHint
                                                      | Qt::WindowTitleHint));
    createSelection->setWindowIcon(QIcon());
    createSelection->setName(selectionItem.name());
    createSelection->setComment(selectionItem.comment());

    if (createSelection->exec() == CreateSelectionDialog::Accepted
            && (createSelection->getName() != selectionItem.name()
                || createSelection->getComment() != selectionItem.comment())
            && !createSelection->getName().isNull() && !createSelection->getName().isEmpty()) {

        selectionItem.name(createSelection->getName());
        selectionItem.comment(createSelection->getComment());

        // update the dataset
        m_dataProxy->updateGeneSelection(selectionItem);
        m_dataProxy->activateCurrentDownloads();
    }
}

void ExperimentPage::slotPerformDEA()
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

    QScopedPointer<AnalysisDEA> analysisDEA(new AnalysisDEA(*selectionObject1, *selectionObject2));
    analysisDEA->exec();
}

void ExperimentPage::slotShowTissue()
{
    const auto selected = m_ui->experiments_tableView->experimentTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    auto selectionObject = currentSelection.at(0);
    QByteArray tissue_snapshot = selectionObject->tissueSnapShot();
    if (tissue_snapshot.isNull() || tissue_snapshot.isEmpty()){
        return;
    }

    QByteArray image_ba = QByteArray::fromBase64(tissue_snapshot);
    QImage image;
    image.loadFromData(image_ba);

    if (image.isNull()) {
        return;
    }

    QWidget *image_widget = new QWidget();
    image_widget->setAttribute(Qt::WA_DeleteOnClose);
    image_widget->setMinimumSize(600, 600);
    QVBoxLayout *layout1 = new QVBoxLayout(image_widget);
    QScrollArea *image_scroll = new QScrollArea();
    layout1->addWidget(image_scroll);
    QVBoxLayout *layout = new QVBoxLayout(image_scroll);
    QLabel *image_label = new QLabel();
    image_label->setPixmap(QPixmap::fromImage(image));
    image_label->setScaledContents(true);
    layout->addWidget(image_label);
    image_widget->show();
}

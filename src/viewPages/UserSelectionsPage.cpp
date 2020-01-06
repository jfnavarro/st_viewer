#include "UserSelectionsPage.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QInputDialog>

#include "viewPages/SelectionGenesWidget.h"
#include "viewPages/SelectionSpotsWidget.h"
#include "model/UserSelectionsItemModel.h"
#include "dialogs/EditSelectionDialog.h"
#include "analysis/AnalysisDEA.h"
#include "analysis/AnalysisCorrelation.h"
#include "analysis/AnalysisQC.h"
#include "SettingsStyle.h"

#include "ui_selectionsPage.h"

using namespace Style;

UserSelectionsPage::UserSelectionsPage(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::UserSelections())
{
    m_ui->setupUi(this);

    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    m_ui->userSelectionsPageWidget->setStyleSheet("QWidget#userSelectionsPageWidget "
                                                  + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // connect signals
    connect(m_ui->filterLineEdit, &QLineEdit::textChanged,
            selectionsProxyModel(), &QSortFilterProxyModel::setFilterFixedString);
    connect(m_ui->removeSelection, SIGNAL(clicked(bool)),
            this, SLOT(slotRemoveSelection()));
    connect(m_ui->exportSelection, SIGNAL(clicked(bool)),
            this, SLOT(slotExportSelection()));
    connect(m_ui->importSelection, &QPushButton::clicked,
            this, &UserSelectionsPage::slotImportSelection);
    connect(m_ui->editSelection, SIGNAL(clicked(bool)),
            this, SLOT(slotEditSelection()));
    connect(m_ui->ddaAnalysis, &QPushButton::clicked,
            this, &UserSelectionsPage::slotPerformDEA);
    connect(m_ui->correlationAnalysis, &QPushButton::clicked,
            this, &UserSelectionsPage::slotPerformCorrelation);
    connect(m_ui->selections_tableView,
            &UserSelectionTableView::clicked,
            this, &UserSelectionsPage::slotSelectionSelected);
    connect(m_ui->showGenes, &QPushButton::clicked, this, &UserSelectionsPage::slotShowGenes);
    connect(m_ui->showSpots, &QPushButton::clicked, this, &UserSelectionsPage::slotShowSpots);
    connect(m_ui->qcAnalysis, &QPushButton::clicked, this, &UserSelectionsPage::slotQC);
    connect(m_ui->merge, &QPushButton::clicked, this, &UserSelectionsPage::slotMerge);

    connect(m_ui->selections_tableView, SIGNAL(signalSelectionExport(QModelIndex)),
            this, SLOT(slotExportSelection(QModelIndex)));
    connect(m_ui->selections_tableView, SIGNAL(signalSelectionEdit(QModelIndex)),
            this, SLOT(slotEditSelection(QModelIndex)));
    connect(m_ui->selections_tableView, SIGNAL(signalSelectionDelete(QModelIndex)),
            this, SLOT(slotRemoveSelection(QModelIndex)));

    clearControls();
}

UserSelectionsPage::~UserSelectionsPage()
{
}

void UserSelectionsPage::clean()
{
    selectionsModel()->clear();
}

QSortFilterProxyModel *UserSelectionsPage::selectionsProxyModel()
{
    QSortFilterProxyModel *selectionsProxyModel
            = qobject_cast<QSortFilterProxyModel *>(m_ui->selections_tableView->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}

UserSelectionsItemModel *UserSelectionsPage::selectionsModel()
{
    UserSelectionsItemModel *model
            = qobject_cast<UserSelectionsItemModel *>(selectionsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void UserSelectionsPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    clearControls();
}

void UserSelectionsPage::clearControls()
{
    // clear selection/focus
    m_ui->selections_tableView->clearSelection();
    m_ui->selections_tableView->clearFocus();

    // only import enabled by default
    m_ui->removeSelection->setEnabled(false);
    m_ui->qcAnalysis->setEnabled(false);
    m_ui->exportSelection->setEnabled(false);
    m_ui->ddaAnalysis->setEnabled(false);
    m_ui->editSelection->setEnabled(false);
    m_ui->showGenes->setEnabled(false);
    m_ui->showSpots->setEnabled(false);
    m_ui->importSelection->setEnabled(true);
    m_ui->correlationAnalysis->setEnabled(false);
    m_ui->merge->setEnabled(false);
}


void UserSelectionsPage::slotSelectionSelected(QModelIndex index)
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (!index.isValid() || currentSelection.empty()) {
        return;
    }
    const bool enableMultiple = currentSelection.size() > 1;
    const bool enableDouble = currentSelection.size() == 2;
    const bool enableSingle = currentSelection.size() == 1;
    // configure UI controls if we select 1 or more selections
    m_ui->removeSelection->setEnabled(enableMultiple || enableSingle);
    m_ui->exportSelection->setEnabled(enableSingle);
    m_ui->importSelection->setEnabled(enableSingle);
    m_ui->ddaAnalysis->setEnabled(enableDouble);
    m_ui->qcAnalysis->setEnabled(enableSingle);
    m_ui->editSelection->setEnabled(enableSingle);
    m_ui->showGenes->setEnabled(enableSingle);
    m_ui->showSpots->setEnabled(enableSingle);
    m_ui->correlationAnalysis->setEnabled(enableDouble);
    m_ui->merge->setEnabled(enableMultiple);
}

void UserSelectionsPage::addSelection(const UserSelection& selection)
{
    if (selection.totalGenes() > 0 && selection.totalSpots() > 0) {
        m_selections.append(selection);
        selectionsUpdated();
    }
}

void UserSelectionsPage::selectionsUpdated()
{
    clearControls();
    selectionsModel()->loadUserSelections(m_selections);
    m_ui->selections_tableView->update();
}

void UserSelectionsPage::slotRemoveSelection()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelections = selectionsModel()->getSelections(selected);
    if (currentSelections.empty()) {
        return;
    }
    removeSelections(currentSelections);
}

void UserSelectionsPage::slotRemoveSelection(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    const auto currentSelections = selectionsModel()->getSelections(QItemSelection(index,index));
    if (currentSelections.empty()) {
        return;
    }
    removeSelections(currentSelections);
}

void UserSelectionsPage::removeSelections(const QList<UserSelection> &selections)
{
    const int answer
            = QMessageBox::warning(this,
                                   tr("Remove Selection"),
                                   tr("Are you really sure you want to remove the selection/s?"),
                                   QMessageBox::Yes,
                                   QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    // Remove the selections
    for (auto selection : selections) {
        m_selections.removeOne(selection);
    }

    // update the model
    selectionsUpdated();
}

void UserSelectionsPage::slotExportSelection()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    // currentSelection should only have one element
    exportSelection(currentSelection.front());
}

void UserSelectionsPage::slotExportSelection(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    const auto currentSelections = selectionsModel()->getSelections(QItemSelection(index,index));
    if (currentSelections.empty() || currentSelections.size() > 1) {
        return;
    }
    // currentSelection should only have one element
    exportSelection(currentSelections.front());
}

void UserSelectionsPage::exportSelection(const UserSelection &selection)
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Export Selection"),
                                                    QDir::homePath(),
                                                    QString("%1").arg(tr("Text Files (*.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        QMessageBox::critical(this, tr("Export Selection"), tr("The directory is not writable"));
        return;
    }

    // export selection
    try {
        STData::save(filename, selection.data());
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Export Selection"), tr("Error exporting the selection"));
        qDebug() << "There was an error saving the matrix in the selection page " << e.what();
    }
}

void UserSelectionsPage::slotEditSelection()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    // currentSelection should only have one element
    editSelection(currentSelection.front());
}

void UserSelectionsPage::slotEditSelection(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    const auto currentSelections = selectionsModel()->getSelections(QItemSelection(index,index));
    if (currentSelections.empty() || currentSelections.size() > 1) {
        return;
    }
    // currentSelection should only have one element
    editSelection(currentSelections.front());
}

void UserSelectionsPage::editSelection(const UserSelection &selection)
{
    // creates a selection dialog with the current fields
    QScopedPointer<EditSelectionDialog> editSelection(
                new EditSelectionDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editSelection->setWindowIcon(QIcon());
    editSelection->setName(selection.name());
    editSelection->setComment(selection.comment());
    if (editSelection->exec() == EditSelectionDialog::Accepted
            && !editSelection->getName().isNull() && !editSelection->getName().isEmpty()
            && (editSelection->getName() != selection.name()
                || editSelection->getComment() != selection.comment())
            && !nameExist(editSelection->getName())) {

        const int index = m_selections.indexOf(selection);
        Q_ASSERT(index != -1);

        // update object in the container
        m_selections[index].name(editSelection->getName());
        m_selections[index].comment(editSelection->getComment());

        // update model
        selectionsUpdated();
    }
}

void UserSelectionsPage::slotImportSelection()
{
    QFileDialog dialog(this, tr("Import selection (can select multiple)"));
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(QString("%1").arg(tr("TSV Files (*.tsv)")));
    QStringList fileNames;
    if (dialog.exec()) {
        // get all the selected files and iterate
        fileNames = dialog.selectedFiles();
        for (auto filename : fileNames) {
            // early out
            if (filename.isEmpty()) {
                continue;
            }

            QFileInfo info(filename);
            // We use the file name as selection name and check that is not present
            const QString name = info.baseName();
            if (nameExist(name)) {
                QMessageBox::critical(this, tr("Import selection"),
                                      tr("There exists a selection with the same name ") + name);
                continue;
            }

            if (info.isDir() || !info.isFile() || !info.isReadable()) {
                QMessageBox::critical(this, tr("Import selection"),
                                      tr("File is incorrect or not readable ") + filename);
            } else {
                try {
                    auto data = STData::read(filename);
                    UserSelection new_selection;
                    new_selection.data(data);
                    new_selection.name(name);
                    addSelection(new_selection);
                } catch (const std::exception &e) {
                    QMessageBox::critical(this, tr("Import selection"),
                                          tr("Error parsing file ") + filename);
                    qDebug() << "Error parsing ST data file (Selection) " << e.what();
                }
            }
        }
    }
}

void UserSelectionsPage::slotPerformDEA()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() != 2) {
        return;
    }

    // get the two selection objects
    const auto selectionObject1 = currentSelection.at(0);
    const auto selectionObject2 = currentSelection.at(1);


    // launch the DEA widget
    AnalysisDEA *deaWidget(
                new AnalysisDEA(selectionObject1.data(),
                                selectionObject2.data(),
                                selectionObject1.name(),
                                selectionObject2.name(),
                                this,
                                Qt::Window));
    deaWidget->show();
}

void UserSelectionsPage::slotPerformCorrelation()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() != 2) {
        return;
    }

    // get the two selection objects
    const auto selectionObject1 = currentSelection.at(0);
    const auto selectionObject2 = currentSelection.at(1);

    // launch the correlation widget
    AnalysisCorrelation *correlationWidget(
                new AnalysisCorrelation(selectionObject1.data(),
                                        selectionObject2.data(),
                                        selectionObject1.name(),
                                        selectionObject2.name(),
                                        this,
                                        Qt::Window));
    correlationWidget->show();
}

void UserSelectionsPage::slotShowGenes()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    const auto selectionObject = currentSelection.front();
    SelectionGenesWidget *genesWidget(
                new SelectionGenesWidget(selectionObject.data(), this, Qt::Window));
    genesWidget->show();
}

void UserSelectionsPage::slotShowSpots()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    const auto selectionObject = currentSelection.front();
    SelectionSpotsWidget *spotsWidget(
                new SelectionSpotsWidget(selectionObject.data(), this, Qt::Window));
    spotsWidget->show();
}

void UserSelectionsPage::slotQC()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    const auto selectionObject = currentSelection.front();
    AnalysisQC *qc = new AnalysisQC(selectionObject.data(), this, Qt::Window);
    qc->show();
}

void UserSelectionsPage::slotMerge()
{
    // get the selected objects
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.size() <= 1) {
        return;
    }

    QList<STData::STDataFrame> datasets;
    for (const auto selection : currentSelection) {
        datasets.append(selection.data());
    }

    bool ok = false;
    const QString name = QInputDialog::getText(this,
                                               tr("Merge selections"),
                                               tr("Merged selection name:"),
                                               QLineEdit::Normal,
                                               tr("merged"),
                                               &ok);
    if (ok && !name.isEmpty() && !nameExist(name)) {
        const auto merged = STData::aggregate(datasets);
        UserSelection new_selection(merged);
        new_selection.name(name);
        addSelection(new_selection);
    }
}

bool UserSelectionsPage::nameExist(const QString &name)
{
    return std::find_if(m_selections.begin(), m_selections.end(),
                        [&name](const UserSelection& selection)
    {return selection.name() == name;}) != m_selections.end();
}

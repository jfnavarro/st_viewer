#include "UserSelectionsPage.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>

#include "model/UserSelectionsItemModel.h"
#include "dialogs/EditSelectionDialog.h"
#include "analysis/AnalysisDEA.h"
#include "analysis/AnalysisCorrelation.h"
#include "SettingsStyle.h"

#include "ui_selectionsPage.h"

using namespace Style;

UserSelectionsPage::UserSelectionsPage(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::UserSelections())
    , m_waiting_spinner(nullptr)
{
    m_ui->setupUi(this);
    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    m_ui->userSelectionsPageWidget->setStyleSheet("QWidget#userSelectionsPageWidget "
                                                  + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // initialize waiting spinner
    m_waiting_spinner.reset(new WaitingSpinnerWidget(this, true, true));
    m_waiting_spinner->setRoundness(70.0);
    m_waiting_spinner->setMinimumTrailOpacity(15.0);
    m_waiting_spinner->setTrailFadePercentage(70.0);
    m_waiting_spinner->setNumberOfLines(12);
    m_waiting_spinner->setLineLength(20);
    m_waiting_spinner->setLineWidth(10);
    m_waiting_spinner->setInnerRadius(20);
    m_waiting_spinner->setRevolutionsPerSecond(1);
    m_waiting_spinner->setColor(QColor(0, 155, 60));

    // connect signals
    connect(m_ui->filterLineEdit, &QLineEdit::textChanged,
            selectionsProxyModel(), &QSortFilterProxyModel::setFilterFixedString);
    connect(m_ui->removeSelection, &QPushButton::clicked,
            this, &UserSelectionsPage::slotRemoveSelection);
    connect(m_ui->exportSelection, &QPushButton::clicked,
            this, &UserSelectionsPage::slotExportSelection);
    connect(m_ui->importSelection, &QPushButton::clicked,
            this, &UserSelectionsPage::slotImportSelection);
    connect(m_ui->editSelection, &QPushButton::clicked,
            this, &UserSelectionsPage::slotEditSelection);
    connect(m_ui->ddaAnalysis, &QPushButton::clicked,
            this, &UserSelectionsPage::slotPerformDEA);
    connect(m_ui->correlationAnalysis, &QPushButton::clicked,
            this, &UserSelectionsPage::slotPerformCorrelation);
    connect(m_ui->selections_tableView,
            &UserSelectionTableView::clicked,
            this, &UserSelectionsPage::slotSelectionSelected);
    connect(m_ui->showGenes, &QPushButton::clicked, this, &UserSelectionsPage::slotShowGenes);
    connect(m_ui->showSpots, &QPushButton::clicked, this, &UserSelectionsPage::slotShowSpots);

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
    m_ui->exportSelection->setEnabled(false);
    m_ui->ddaAnalysis->setEnabled(false);
    m_ui->editSelection->setEnabled(false);
    m_ui->showGenes->setEnabled(false);
    m_ui->showSpots->setEnabled(false);
    m_ui->importSelection->setEnabled(true);
    m_ui->correlationAnalysis->setEnabled(false);
}


void UserSelectionsPage::slotSelectionSelected(QModelIndex index)
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (!index.isValid() || currentSelection.empty()) {
        return;
    }
    const bool enableMultiple = currentSelection.size() > 1;
    const bool enableSingle = currentSelection.size() == 1;
    // configure UI controls if we select 1 or more selections
    m_ui->removeSelection->setEnabled(enableMultiple || enableSingle);
    m_ui->exportSelection->setEnabled(enableSingle);
    m_ui->importSelection->setEnabled(enableSingle);
    m_ui->ddaAnalysis->setEnabled(enableMultiple);
    m_ui->editSelection->setEnabled(enableSingle);
    m_ui->showGenes->setEnabled(enableSingle);
    m_ui->showSpots->setEnabled(enableSingle);
    m_ui->correlationAnalysis->setEnabled(enableMultiple);
}

void UserSelectionsPage::addSelection(const UserSelection& selection)
{
    m_selections.append(selection);
    selectionsUpdated();
}

void UserSelectionsPage::selectionsUpdated()
{
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
    for (auto selection : currentSelections) {
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

    // currentSelection should only have one element
    const auto selectionItem = currentSelection.front();

    // export selection
    try {
        STData::save(filename, selectionItem.data());
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
    auto selection = currentSelection.front();

    // creates a selection dialog with the current fields
    QScopedPointer<EditSelectionDialog> editSelection(
        new EditSelectionDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editSelection->setWindowIcon(QIcon());
    editSelection->setName(selection.name());
    editSelection->setComment(selection.comment());
    if (editSelection->exec() == EditSelectionDialog::Accepted
        && (editSelection->getName() != selection.name()
            || editSelection->getComment() != selection.comment())
        && !editSelection->getName().isNull() && !editSelection->getName().isEmpty()) {

        const int index = m_selections.indexOf(selection);
        Q_ASSERT(index != -1);

        // update fields
        selection.name(editSelection->getName());
        selection.comment(editSelection->getComment());

        // update object in the container
        m_selections[index] = selection;

        // update model
        selectionsUpdated();
    }
}

void UserSelectionsPage::slotImportSelection()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open ST Data File (Selection)"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TSV Files (*.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this, tr("ST Data File"), tr("File is incorrect or not readable"));
    } else {
        try {
            auto data = STData::read(filename);
            UserSelection new_selection;
            new_selection.data(data);
            //TODO check that the name does not exist already
            new_selection.name(info.baseName());
            //TODO the meta data (dataset name) is lost here
            //use a JSON metadata file to get it
            addSelection(new_selection);
        } catch (const std::exception &e) {
            QMessageBox::critical(this, tr("ST Data File"), tr("Error parsing file"));
            qDebug() << "Error parsing ST data file (Selection) " << e.what();
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

    /*
    // get the two selection objects
    const auto selectionObject1 = currentSelection.at(0);
    const auto selectionObject2 = currentSelection.at(1);
    // launch the DEA widget
    */
}

void UserSelectionsPage::slotPerformCorrelation()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() != 2) {
        return;
    }

    // get the two selection objects
    //const auto selectionObject1 = currentSelection.at(0);
    //const auto selectionObject2 = currentSelection.at(1);
    // launch the correlation widget
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

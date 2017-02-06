#include "UserSelectionsPage.h"

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QScrollArea>
#include <QDateTime>
#include <QLabel>
#include "ext/QtWaitingSpinner/waitingspinnerwidget.h"
#include "model/UserSelectionsItemModel.h"
#include "dialogs/EditSelectionDialog.h"
#include "analysis/AnalysisDEA.h"
#include "viewPages/SelectionsWidget.h"
#include "SettingsStyle.h"

#include "ui_selectionsPage.h"

using namespace Style;

UserSelectionsPage::UserSelectionsPage(QSharedPointer<DataProxy> dataProxy, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::UserSelections())
    , m_dataProxy(dataProxy)
    , m_selectionsWidget(nullptr)
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
    connect(m_ui->filterLineEdit,
            SIGNAL(textChanged(QString)),
            selectionsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->removeSelection, SIGNAL(clicked(bool)), this, SLOT(slotRemoveSelection()));
    connect(m_ui->exportSelection, SIGNAL(clicked(bool)), this, SLOT(slotExportSelection()));
    connect(m_ui->importSelection, SIGNAL(clicked(bool)), this, SLOT(slotImportSelection()));
    connect(m_ui->editSelection, SIGNAL(clicked(bool)), this, SLOT(slotEditSelection()));
    connect(m_ui->ddaAnalysis, SIGNAL(clicked(bool)), this, SLOT(slotPerformDEA()));
    connect(m_ui->clusterAnalysis, SIGNAL(clicked(bool)), this, SLOT(slotPerformClustering()));
    connect(m_ui->selections_tableView,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(slotSelectionSelected(QModelIndex)));
    connect(m_ui->showTissue, SIGNAL(clicked(bool)), this, SLOT(slotShowTissue()));
    connect(m_ui->showGenes, SIGNAL(clicked(bool)), this, SLOT(slotShowGenes()));
    connect(m_ui->showSpots, SIGNAL(clicked(bool)), this, SLOT(slotShowSpots()));

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
    m_ui->showTissue->setEnabled(false);
    m_ui->showGenes->setEnabled(false);
    m_ui->showSpots->setEnabled(false);
    m_ui->importSelection->setEnabled(true);
    m_ui->clusterAnalysis->setEnabled(false);
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
    m_ui->removeSelection->setEnabled(enableMultiple);
    m_ui->exportSelection->setEnabled(enableSingle);
    m_ui->ddaAnalysis->setEnabled(enableMultiple);
    m_ui->editSelection->setEnabled(enableSingle);
    m_ui->showTissue->setEnabled(enableSingle);
    m_ui->showGenes->setEnabled(enableSingle);
    m_ui->showSpots->setEnabled(enableSingle);
    m_ui->clusterAnalysis->setEnabled(enableMultiple);
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

    //TODO remove the selections

    // update the model
    slotSelectionsUpdated();
}

void UserSelectionsPage::slotExportSelection()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Export File"),
                                                    QDir::homePath(),
                                                    QString("%1").arg(tr("Text Files (*.txt)")));
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

    // create file
    QFile textFile(filename);

    // currentSelection should only have one element
    const auto selectionItem = currentSelection.front();

    // export selection
    selectionItem.save(textFile);

    // close file
    textFile.close();
}

void UserSelectionsPage::slotEditSelection()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);

    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    // currentSelection should only have one element
    const auto selection = currentSelection.front();

    // creates a selection dialog with the current fields
    QScopedPointer<EditSelectionDialog> editSelection(
        new EditSelectionDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editSelection->setWindowIcon(QIcon());
    editSelection->setName(selection.name());
    editSelection->setComment(selection.comment());
    editSelection->setType(UserSelection::typeToQString(selection.type()));
    if (editSelection->exec() == EditSelectionDialog::Accepted
        && (editSelection->getName() != selection.name()
            || editSelection->getComment() != selection.comment()
            || editSelection->getType() != UserSelection::typeToQString(selection.type()))
        && !editSelection->getName().isNull() && !editSelection->getName().isEmpty()) {

        // update fields
        selection.name(editSelection->getName());
        selection.comment(editSelection->getComment());
        selection.type(UserSelection::QStringToType(editSelection->getType()));

        //TODO update object in the container

        // update model
        slotSelectionsUpdated()
    }
}

void UserSelectionsPage::slotImportSelection()
{
    //TODO show file dialog and create a new selection
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
    // creates the DEA widget and shows it
    QScopedPointer<AnalysisDEA> analysisDEA(new AnalysisDEA(*selectionObject1, *selectionObject2));
    analysisDEA->exec();
}

void UserSelectionsPage::slotPerformClustering()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty()) {
        return;
    }

    //TODO invoke clustering dialog with the selections
}

void UserSelectionsPage::slotShowTissue()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    const auto selectionObject = currentSelection.front();

    // if no snapshot returns
    QByteArray tissue_snapshot = selectionObject.tissueSnapShot();
    if (tissue_snapshot.isNull() || tissue_snapshot.isEmpty()) {
        return;
    }

    // create a widget that shows the tissue snapshot
    QByteArray image_ba = QByteArray::fromBase64(tissue_snapshot);
    QImage image;
    image.loadFromData(image_ba);
    if (image.isNull()) {
        return;
    }

    // create a widget to show the image
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

void UserSelectionsPage::slotShowGenes()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    const auto selectionObject = currentSelection.front();
    // lazy init
    if (m_genesWidget.isNull()) {
        m_genesWidget.reset(new SelectionsWidget(nullptr, Qt::Dialog));
    }
    // update model
    m_genesWidget->slotLoadModel(selectionObject.getGenesCounts());
    m_genesWidget->show();
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
    // lazy init
    if (m_spotsWidget.isNull()) {
        m_spotsWidget.reset(new SelectionsWidget(nullptr, Qt::Dialog));
    }
    // update model
    m_spotsWidget->slotLoadModel(selectionObject.getSpotsCounts());
    m_spotsWidget->show();
}

void UserSelectionsPage::slotSelectionsUpdated()
{
    // update model with downloaded genes selections
    selectionsModel()->loadUserSelections(m_selections);
    clearControls();
}

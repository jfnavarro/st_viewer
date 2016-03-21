#include "UserSelectionsPage.h"

#include "ui_selectionsPage.h"

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QScrollArea>
#include <QDateTime>
#include "QtWaitingSpinner/waitingspinnerwidget.h"

#include "dataModel/User.h"
#include "utils/Utils.h"
#include "io/FeatureExporter.h"
#include "model/UserSelectionsItemModel.h"
#include "dialogs/EditSelectionDialog.h"
#include "analysis/AnalysisDEA.h"
#include "viewPages/SelectionsWidget.h"


using namespace Globals;

UserSelectionsPage::UserSelectionsPage(QPointer<DataProxy> dataProxy, QWidget* parent)
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
    m_waiting_spinner = new WaitingSpinnerWidget(this, true, true);
    m_waiting_spinner->setRoundness(70.0);
    m_waiting_spinner->setMinimumTrailOpacity(15.0);
    m_waiting_spinner->setTrailFadePercentage(70.0);
    m_waiting_spinner->setNumberOfLines(12);
    m_waiting_spinner->setLineLength(20);
    m_waiting_spinner->setLineWidth(10);
    m_waiting_spinner->setInnerRadius(20);
    m_waiting_spinner->setRevolutionsPerSecond(1);
    m_waiting_spinner->setColor(QColor(0,155,60));

    // connect signals
    connect(m_ui->filterLineEdit,
            SIGNAL(textChanged(QString)),
            selectionsProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(m_ui->removeSelection, SIGNAL(clicked(bool)), this, SLOT(slotRemoveSelection()));
    connect(m_ui->exportSelection, SIGNAL(clicked(bool)), this, SLOT(slotExportSelection()));
    connect(m_ui->ddaAnalysis, SIGNAL(clicked(bool)), this, SLOT(slotPerformDEA()));
    connect(m_ui->selections_tableView,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(slotSelectionSelected(QModelIndex)));
    connect(m_ui->editSelection, SIGNAL(clicked(bool)), this, SLOT(slotEditSelection()));
    connect(m_ui->showTissue, SIGNAL(clicked(bool)), this, SLOT(slotShowTissue()));
    connect(m_ui->showTable, SIGNAL(clicked(bool)), this, SLOT(slotShowTable()));
    connect(m_ui->saveDB, SIGNAL(clicked(bool)), this, SLOT(slotSaveSelection()));
    connect(m_ui->importSelection, SIGNAL(clicked(bool)), this, SLOT(slotImportSelection()));

    // connect data proxy signal
    connect(m_dataProxy.data(),
            SIGNAL(signalUserSelectionsDownloaded(DataProxy::DownloadStatus)),
            this,
            SLOT(slotSelectionsDownloaded(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalUserSelectionModified(DataProxy::DownloadStatus)),
            this,
            SLOT(slotSelectionModified(DataProxy::DownloadStatus)));
    connect(m_dataProxy.data(),
            SIGNAL(signalUserSelectionDeleted(DataProxy::DownloadStatus)),
            this,
            SLOT(slotSelectionModified(DataProxy::DownloadStatus)));

    clearControls();
}

UserSelectionsPage::~UserSelectionsPage()
{

}

void UserSelectionsPage::clean()
{
    selectionsModel()->clear();
}

QSortFilterProxyModel* UserSelectionsPage::selectionsProxyModel()
{
    QSortFilterProxyModel* selectionsProxyModel
        = qobject_cast<QSortFilterProxyModel*>(m_ui->selections_tableView->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}

UserSelectionsItemModel* UserSelectionsPage::selectionsModel()
{
    UserSelectionsItemModel* model
        = qobject_cast<UserSelectionsItemModel*>(selectionsProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

void UserSelectionsPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    clearControls();
    // load selections
    loadSelections();
}

void UserSelectionsPage::clearControls()
{
    // clear selection/focus
    m_ui->selections_tableView->clearSelection();
    m_ui->selections_tableView->clearFocus();

    // remove, edit, dda and export not enable by default
    m_ui->removeSelection->setEnabled(false);
    m_ui->exportSelection->setEnabled(false);
    m_ui->ddaAnalysis->setEnabled(false);
    m_ui->editSelection->setEnabled(false);
    m_ui->showTissue->setEnabled(false);
    m_ui->showTable->setEnabled(false);
    m_ui->saveDB->setEnabled(false);
    m_ui->importSelection->setEnabled(false);
    m_ui->cluster->setEnabled(false);
}

void UserSelectionsPage::loadSelections()
{
    if (!m_dataProxy->userLogIn()) {
        return;
    }

    // load selections
    m_waiting_spinner->start();
    m_dataProxy->loadUserSelections();
    m_dataProxy->activateCurrentDownloads();
}

void UserSelectionsPage::slotSelectionsDownloaded(const DataProxy::DownloadStatus status)
{
    if (status == DataProxy::Success) {
        // update model
        slotSelectionsUpdated();
    }
    m_waiting_spinner->stop();
}

void UserSelectionsPage::slotSelectionModified(const DataProxy::DownloadStatus status)
{
    if (status == DataProxy::Success) {
        // re-upload genes selections after an update
        loadSelections();
    }
    m_waiting_spinner->stop();
}

void UserSelectionsPage::slotSelectionSelected(QModelIndex index)
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    const bool enableDDA = index.isValid() && currentSelection.size() == 2;
    const bool enableRest = index.isValid() && currentSelection.size() == 1;
    const auto selection = currentSelection.first();
    Q_ASSERT(!selection.isNull());
    // configure UI controls if we select 1 or 2 selections
    m_ui->removeSelection->setEnabled(enableRest);
    m_ui->exportSelection->setEnabled(enableRest);
    m_ui->ddaAnalysis->setEnabled(enableDDA);
    m_ui->editSelection->setEnabled(enableRest);
    m_ui->showTissue->setEnabled(enableRest);
    m_ui->showTable->setEnabled(enableRest);
    m_ui->saveDB->setEnabled(enableRest && !selection->saved() && m_dataProxy->userLogIn());
    m_ui->cluster->setEnabled(false);
}

void UserSelectionsPage::slotRemoveSelection()
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }

    const int answer
        = QMessageBox::warning(this,
                               tr("Remove Selection"),
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
    if (selectionItem->saved()) {
        m_waiting_spinner->start();
        m_dataProxy->removeSelection(selectionItem->id());
        m_dataProxy->activateCurrentDownloads();
    } else {
        m_dataProxy->parseRemoveUserSelection(selectionItem->id());
        slotSelectionsUpdated();
    }
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
        QMessageBox::critical(this,
                              tr("Export Selection"),
                              tr("The directory is not writable"));
        return;
    }

    // create file
    QFile textFile(filename);

    // currentSelection should only have one element
    auto selectionItem = currentSelection.first();
    Q_ASSERT(!selectionItem.isNull());

    // export selection
    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        FeatureExporter exporter
            = FeatureExporter(FeatureExporter::SimpleFull, FeatureExporter::TabDelimited);
        exporter.exportItem(textFile, selectionItem->selectedFeatures());
    }

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
    Q_ASSERT(!currentSelection.first().isNull());
    UserSelection selectionItem(*currentSelection.first());

    // creates a selection dialog with the current fields
    QScopedPointer<EditSelectionDialog> editSelection(
        new EditSelectionDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editSelection->setWindowIcon(QIcon());
    editSelection->setName(selectionItem.name());
    editSelection->setComment(selectionItem.comment());
    editSelection->setType(UserSelection::typeToQString(selectionItem.type()));
    if (editSelection->exec() == EditSelectionDialog::Accepted
        && (editSelection->getName() != selectionItem.name()
            || editSelection->getComment() != selectionItem.comment()
            || editSelection->getType() != UserSelection::typeToQString(selectionItem.type()))
        && !editSelection->getName().isNull()
        && !editSelection->getName().isEmpty()) {

        // update fields
        selectionItem.name(editSelection->getName());
        selectionItem.comment(editSelection->getComment());
        selectionItem.type(UserSelection::QStringToType(editSelection->getType()));

        if (m_dataProxy->userLogIn()) {
            // update the selection object in the database
            m_waiting_spinner->start();
            m_dataProxy->updateUserSelection(selectionItem);
            m_dataProxy->activateCurrentDownloads();
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
    Q_ASSERT(!selectionObject1.isNull());
    const auto selectionObject2 = currentSelection.at(1);
    Q_ASSERT(!selectionObject2.isNull());
    // creates the DEA widget and shows it
    QScopedPointer<AnalysisDEA> analysisDEA(new AnalysisDEA(*selectionObject1, *selectionObject2));
    analysisDEA->exec();
}

void UserSelectionsPage::slotShowTissue()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    const auto selectionObject = currentSelection.first();

    // if no snapshot returns
    QByteArray tissue_snapshot = selectionObject->tissueSnapShot();
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
    QWidget* image_widget = new QWidget();
    image_widget->setAttribute(Qt::WA_DeleteOnClose);
    image_widget->setMinimumSize(600, 600);
    QVBoxLayout* layout1 = new QVBoxLayout(image_widget);
    QScrollArea* image_scroll = new QScrollArea();
    layout1->addWidget(image_scroll);
    QVBoxLayout* layout = new QVBoxLayout(image_scroll);
    QLabel* image_label = new QLabel();
    image_label->setPixmap(QPixmap::fromImage(image));
    image_label->setScaledContents(true);
    layout->addWidget(image_label);
    image_widget->show();
}

void UserSelectionsPage::slotSaveSelection()
{
    if (!m_dataProxy->userLogIn()) {
        return;
    }
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    const auto selectionObject = currentSelection.first();
    // add the user to the selection
    const auto user = m_dataProxy->getUser();
    Q_ASSERT(!user.isNull());
    selectionObject->userId(user->id());

    const int answer
        = QMessageBox::warning(this,
                               tr("Save Selection"),
                               tr("Are you really sure you want to save the selection?"),
                               QMessageBox::Yes,
                               QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    // save the selection object in the database
    m_waiting_spinner->start();
    m_dataProxy->addUserSelection(*selectionObject, true);
    m_dataProxy->activateCurrentDownloads();
}

void UserSelectionsPage::slotShowTable()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    const auto selectionObject = currentSelection.first();
    // lazy init
    if (m_selectionsWidget.isNull()) {
        m_selectionsWidget = new SelectionsWidget(nullptr, Qt::SubWindow);
    }
    // update model
    m_selectionsWidget->slotLoadModel(selectionObject->selectedGenes());
    m_selectionsWidget->show();
}

void UserSelectionsPage::slotSelectionsUpdated()
{
    // update model with downloaded genes selections
    selectionsModel()->loadUserSelections(m_dataProxy->getUserSelectionList());
    clearControls();
}

void UserSelectionsPage::slotImportSelection()
{
    //TODO
}

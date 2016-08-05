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

#include "dataModel/User.h"
#include "io/FeatureExporter.h"
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
    connect(m_ui->ddaAnalysis, SIGNAL(clicked(bool)), this, SLOT(slotPerformDEA()));
    connect(m_ui->selections_tableView,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(slotSelectionSelected(QModelIndex)));
    connect(m_ui->editSelection, SIGNAL(clicked(bool)), this, SLOT(slotEditSelection()));
    connect(m_ui->showTissue, SIGNAL(clicked(bool)), this, SLOT(slotShowTissue()));
    connect(m_ui->showTable, SIGNAL(clicked(bool)), this, SLOT(slotShowTable()));
    connect(m_ui->saveDB, SIGNAL(clicked(bool)), this, SLOT(slotSaveSelection()));
    //connect(m_ui->importSelection, SIGNAL(clicked(bool)), this, SLOT(slotImportSelection()));

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
    // load selections
    loadSelections();
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
    m_ui->showTable->setEnabled(false);
    m_ui->saveDB->setEnabled(false);
    //m_ui->importSelection->setEnabled(true);
    //m_ui->cluster->setEnabled(false);
}

void UserSelectionsPage::loadSelections()
{
    if (!m_dataProxy->userLogIn()) {
        return;
    }

    // load selections
    m_waiting_spinner->start();
    m_dataProxy->loadUserSelections();
    m_waiting_spinner->stop();
    // update the model
    slotSelectionsUpdated();
}

void UserSelectionsPage::slotSelectionSelected(QModelIndex index)
{
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (!index.isValid() || currentSelection.empty()) {
        return;
    }
    const bool enableDDA = currentSelection.size() == 2;
    const bool enableRest = currentSelection.size() == 1;
    const auto selection = currentSelection.front();
    Q_ASSERT(selection);
    // configure UI controls if we select 1 or 2 selections
    m_ui->removeSelection->setEnabled(true);
    m_ui->exportSelection->setEnabled(enableRest);
    m_ui->ddaAnalysis->setEnabled(enableDDA);
    m_ui->editSelection->setEnabled(enableRest);
    m_ui->showTissue->setEnabled(enableRest);
    m_ui->showTable->setEnabled(enableRest);
    m_ui->saveDB->setEnabled(enableRest && !selection->saved() && m_dataProxy->userLogIn());
    //m_ui->cluster->setEnabled(false);
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

    m_waiting_spinner->start();
    for (const auto &selection : currentSelections) {
        Q_ASSERT(selection);
        m_dataProxy->removeSelection(selection->id(), selection->saved());
    }
    m_waiting_spinner->stop();
    // update the selections
    loadSelections();
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
    Q_ASSERT(selectionItem);

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
    const auto selection = currentSelection.front();
    Q_ASSERT(selection);

    // creates a selection dialog with the current fields
    QScopedPointer<EditSelectionDialog> editSelection(
        new EditSelectionDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    editSelection->setWindowIcon(QIcon());
    editSelection->setName(selection->name());
    editSelection->setComment(selection->comment());
    editSelection->setType(UserSelection::typeToQString(selection->type()));
    if (editSelection->exec() == EditSelectionDialog::Accepted
        && (editSelection->getName() != selection->name()
            || editSelection->getComment() != selection->comment()
            || editSelection->getType() != UserSelection::typeToQString(selection->type()))
        && !editSelection->getName().isNull() && !editSelection->getName().isEmpty()) {

        // update fields
        selection->name(editSelection->getName());
        selection->comment(editSelection->getComment());
        selection->type(UserSelection::QStringToType(editSelection->getType()));

        if (m_dataProxy->userLogIn() && selection->saved()) {
            // update the selection object in the database
            m_waiting_spinner->start();
            m_dataProxy->updateUserSelection(*selection);
            m_waiting_spinner->stop();
        }
        // NOTE no need for this
        // slotSelectionsUpdated();
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
    Q_ASSERT(selectionObject1);
    const auto selectionObject2 = currentSelection.at(1);
    Q_ASSERT(selectionObject2);
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
    const auto selectionObject = currentSelection.front();

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

    const int answer
        = QMessageBox::warning(this,
                               tr("Save Selection"),
                               tr("Are you really sure you want to save the selection?"),
                               QMessageBox::Yes,
                               QMessageBox::No | QMessageBox::Escape);

    if (answer != QMessageBox::Yes) {
        return;
    }

    const auto selectionObject = currentSelection.front();
    // add the user to the selection
    const auto user = m_dataProxy->getUser();
    Q_ASSERT(user);
    selectionObject->userId(user->id());
    // must set Id to null before saving it to avoid a DB error
    //TODO find a cleaner way to solve this
    selectionObject->id(QString());

    // save the selection object in the database and remove the old one
    m_waiting_spinner->start();
    m_dataProxy->addUserSelection(*selectionObject, true);
    m_dataProxy->removeSelection(selectionObject->id(), false);
    m_waiting_spinner->stop();
    // update the selections
    loadSelections();
}

void UserSelectionsPage::slotShowTable()
{
    // get the selected object (should be only one)
    const auto selected = m_ui->selections_tableView->userSelecionTableItemSelection();
    const auto currentSelection = selectionsModel()->getSelections(selected);
    if (currentSelection.empty() || currentSelection.size() > 1) {
        return;
    }
    const auto selectionObject = currentSelection.front();
    // lazy init
    if (m_selectionsWidget.isNull()) {
        m_selectionsWidget.reset(new SelectionsWidget(nullptr, Qt::Dialog));
    }
    // update model
    m_selectionsWidget->slotLoadModel(selectionObject->getGeneCounts());
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
    // TODO
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SelectionsWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSortFilterProxyModel>

#include "viewTables/GeneSelectionTableView.h"
#include "model/GeneSelectionItemModel.h"
#include "utils/SetTips.h"

using namespace Globals;

SelectionsWidget::SelectionsWidget(QWidget *parent) :
    QWidget(parent),
    m_geneSelectionFilterLineEdit(nullptr),
    m_selections_tableview(nullptr)
{
    QVBoxLayout *selectionLayout = new QVBoxLayout();
    selectionLayout->setSpacing(0);
    selectionLayout->setContentsMargins(10, 10, 10, 10);
    QHBoxLayout *selectionBottonsLayout = new QHBoxLayout();
    selectionBottonsLayout->setSpacing(0);
    selectionBottonsLayout->setContentsMargins(0, 5, 0, 5);

    //add separation between buttons
    selectionBottonsLayout->addSpacing(10);

    QPushButton *saveSelection = new QPushButton(this);
    configureButton(saveSelection,
                    QIcon(QStringLiteral(":/images/save-selection.png")),
                    tr("Save the current selection in the cloud"));
    selectionBottonsLayout->addWidget(saveSelection);
    //add separation
    selectionBottonsLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *exportGenesSelection = new QPushButton(this);
    configureButton(exportGenesSelection,
                    QIcon(QStringLiteral(":/images/export-genes.png")),
                    tr("Export the currently selected genes to a file"));
    selectionBottonsLayout->addWidget(exportGenesSelection);
    //add separation
    selectionBottonsLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *exportFeaturesSelection = new QPushButton(this);
    configureButton(exportFeaturesSelection,
                    QIcon(QStringLiteral(":/images/export-features.png")),
                    tr("Export the currently selected features to a file"));
    selectionBottonsLayout->addWidget(exportFeaturesSelection);
    //add separation
    selectionBottonsLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *clearSelection = new QPushButton(this);
    configureButton(clearSelection,
                    QIcon(QStringLiteral(":/images/remove-selection.png")),
                    tr("Remove the current selection"));
    selectionBottonsLayout->addWidget(clearSelection);
    //add separation
    selectionBottonsLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    m_geneSelectionFilterLineEdit = new QLineEdit(this);
    m_geneSelectionFilterLineEdit->setFixedSize(CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE);
    m_geneSelectionFilterLineEdit->setClearButtonEnabled(true);
    m_geneSelectionFilterLineEdit->setStyleSheet(CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE);
    setToolTipAndStatusTip(
            tr("Search by gene name"),
            m_geneSelectionFilterLineEdit);
    selectionBottonsLayout->addWidget(m_geneSelectionFilterLineEdit);
    selectionBottonsLayout->setAlignment(m_geneSelectionFilterLineEdit, Qt::AlignRight);

    //add buttons layout to main layout
    selectionLayout->addLayout(selectionBottonsLayout);

    //add table to main layout
    m_selections_tableview = new GeneSelectionTableView(this);
    selectionLayout->addWidget(m_selections_tableview);

    //set the main layout
    setLayout(selectionLayout);

    //connections
    connect(m_geneSelectionFilterLineEdit, SIGNAL(textChanged(QString)), m_selections_tableview,
            SLOT(setGeneNameFilter(QString)));
    connect(exportGenesSelection, SIGNAL(clicked(bool)),
            this, SIGNAL(signalExportGenesSelection()));
    connect(exportFeaturesSelection, SIGNAL(clicked(bool)),
            this, SIGNAL(signalExportFeaturesSelection()));
    connect(saveSelection, SIGNAL(clicked(bool)), this, SIGNAL(signalSaveSelection()));
    connect(clearSelection, SIGNAL(clicked(bool)), this, SIGNAL(signalClearSelection()));
}

SelectionsWidget::~SelectionsWidget()
{

}

void SelectionsWidget::clear()
{
    m_geneSelectionFilterLineEdit->clearFocus();
    m_geneSelectionFilterLineEdit->clear();

    m_selections_tableview->clearSelection();
    m_selections_tableview->clearFocus();

    getModel()->clearSelectedGenes();
}

void SelectionsWidget::slotLoadModel(const GeneSelection::selectedItemsList &geneList)
{
    getModel()->loadSelectedGenes(geneList);
}

void SelectionsWidget::configureButton(QPushButton *button, const QIcon icon, const QString tooltip)
{
    Q_ASSERT(button != nullptr);
    button->setIcon(icon);
    button->setIconSize(CELL_PAGE_SUB_MENU_ICON_SIZE);
    button->setFixedSize(CELL_PAGE_SUB_MENU_BUTTON_SIZE);
    button->setStyleSheet(CELL_PAGE_SUB_MENU_BUTTON_STYLE);
    button->setCursor(Qt::PointingHandCursor);
    setToolTipAndStatusTip(tooltip, button);
}

GeneSelectionItemModel *SelectionsWidget::getModel()
{
    GeneSelectionItemModel *selectionModel =
            qobject_cast<GeneSelectionItemModel*>(getProxyModel()->sourceModel());
    Q_ASSERT(selectionModel);
    return selectionModel;
}

QSortFilterProxyModel *SelectionsWidget::getProxyModel()
{
    QSortFilterProxyModel *selectionsProxyModel =
            qobject_cast<QSortFilterProxyModel*>(m_selections_tableview->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}

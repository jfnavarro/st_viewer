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

static const QSize BUTTON_SIZE = QSize(40, 30);
static const QSize LINE_EDIT_SIZE = QSize(150, 25);
static const QString BUTTON_STYLE = "border: 1px solid rgb(209, 209, 209); "
                                    "border-radius: 5px; background-color: rgb(255, 255, 255);";
static const QString LINE_EDIT_STYLE = "border: 1px solid rgb(209, 209, 209); "
                                       "border-radius: 5px; "
                                       "background-color: rgb(255, 255, 255); "
                                       "selection-background-color: darkgray;";
static const int BUTTON_SPACE = 15;

//TODO we should have a Factory for buttons and GUI elements

SelectionsWidget::SelectionsWidget(QWidget *parent) :
    QWidget(parent),
    m_saveSelection(nullptr),
    m_exportGenesSelection(nullptr),
    m_exportFeaturesSelection(nullptr),
    m_clearSelection(nullptr),
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

    m_saveSelection = new QPushButton(this);
    m_saveSelection->setIcon(QIcon(QStringLiteral(":/images/save-selection.png")));
    m_saveSelection->setIconSize(BUTTON_SIZE);
    m_saveSelection->setFixedSize(BUTTON_SIZE);
    m_saveSelection->setStyleSheet(BUTTON_STYLE);
    m_saveSelection->setCursor(Qt::PointingHandCursor);
    setToolTipAndStatusTip(
            tr("Save the current selection in the cloud"),
            m_saveSelection);
    selectionBottonsLayout->addWidget(m_saveSelection);
    //add separation between buttons
    selectionBottonsLayout->addSpacing(BUTTON_SPACE);

    m_exportGenesSelection = new QPushButton(this);
    m_exportGenesSelection->setIcon(QIcon(QStringLiteral(":/images/export-genes.png")));
    m_exportGenesSelection->setIconSize(BUTTON_SIZE);
    m_exportGenesSelection->setFixedSize(BUTTON_SIZE);
    m_exportGenesSelection->setStyleSheet(BUTTON_STYLE);
    m_exportGenesSelection->setCursor(Qt::PointingHandCursor);
    setToolTipAndStatusTip(
            tr("Export the currently selected genes to a file"),
            m_exportGenesSelection);
    selectionBottonsLayout->addWidget(m_exportGenesSelection);
    //add separation between buttons
    selectionBottonsLayout->addSpacing(BUTTON_SPACE);

    m_exportFeaturesSelection = new QPushButton(this);
    m_exportFeaturesSelection->setIcon(QIcon(QStringLiteral(":/images/export-features.png")));
    m_exportFeaturesSelection->setIconSize(BUTTON_SIZE);
    m_exportFeaturesSelection->setFixedSize(BUTTON_SIZE);
    m_exportFeaturesSelection->setStyleSheet(BUTTON_STYLE);
    m_exportFeaturesSelection->setCursor(Qt::PointingHandCursor);
    setToolTipAndStatusTip(
            tr("Export the currently selected features to a file"),
            m_exportFeaturesSelection);
    selectionBottonsLayout->addWidget(m_exportFeaturesSelection);
    //add separation between buttons
    selectionBottonsLayout->addSpacing(BUTTON_SPACE);

    m_clearSelection = new QPushButton(this);
    m_clearSelection->setIcon(QIcon(QStringLiteral(":/images/remove-selection.png")));
    m_clearSelection->setIconSize(BUTTON_SIZE);
    m_clearSelection->setFixedSize(BUTTON_SIZE);
    m_clearSelection->setStyleSheet(BUTTON_STYLE);
    m_clearSelection->setCursor(Qt::PointingHandCursor);
    setToolTipAndStatusTip(
            tr("Remove the current selection"),
            m_clearSelection);
    selectionBottonsLayout->addWidget(m_clearSelection);
    //add separation and stretch
    selectionBottonsLayout->addSpacing(BUTTON_SPACE);

    m_geneSelectionFilterLineEdit = new QLineEdit(this);
    m_geneSelectionFilterLineEdit->setFixedSize(LINE_EDIT_SIZE);
    m_geneSelectionFilterLineEdit->setClearButtonEnabled(true);
    m_geneSelectionFilterLineEdit->setStyleSheet(LINE_EDIT_STYLE);
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
    // export selection
    connect(m_exportGenesSelection, SIGNAL(clicked(bool)),
            this, SIGNAL(signalExportGenesSelection()));
    connect(m_exportFeaturesSelection, SIGNAL(clicked(bool)),
            this, SIGNAL(signalExportFeaturesSelection()));

    // save selection
    connect(m_saveSelection, SIGNAL(clicked(bool)), this, SIGNAL(signalSaveSelection()));
    // selection actions
    connect(m_clearSelection, SIGNAL(clicked(bool)), this, SIGNAL(signalClearSelection()));
}

SelectionsWidget::~SelectionsWidget()
{
    m_saveSelection->deleteLater();
    m_saveSelection = nullptr;

    m_exportGenesSelection->deleteLater();
    m_exportGenesSelection = nullptr;

    m_exportFeaturesSelection->deleteLater();
    m_exportFeaturesSelection = nullptr;

    m_clearSelection->deleteLater();
    m_clearSelection = nullptr;

    m_geneSelectionFilterLineEdit->deleteLater();
    m_geneSelectionFilterLineEdit = nullptr;

    m_selections_tableview->deleteLater();
    m_selections_tableview = nullptr;
}

void SelectionsWidget::clear()
{
    m_geneSelectionFilterLineEdit->clearFocus();
    m_geneSelectionFilterLineEdit->clear();

    m_selections_tableview->clearSelection();
    m_selections_tableview->clearFocus();

    //TODO this causes a error when using dataproxy
    getModel()->clearSelectedGenes();
}

void SelectionsWidget::slotLoadModel(const GeneSelection::selectedItemsList &geneList)
{
    getModel()->loadSelectedGenes(geneList);
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

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
    QHBoxLayout *selectionBottonsLayout = new QHBoxLayout();

    m_saveSelection = new QPushButton(this);
    m_saveSelection->setIcon(QIcon(QStringLiteral(":/images/file_export.png")));
    setToolTipAndStatusTip(
            tr("Save the current selection in the cloud"),
            m_saveSelection);
    selectionBottonsLayout->addWidget(m_saveSelection);

    m_exportGenesSelection = new QPushButton(this);
    m_exportGenesSelection->setIcon(QIcon(QStringLiteral(":/images/export.png")));
    setToolTipAndStatusTip(
            tr("Export the currently selected genes to a file"),
            m_exportGenesSelection);
    selectionBottonsLayout->addWidget(m_exportGenesSelection);

    m_exportFeaturesSelection = new QPushButton(this);
    m_exportFeaturesSelection->setIcon(QIcon(QStringLiteral(":/images/exportall.png")));
    setToolTipAndStatusTip(
            tr("Export the currently selected features to a file"),
            m_exportFeaturesSelection);
    selectionBottonsLayout->addWidget(m_exportFeaturesSelection);

    m_clearSelection = new QPushButton(this);
    m_clearSelection->setIcon(QIcon(QStringLiteral(":/images/clear2.png")));
    setToolTipAndStatusTip(
            tr("Remove the current selection"),
            m_clearSelection);
    selectionBottonsLayout->addWidget(m_clearSelection);

    //add separation and stretch in between the search box
    selectionBottonsLayout->addSpacing(5);
    selectionBottonsLayout->addStretch(5);

    m_geneSelectionFilterLineEdit = new QLineEdit(this);
    m_geneSelectionFilterLineEdit->setFixedSize(150, 20);
    m_geneSelectionFilterLineEdit->setClearButtonEnabled(true);
    setToolTipAndStatusTip(
            tr("Search by gene name"),
            m_geneSelectionFilterLineEdit);
    selectionBottonsLayout->addWidget(m_geneSelectionFilterLineEdit);
    selectionBottonsLayout->setAlignment(m_geneSelectionFilterLineEdit, Qt::AlignRight);

    selectionLayout->addLayout(selectionBottonsLayout);

    m_selections_tableview = new GeneSelectionTableView(this);
    selectionLayout->addWidget(m_selections_tableview);

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

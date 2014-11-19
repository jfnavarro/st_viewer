/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GenesWidget.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QPushButton>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QColorDialog>

#include "viewTables/GenesTableView.h"
#include "model/GeneFeatureItemModel.h"
#include "utils/SetTips.h"

GenesWidget::GenesWidget(QWidget *parent) :
    QWidget(parent),
    m_selectionAllButton(nullptr),
    m_selectionClearAllButton(nullptr),
    m_lineEdit(nullptr),
    m_genes_tableview(nullptr),
    m_colorList(nullptr),
    m_showColorButton(nullptr),
    m_showSelectedButton(nullptr),
    m_hideSelectedButton(nullptr)
{
    QVBoxLayout *genesLayout = new QVBoxLayout();
    genesLayout->setSpacing(0);
    genesLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *geneListLayout = new QHBoxLayout();
    geneListLayout->setSpacing(0);
    geneListLayout->setContentsMargins(0, 0, 0, 0);

    //create actions buttons
    m_showSelectedButton = new QPushButton(this);
    m_showSelectedButton->setIcon(QIcon(QStringLiteral(":/images/visible.png")));
    setToolTipAndStatusTip(
            tr("Show selected genes"),
            m_showSelectedButton);
    geneListLayout->addWidget(m_showSelectedButton);
    geneListLayout->addSpacing(1);
    geneListLayout->addStretch(1);

    m_hideSelectedButton = new QPushButton(this);
    m_hideSelectedButton->setIcon(QIcon(QStringLiteral(":/images/nonvisible.png")));
    setToolTipAndStatusTip(
            tr("Hide selected genes"),
            m_hideSelectedButton);
    geneListLayout->addWidget(m_hideSelectedButton);
    geneListLayout->addSpacing(1);
    geneListLayout->addStretch(1);

    //create selections buttons
    m_selectionAllButton = new QPushButton(this);
    m_selectionAllButton->setIcon(QIcon(QStringLiteral(":/images/select-all.png")));
    setToolTipAndStatusTip(
            tr("Select all genes"),
            m_selectionAllButton);
    geneListLayout->addWidget(m_selectionAllButton);
    geneListLayout->addSpacing(1);
    geneListLayout->addStretch(1);

    m_selectionClearAllButton = new QPushButton(this);
    m_selectionClearAllButton->setIcon(QIcon(QStringLiteral(":/images/unselect-all.png")));
    setToolTipAndStatusTip(
            tr("Deselect all genes"),
            m_selectionClearAllButton);
    geneListLayout->addWidget(m_selectionClearAllButton);
    geneListLayout->addSpacing(1);
    geneListLayout->addStretch(1);

    //create color selection button
    m_showColorButton = new QPushButton(this);
    m_showColorButton->setIcon(QIcon(QStringLiteral(":/images/select-color.png")));
    setToolTipAndStatusTip(
            tr("Set color of selected genes"),
            m_showColorButton);
    m_colorList = new QColorDialog(Globals::DEFAULT_COLOR_GENE, this);
    //OSX native color dialog gives problems
    m_colorList->setOption(QColorDialog::DontUseNativeDialog, true);
    geneListLayout->addWidget(m_showColorButton);
    geneListLayout->addSpacing(1);
    geneListLayout->addStretch(1);

    //create line edit search
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setFixedSize(150, 20);
    m_lineEdit->setToolTip(tr("Search by gene name..."));
    geneListLayout->addWidget(m_lineEdit);
    geneListLayout->setAlignment(m_lineEdit, Qt::AlignRight);

    //add actions menu to main layout
    genesLayout->addLayout(geneListLayout);

    //create genes table
    m_genes_tableview = new GenesTableView(this);
    //add table to main layout
    genesLayout->addWidget(m_genes_tableview);

    //set main layout
    setLayout(genesLayout);

    //connections
    connect(m_showSelectedButton, SIGNAL(clicked(bool)), this, SLOT(slotShowAllSelected()));
    connect(m_hideSelectedButton, SIGNAL(clicked(bool)), this, SLOT(slotHideAllSelected()));
    connect(m_selectionAllButton,
            SIGNAL(clicked(bool)), m_genes_tableview, SLOT(selectAll()));
    connect(m_selectionAllButton,
            SIGNAL(clicked(bool)), m_genes_tableview, SLOT(setFocus()));
    connect(m_selectionClearAllButton,
            SIGNAL(clicked(bool)), m_genes_tableview, SLOT(clearSelection()));
    connect(m_showColorButton.data(), &QPushButton::clicked, [=]{ m_colorList->show();
                                                                  m_colorList->raise();
                                                                  m_colorList->activateWindow(); });
    connect(m_colorList.data(), &QColorDialog::colorSelected,
            [=]() { slotSetColorAllSelected(m_colorList->currentColor()); });
    connect(m_lineEdit, SIGNAL(textChanged(QString)), m_genes_tableview,
            SLOT(setGeneNameFilter(QString)));
    connect(getModel(), SIGNAL(signalSelectionChanged(DataProxy::GeneList)),
            this,
            SIGNAL(signalSelectionChanged(DataProxy::GeneList)));
    connect(getModel(), SIGNAL(signalColorChanged(DataProxy::GeneList)),
            this,
            SIGNAL(signalColorChanged(DataProxy::GeneList)));
}

GenesWidget::~GenesWidget()
{
    m_selectionAllButton->deleteLater();
    m_selectionAllButton = nullptr;

    m_selectionClearAllButton->deleteLater();
    m_selectionClearAllButton = nullptr;

    m_lineEdit->deleteLater();
    m_lineEdit = nullptr;

    m_genes_tableview->deleteLater();
    m_genes_tableview = nullptr;

    m_colorList->deleteLater();
    m_colorList = nullptr;

    m_showColorButton->deleteLater();
    m_showColorButton = nullptr;

    m_showSelectedButton->deleteLater();
    m_showSelectedButton = nullptr;

    m_hideSelectedButton->deleteLater();
    m_hideSelectedButton = nullptr;
}

void GenesWidget::clear()
{
    m_lineEdit->clearFocus();
    m_lineEdit->clear();

    m_genes_tableview->clearSelection();
    m_genes_tableview->clearFocus();

    getModel()->clearGenes();

    m_colorList->setCurrentColor(Globals::DEFAULT_COLOR_GENE);
}

void GenesWidget::slotShowAllSelected()
{
    slotSetVisibilityForSelectedRows(true);
}

void GenesWidget::slotHideAllSelected()
{
    slotSetVisibilityForSelectedRows(false);
}

void GenesWidget::slotSetVisibilityForSelectedRows(bool visible)
{
    getModel()->setGeneVisibility(m_genes_tableview->geneTableItemSelection(), visible);
    m_genes_tableview->update();
}

void GenesWidget::slotSetColorAllSelected(const QColor &color)
{
    getModel()->setGeneColor(m_genes_tableview->geneTableItemSelection(), color);
    m_genes_tableview->update();
}

void GenesWidget::slotLoadModel(const DataProxy::GeneList &geneList)
{
    getModel()->loadGenes(geneList);
}

GeneFeatureItemModel *GenesWidget::getModel()
{
    GeneFeatureItemModel *geneModel =
            qobject_cast<GeneFeatureItemModel*>(getProxyModel()->sourceModel());
    Q_ASSERT(geneModel);
    return geneModel;
}

QSortFilterProxyModel *GenesWidget::getProxyModel()
{
    QSortFilterProxyModel *proxyModel =
            qobject_cast<QSortFilterProxyModel*>(m_genes_tableview->model());
    Q_ASSERT(proxyModel);
    return proxyModel;
}

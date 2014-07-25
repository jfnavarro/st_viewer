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

#include "viewTables/GenesTableView.h"

#include "model/GeneFeatureItemModel.h"

#include "color/ColorListEditor.h"
#include "color/ColorPalette.h"

GenesWidget::GenesWidget(QWidget *parent) :
    QWidget(parent),
    //m_selectionMenu(nullptr),
    m_selectionAllButton(nullptr),
    m_selectionClearAllButton(nullptr),
    //m_actionMenu(nullptr),
    m_lineEdit(nullptr),
    m_genes_tableview(nullptr),
    m_colorList(nullptr),
    m_showSelectedButton(nullptr),
    m_hideSelectedButton(nullptr)
{
    QVBoxLayout *genesLayout = new QVBoxLayout();
    QHBoxLayout *geneListLayout = new QHBoxLayout();

    //create genes table
    m_genes_tableview = new GenesTableView(this);

    //create selections menu

    m_selectionAllButton = new QPushButton(this);
    m_selectionAllButton->setText(tr("Select All"));
    connect(m_selectionAllButton, SIGNAL(clicked(bool)), m_genes_tableview, SLOT(selectAll()));

    m_selectionClearAllButton = new QPushButton(this);
    m_selectionClearAllButton->setText(tr("Deselect All"));
    connect(m_selectionClearAllButton, SIGNAL(clicked(bool)), m_genes_tableview, SLOT(clearSelection()));

    geneListLayout->addWidget(m_selectionAllButton);
    geneListLayout->addWidget(m_selectionClearAllButton);

    //m_selectionMenu = new QPushButton(this);
    //QMenu *selectionsMenu = new QMenu();
    //m_selectionMenu->setMenu(selectionsMenu);
    //m_selectionMenu->setToolTip(tr("Selection options"));
    //m_selectionMenu->setText(tr("Selection"));
    //m_selectionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/checkall.png")),
    //                                   tr("Select all rows"), m_genes_tableview, SLOT(selectAll()));
    //m_selectionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/checkall.png")),
    //                                   tr("Deselect all rows"), m_genes_tableview, SLOT(clearSelection()));
    //geneListLayout->addWidget(m_selectionMenu);


    //create actions menu
    m_showSelectedButton = new QPushButton(this);
    m_showSelectedButton->setToolTip(tr("Show selected genes"));
    m_showSelectedButton->setIcon(QIcon(QStringLiteral(":/images/visible.png")));
    connect(m_showSelectedButton, SIGNAL(clicked(bool)), this, SLOT(slotShowAllSelected()));

    m_hideSelectedButton = new QPushButton(this);
    m_hideSelectedButton->setToolTip(tr("Hide selected genes"));
    m_hideSelectedButton->setIcon(QIcon(QStringLiteral(":/images/novisible.png")));
    connect(m_hideSelectedButton, SIGNAL(clicked(bool)), this, SLOT(slotHideAllSelected()));

    m_colorList = new ColorListEditor(this);
    m_colorList->setToolTip(tr("Set color of selected genes:"));
    connect(m_colorList, static_cast< void (QComboBox::*)(int) >(&QComboBox::activated),
            [=]() { slotSetColorAllSelected(m_colorList->color());});

    geneListLayout->addWidget(m_showSelectedButton);
    geneListLayout->addWidget(m_hideSelectedButton);
    geneListLayout->addWidget(m_colorList);

    //m_actionMenu = new QPushButton(this);
    //QMenu *actionsMenu = new QMenu();
    //m_actionMenu->setMenu(actionsMenu);
    //m_actionMenu->setToolTip(tr("Action on selected rows"));
    //m_actionMenu->setText(tr("Action"));
    // add actions to act on selected rows
    //QAction *showAllAction = m_actionMenu->menu()->addAction(
    //            QIcon(QStringLiteral(":/images/visible.png")), tr("Show selected genes"));
    //QAction *hideAllAction  = m_actionMenu->menu()->addAction(
    //            QIcon(QStringLiteral(":/images/novisible.png")), tr("Hide selected genes"));
    //m_actionMenu->menu()->addSeparator();
    //color action
    //QWidgetAction *widgetAction = new QWidgetAction(this);
    //m_colorList = new ColorListEditor();
    //widgetAction->setDefaultWidget(m_colorList);
    //m_actionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/edit_color.png")),
    //                                tr("Set color of selected genes:"));
    //TODO have the color combobox in the same line as the text and/or make it less width and centered
    //m_actionMenu->menu()->addAction(widgetAction);
    //show combobox when menu opens
    //connect(widgetAction, &QAction::triggered, [=]{ m_colorList->show(); });
    // QComboBox::activated is overloaded so we need a static_cast<>
    //connect(m_colorList, static_cast< void (QComboBox::*)(int) >(&QComboBox::activated),
   //         [=]() { slotSetColorAllSelected(m_colorList->color());});

    //adds the menu to the layout
    //geneListLayout->addWidget(m_actionMenu);

    //add separation and stretch in between the search box
    geneListLayout->addSpacing(5);
    geneListLayout->addStretch(5);

    //create line edit search
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setFixedSize(200, 20);
    m_lineEdit->setToolTip(tr("Search by gene name..."));
    geneListLayout->addWidget(m_lineEdit);
    geneListLayout->setAlignment(m_lineEdit, Qt::AlignRight);

    //add actions menu to main layout
    genesLayout->addLayout(geneListLayout);

    //add table to main layout
    genesLayout->addWidget(m_genes_tableview);

    //set main layout
    setLayout(genesLayout);

    //connections
    //connect(showAllAction, SIGNAL(triggered(bool)), this, SLOT(slotShowAllSelected()));
    //connect(hideAllAction, SIGNAL(triggered(bool)), this, SLOT(slotHideAllSelected()));
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
    //m_selectionMenu->deleteLater();
    //m_selectionMenu = nullptr;

    m_selectionAllButton->deleteLater();
    m_selectionAllButton = nullptr;

    m_selectionClearAllButton->deleteLater();
    m_selectionClearAllButton = nullptr;

    //m_actionMenu->deleteLater();
    //m_actionMenu = nullptr;

    m_lineEdit->deleteLater();
    m_lineEdit = nullptr;

    m_genes_tableview->deleteLater();
    m_genes_tableview = nullptr;

    m_colorList->deleteLater();
    m_colorList = nullptr;

    m_showSelectedButton->deleteLater();
    m_showSelectedButton = nullptr;

    m_hideSelectedButton->deleteLater();
    m_hideSelectedButton = nullptr;
}

void GenesWidget::clear()
{
    m_lineEdit->clearFocus();
    m_lineEdit->clear();

    m_colorList->setColor(Globals::DEFAULT_COLOR_GENE);

    m_genes_tableview->clearSelection();
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
    m_genes_tableview->repaint();
}

void GenesWidget::slotSetColorAllSelected(const QColor &color)
{
    getModel()->setGeneColor(m_genes_tableview->geneTableItemSelection(), color);
    m_genes_tableview->repaint();
}

void GenesWidget::slotLoadModel(DataProxy::GeneList &geneList)
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

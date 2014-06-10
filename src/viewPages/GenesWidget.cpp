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
#include <qtcolorpicker.h>
#include <QSortFilterProxyModel>

#include "viewTables/GenesTableView.h"

#include "model/GeneFeatureItemModel.h"

#include "color/ColorPalette.h"

GenesWidget::GenesWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *genesLayout = new QVBoxLayout();
    QHBoxLayout *geneListLayout = new QHBoxLayout();

    //create genes table
    m_genes_tableview = new GenesTableView();
    m_genes_tableview->setMinimumSize(QSize(100, 0));

    //create selections menu
    m_selectionMenu = new QPushButton(this);
    QMenu *selectionsMenu = new QMenu(m_selectionMenu);
    m_selectionMenu->setToolTip(tr("Selection options"));
    m_selectionMenu->setText(tr("Selection"));
    m_selectionMenu->setMenu(selectionsMenu);
    m_selectionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                                       tr("Select all rows"), m_genes_tableview, SLOT(selectAll()));
    m_selectionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                                       tr("Deselect all rows"), m_genes_tableview, SLOT(clearSelection()));
    geneListLayout->addWidget(m_selectionMenu);

    //create actions menu
    m_actionMenu = new QPushButton(this);
    QMenu *actionsMenu = new QMenu();
    m_actionMenu->setMenu(actionsMenu);
    m_actionMenu->setToolTip(tr("Action on selected rows"));
    m_actionMenu->setText(tr("Action"));
    // add actions to act on selected rows
    QAction *showAllAction = m_actionMenu->menu()->addAction(
                QIcon(QStringLiteral(":/images/grid-icon-md.png")), tr("Show all selected rows"));
    QAction *hideAllAction  = m_actionMenu->menu()->addAction(
                QIcon(QStringLiteral(":/images/grid-icon-md.png")),tr("Hide all selected rows"));
    m_actionMenu->menu()->addSeparator();
    //create color picker and add actions/connections
    //TODO this seg faults (seems like QColorPicker can only be created after initialization is done)
    //m_colorPickerPopup = ColorPicker::createColorPickerPopup(QColor(), this);
    //QWidgetAction *widgetAction = new QWidgetAction(m_actionMenu);
    //widgetAction->setDefaultWidget(m_colorPickerPopup);
    //m_actionMenu->menu()->addAction(tr("Set color of selected:"));
    //m_actionMenu->menu()->addAction(widgetAction);
    geneListLayout->addWidget(m_actionMenu);

    //create line edit search
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setFixedSize(200, 25);
    geneListLayout->addWidget(m_lineEdit);

    //add actions menu to main layout
    genesLayout->addLayout(geneListLayout);

    //add table to main layout
    genesLayout->addWidget(m_genes_tableview);

    //set main layout
    setLayout(genesLayout);

    //connections
    connect(showAllAction, SIGNAL(triggered(bool)), this, SLOT(slotShowAllSelected()));
    connect(hideAllAction, SIGNAL(triggered(bool)), this, SLOT(slotHideAllSelected()));
    connect(m_lineEdit, SIGNAL(textChanged(QString)), m_genes_tableview,
            SLOT(setGeneNameFilter(QString)));
    //connect(m_colorPickerPopup, SIGNAL(selected(const QColor &)), this,
    //        SLOT(slotSetColorAllSelected(const QColor &)));
    connect(getModel(), SIGNAL(signalSelectionChanged(DataProxy::GeneList)),
            this,
            SIGNAL(signalSelectionChanged(DataProxy::GeneList)));
    connect(getModel(), SIGNAL(signalColorChanged(DataProxy::GeneList)),
            this,
            SIGNAL(signalColorChanged(DataProxy::GeneList)));
}

GenesWidget::~GenesWidget()
{

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
}

void GenesWidget::slotSetColorAllSelected(const QColor &color)
{
    getModel()->setGeneColor(m_genes_tableview->geneTableItemSelection(), color);
}

void GenesWidget::slotLoadModel()
{
    getModel()->loadGenes();
}

void GenesWidget::slotClearModel()
{
    //nothing for now
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

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
    genesLayout->addWidget(m_genes_tableview);

    //create actions menu
    m_actionMenu = new QPushButton();
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
    m_colorPickerPopup = ColorPicker::createColorPickerPopup(QColor(), parent);
    QWidgetAction *widgetAction = new QWidgetAction(m_actionMenu);
    widgetAction->setDefaultWidget(m_colorPickerPopup);
    m_actionMenu->menu()->addAction(tr("Set color of selected:"));
    m_actionMenu->menu()->addAction(widgetAction);
    geneListLayout->addWidget(m_actionMenu);

    //create selections menu
    m_selectionMenu = new QPushButton();
    QMenu *selectionsMenu = new QMenu(m_selectionMenu);
    m_selectionMenu->setMenu(selectionsMenu);
    m_selectionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                                       tr("Select all rows"), m_genes_tableview, SLOT(selectAll()));
    m_selectionMenu->menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                                       tr("Deselect all rows"), m_genes_tableview, SLOT(clearSelection()));
    geneListLayout->addWidget(m_selectionMenu);

    //create line edit search
    m_lineEdit = new QLineEdit();
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setMinimumSize(QSize(50, 0));
    geneListLayout->addWidget(m_selectionMenu);

    //add actions menu to layout
    genesLayout->addLayout(geneListLayout);

    //set main layout
    setLayout(genesLayout);

    //connections
    connect(showAllAction, SIGNAL(triggered(bool)), this, SLOT(slotShowAllSelected()));
    connect(hideAllAction, SIGNAL(triggered(bool)), this, SLOT(slowHideAllSelected()));
    connect(m_lineEdit, SIGNAL(textChanged(QString)), m_genes_tableview,
            SLOT(setGeneNameFilter(QString)));
    connect(m_colorPickerPopup, SIGNAL(selected(const QColor &)), this,
            SLOT(slotSetColorAllSelected(const QColor &)));
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

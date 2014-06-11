/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneFeatureItemModel.h"

#include "viewTables/BooleanItemDelegate.h"
#include "viewTables/GeneViewDelegate.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent),
      m_geneModel(nullptr),
      m_sortGenesProxyModel(nullptr)
{
    // model view for genes list selector
    BooleanItemDelegate *booleanItemDelegate = new BooleanItemDelegate(this);
    GeneViewDelegate *geneViewDelegate = new GeneViewDelegate(this);

    m_geneModel = new GeneFeatureItemModel(this);
    
    m_sortGenesProxyModel = new SortGenesProxyModel(this);
    m_sortGenesProxyModel->setSourceModel(m_geneModel);
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterRegExp(QRegExp("(ambiguous)*|(^[0-9])*",
                                            Qt::CaseInsensitive)); //I do not want to show ambiguous genes or numbers
    setModel(m_sortGenesProxyModel);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    setItemDelegateForColumn(GeneFeatureItemModel::Show, booleanItemDelegate);
    setItemDelegateForColumn(GeneFeatureItemModel::Color, geneViewDelegate);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::AllEditTriggers);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    resizeColumnsToContents();
    resizeRowsToContents();

    model()->submit(); //support for caching (speed up)

    verticalHeader()->hide();
}

GenesTableView::~GenesTableView()
{

}

QItemSelection GenesTableView::geneTableItemSelection() const
{
    auto selected = selectionModel()->selection();
    auto mapped_selected = m_sortGenesProxyModel->mapSelectionToSource(selected);
    return mapped_selected;
}

void GenesTableView::setGeneNameFilter(QString str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}

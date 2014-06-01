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
      m_geneModel(nullptr)
{
    // model view for genes list selector
    BooleanItemDelegate *booleanItemDelegate = new BooleanItemDelegate(this);
    GeneViewDelegate *geneViewDelegate = new GeneViewDelegate(this);
    m_geneModel = new GeneFeatureItemModel(this);
    
    SortGenesProxyModel *sortProxyModel = new SortGenesProxyModel(this);
    sortProxyModel->setSourceModel(m_geneModel);
    sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterRegExp(QRegExp("(ambiguous)*|(^[0-9])*",
                                            Qt::CaseInsensitive)); //I do not want to show ambiguous genes or numbers
    setModel(sortProxyModel);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    setItemDelegateForColumn(GeneFeatureItemModel::Show, booleanItemDelegate);
    setItemDelegateForColumn(GeneFeatureItemModel::Color, geneViewDelegate);

    resizeColumnsToContents();
    resizeRowsToContents();

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    verticalHeader()->hide();

    model()->submit(); //support for caching (speed up)
}

GenesTableView::~GenesTableView()
{

}

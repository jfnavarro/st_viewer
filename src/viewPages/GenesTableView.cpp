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

#include "viewPages/BooleanItemDelegate.h"
#include "viewPages/GeneViewDelegate.h"

GenesTableView::GenesTableView(QWidget *parent)
    : GeneNamesTableView(parent)
{
    // model view for genes list selector
    BooleanItemDelegate* booleanItemDelegate = new BooleanItemDelegate(this);
    GeneViewDelegate* geneViewDelegate = new GeneViewDelegate(this);

    m_sortGenesProxyModel->setFilterRegExp(QRegExp("(ambiguous)*|(^[0-9])*",
                                            Qt::CaseInsensitive)); //I do not want to show ambiguous genes or numbers

    horizontalHeader()->setSortIndicatorShown(true);

    setItemDelegateForColumn(GeneFeatureItemModel::Show, booleanItemDelegate);
    setItemDelegateForColumn(GeneFeatureItemModel::Color, geneViewDelegate);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::AllEditTriggers);


    verticalHeader()->hide();
}

GenesTableView::~GenesTableView()
{

}

void GenesTableView::setGeneFeatureItemModel(GeneFeatureItemModel *geneModel)
{
    Q_ASSERT(geneModel);
    m_sortGenesProxyModel->setSourceModel(geneModel);
    setModel(m_sortGenesProxyModel);
    sortByColumn(0, Qt::AscendingOrder);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    resizeColumnsToContents();
    resizeRowsToContents();

    // Is submit() needed? I don't know what it is meant for /Erik Sjolund
    model()->submit(); //support for caching (speed up)
}


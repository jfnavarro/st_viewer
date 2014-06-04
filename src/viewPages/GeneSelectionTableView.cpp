/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneSelectionTableView.h"

#include <QHeaderView>
#include "model/SortGenesProxyModel.h"
#include "model/GeneSelectionItemModel.h"
#include "viewPages/GeneNamesTableView.h"

GeneSelectionTableView::GeneSelectionTableView(QWidget *parent)
    : GeneNamesTableView(parent)
{
    //model view gene selections table

    horizontalHeader()->setSortIndicatorShown(true);

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);

    setShowGrid(true);

    verticalHeader()->hide();
}

GeneSelectionTableView::~GeneSelectionTableView()
{

}

void GeneSelectionTableView::setGeneSelectionItemModel(GeneSelectionItemModel *geneSelectionItemModel)
{
    Q_ASSERT(geneSelectionItemModel);
    Q_ASSERT(m_sortGenesProxyModel);
    m_sortGenesProxyModel->setSourceModel(geneSelectionItemModel);
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

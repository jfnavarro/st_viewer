/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneSelectionTableView.h"

#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneSelectionItemModel.h"

GeneSelectionTableView::GeneSelectionTableView(QWidget *parent)
    : QTableView(parent),
      m_geneSelectionModel(nullptr),
      m_sortGenesProxyModel(nullptr)
{
    m_geneSelectionModel = new GeneSelectionItemModel(this);

    m_sortGenesProxyModel = new SortGenesProxyModel(this);
    m_sortGenesProxyModel->setSourceModel(m_geneSelectionModel);
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortGenesProxyModel);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);

    setShowGrid(true);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    resizeColumnsToContents();
    resizeRowsToContents();

    verticalHeader()->hide();

    model()->submit(); //support for caching (speed up)
}

GeneSelectionTableView::~GeneSelectionTableView()
{

}

QItemSelection GeneSelectionTableView::geneTableItemSelection() const
{
    auto selected = selectionModel()->selection();
    auto mapped_selected = m_sortGenesProxyModel->mapSelectionToSource(selected);
    return mapped_selected;
}

void GeneSelectionTableView::setGeneNameFilter(QString str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}

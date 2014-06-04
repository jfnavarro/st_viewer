/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneNamesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "model/SortGenesProxyModel.h"

GeneNamesTableView::GeneNamesTableView(QWidget *parent)
    : QTableView(parent)
{
    m_sortGenesProxyModel = new SortGenesProxyModel(this);
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortingEnabled(true);
    horizontalHeader()->setSortIndicatorShown(true);
}

GeneNamesTableView::~GeneNamesTableView()
{

}

QItemSelection GeneNamesTableView::geneTableItemSelection() const
{
    auto selected = selectionModel()->selection();
    auto mapped_selected = m_sortGenesProxyModel->mapSelectionToSource(selected);
    return mapped_selected;
}

void GeneNamesTableView::setGeneNameFilter(QString str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}

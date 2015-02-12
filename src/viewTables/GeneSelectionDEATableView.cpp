/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneSelectionDEATableView.h"

#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneSelectionDEAItemModel.h"

GeneSelectionDEATableView::GeneSelectionDEATableView(QWidget *parent)
    : QTableView(parent),
      m_geneSelectionDEAModel(nullptr),
      m_sortGenesProxyModel(nullptr)
{
    // model
    m_geneSelectionDEAModel = new GeneSelectionDEAItemModel(this);

    // the sorting model
    m_sortGenesProxyModel = new SortGenesProxyModel(this);
    m_sortGenesProxyModel->setSourceModel(m_geneSelectionDEAModel);
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortGenesProxyModel);

    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(GeneSelectionDEAItemModel::Name, Qt::AscendingOrder);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);

    horizontalHeader()->setSectionResizeMode(GeneSelectionDEAItemModel::Name,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(GeneSelectionDEAItemModel::HitsA,
                                             QHeaderView::Fixed);
    horizontalHeader()->setSectionResizeMode(GeneSelectionDEAItemModel::TPMA,
                                             QHeaderView::Fixed);
    horizontalHeader()->setSectionResizeMode(GeneSelectionDEAItemModel::HitsB,
                                             QHeaderView::Fixed);
    horizontalHeader()->setSectionResizeMode(GeneSelectionDEAItemModel::TPMB,
                                             QHeaderView::Fixed);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); //support for caching (speed up)
}

GeneSelectionDEATableView::~GeneSelectionDEATableView()
{

}

QItemSelection GeneSelectionDEATableView::geneTableItemSelection() const
{
    const auto selected = selectionModel()->selection();
    return m_sortGenesProxyModel->mapSelectionToSource(selected);
}

void GeneSelectionDEATableView::setGeneNameFilter(QString str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}


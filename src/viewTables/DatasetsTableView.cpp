/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DatasetsTableView.h"

#include <QModelIndex>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/DatasetItemModel.h"

DatasetsTableView::DatasetsTableView(QWidget *parent)
    : QTableView(parent),
      m_datasetModel(nullptr)
{
    // the model
    m_datasetModel = new DatasetItemModel(this);

    // the sorting model
    m_sortDatasetsProxyModel = new QSortFilterProxyModel(this);
    m_sortDatasetsProxyModel->setSourceModel(m_datasetModel);
    m_sortDatasetsProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortDatasetsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortDatasetsProxyModel);

    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(DatasetItemModel::Name, Qt::AscendingOrder);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    resizeColumnsToContents();
    resizeRowsToContents();

    horizontalHeader()->setSortIndicatorShown(true);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Name, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Tissue, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Specie, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Barcodes, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Genes, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::UBarcodes, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::UGenes, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Created, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::LastModified, QHeaderView::Stretch);
    verticalHeader()->hide();

    model()->submit(); //support for caching (speed up)
}

DatasetsTableView::~DatasetsTableView()
{
    m_datasetModel->deleteLater();
    m_datasetModel = nullptr;
}

QItemSelection DatasetsTableView::datasetsTableItemSelection() const
{
    const auto selected = selectionModel()->selection();
    return m_sortDatasetsProxyModel->mapSelectionToSource(selected);
}

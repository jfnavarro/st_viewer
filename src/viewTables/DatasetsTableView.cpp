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
    QSortFilterProxyModel *sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setSourceModel(m_datasetModel);
    sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(sortProxyModel);
    setSortingEnabled(true);

    sortByColumn(0, Qt::AscendingOrder);

    horizontalHeader()->setSortIndicatorShown(true);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);

    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);

    resizeColumnsToContents();
    resizeRowsToContents();

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    model()->submit(); //support for caching (speed up)
    verticalHeader()->hide();

    //notify the model I have selected a dataset
    connect(this, SIGNAL(doubleClicked(QModelIndex)),
            m_datasetModel , SLOT(datasetSelected(QModelIndex)));
}

DatasetsTableView::~DatasetsTableView()
{

}

#include "DatasetsTableView.h"

#include <QModelIndex>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QPalette>
#include <QDebug>
#include "model/DatasetItemModel.h"

DatasetsTableView::DatasetsTableView(QWidget *parent)
    : QTableView(parent)
    , m_datasetModel(nullptr)
{
    // the data model
    m_datasetModel.reset(new DatasetItemModel(this));

    // the sorting model
    m_sortDatasetsProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortDatasetsProxyModel->setSourceModel(m_datasetModel.data());
    m_sortDatasetsProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortDatasetsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortDatasetsProxyModel.data());

    // settings of the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(DatasetItemModel::Name, Qt::AscendingOrder);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::SelectedClicked);
    setSelectionMode(QAbstractItemView::MultiSelection);

    resizeColumnsToContents();
    resizeRowsToContents();

    horizontalHeader()->setSortIndicatorShown(true);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Tissue, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(DatasetItemModel::Species, QHeaderView::Stretch);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)
}

DatasetsTableView::~DatasetsTableView()
{
}

QItemSelection DatasetsTableView::datasetsTableItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortDatasetsProxyModel->mapSelectionToSource(selected);
}

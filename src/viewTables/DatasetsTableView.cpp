#include "DatasetsTableView.h"

#include <QModelIndex>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QDebug>
#include <QClipboard>
#include <QApplication>

#include "model/DatasetItemModel.h"

DatasetsTableView::DatasetsTableView(QWidget *parent)
    : QTableView(parent)
    , m_sortDatasetsProxyModel(nullptr)
{
    // the data model
    DatasetItemModel *data_model = new DatasetItemModel(this);

    // the sorting model
    m_sortDatasetsProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortDatasetsProxyModel->setSourceModel(data_model);
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

    // allow to copy the dataset name
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DatasetsTableView::customContextMenuRequested,
            this, &DatasetsTableView::customMenuRequested);

}

DatasetsTableView::~DatasetsTableView()
{
}

QItemSelection DatasetsTableView::datasetsTableItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortDatasetsProxyModel->mapSelectionToSource(selected);
}

void DatasetsTableView::customMenuRequested(const QPoint &pos)
{
    const QModelIndex index = indexAt(pos);
    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        menu->addAction(new QAction(tr("Copy name"), this));
        menu->addAction(new QAction(tr("Open"), this));
        menu->addAction(new QAction(tr("Edit"), this));
        menu->addAction(new QAction(tr("Delete"), this));
        QAction *action = menu->exec(viewport()->mapToGlobal(pos));
        if (action != nullptr) {
            const QString action_text = action->text();
            if (action_text == tr("Copy name")) {
                const QModelIndex new_index = m_sortDatasetsProxyModel->index(index.row(), DatasetItemModel::Name);
                const QString dataset_name = m_sortDatasetsProxyModel->data(new_index, Qt::DisplayRole).toString();
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(dataset_name);
            } else if (action_text == tr("Open")) {
                emit signalDatasetOpen(index);
            } else if (action_text == tr("Edit")) {
                emit signalDatasetEdit(index);
            } else if (action_text == tr("Delete")) {
                emit signalDatasetDelete(index);
            }
        }
    }
}

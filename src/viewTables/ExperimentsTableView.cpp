#include "ExperimentsTableView.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/ExperimentsItemModel.h"

ExperimentsTableView::ExperimentsTableView(QWidget *parent)
    : QTableView(parent),
      m_experimentModel(nullptr)
{
    m_experimentModel = new ExperimentsItemModel(this);

    QSortFilterProxyModel *sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setSourceModel(m_experimentModel);
    sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    setModel(sortProxyModel);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    setSortingEnabled(true);
    horizontalHeader()->setSortIndicatorShown(true);
    sortByColumn(0, Qt::AscendingOrder);

    resizeColumnsToContents();
    resizeRowsToContents();

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);

    setShowGrid(true);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    verticalHeader()->hide();
    model()->submit(); //support for caching (speed up)
}

ExperimentsTableView::~ExperimentsTableView()
{

}

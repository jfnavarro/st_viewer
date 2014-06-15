#include "ExperimentsTableView.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/ExperimentsItemModel.h"

ExperimentsTableView::ExperimentsTableView(QWidget *parent)
    : QTableView(parent),
      m_experimentModel(nullptr)
{
    // model
    m_experimentModel = new ExperimentsItemModel(this);

    // sorting model
    QSortFilterProxyModel *sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setSourceModel(m_experimentModel);
    sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(sortProxyModel);
    setSortingEnabled(true);

    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    resizeColumnsToContents();
    resizeRowsToContents();

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::MultiSelection);

    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    verticalHeader()->hide();
    model()->submit(); //support for caching (speed up)
}

ExperimentsTableView::~ExperimentsTableView()
{

}

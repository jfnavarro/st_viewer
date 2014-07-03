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

    sortByColumn(ExperimentsItemModel::Name, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    resizeColumnsToContents();
    resizeRowsToContents();

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::MultiSelection);

    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);

    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Name, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Dataset, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Comment, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Type, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::NGenes, QHeaderView::Stretch);

    verticalHeader()->hide();
    model()->submit(); //support for caching (speed up)
}

ExperimentsTableView::~ExperimentsTableView()
{
    m_experimentModel->deleteLater();
    m_experimentModel = nullptr;
}

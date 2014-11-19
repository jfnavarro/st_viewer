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
    m_sortSelectionsProxyModel = new QSortFilterProxyModel(this);
    m_sortSelectionsProxyModel->setSourceModel(m_experimentModel);
    m_sortSelectionsProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortSelectionsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortSelectionsProxyModel);

    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(ExperimentsItemModel::Name, Qt::AscendingOrder);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::MultiSelection);

    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Name,
                                             QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Dataset,
                                             QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Comment,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::NGenes,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::Created,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ExperimentsItemModel::LastModified,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); //support for caching (speed up)
}

ExperimentsTableView::~ExperimentsTableView()
{
    m_experimentModel->deleteLater();
    m_experimentModel = nullptr;

    m_sortSelectionsProxyModel->deleteLater();
    m_sortSelectionsProxyModel = nullptr;
}

QItemSelection ExperimentsTableView::experimentTableItemSelection() const
{
    const auto selected = selectionModel()->selection();
    return m_sortSelectionsProxyModel->mapSelectionToSource(selected);
}

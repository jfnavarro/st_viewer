#include "UserSelectionTableView.h"
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include "model/UserSelectionsItemModel.h"

UserSelectionTableView::UserSelectionTableView(QWidget *parent)
    : QTableView(parent)
    , m_sortSelectionsProxyModel(nullptr)
{
    // model
    UserSelectionsItemModel *data_model = new UserSelectionsItemModel(this);

    // sorting model
    m_sortSelectionsProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortSelectionsProxyModel->setSourceModel(data_model);
    m_sortSelectionsProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortSelectionsProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortSelectionsProxyModel.data());

    // Configuration of the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(UserSelectionsItemModel::Name, Qt::AscendingOrder);
    // Selection behavior
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::MultiSelection);
    // Columns settings
    horizontalHeader()->setSectionResizeMode(UserSelectionsItemModel::Name,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(UserSelectionsItemModel::Dataset,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(UserSelectionsItemModel::NGenes,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(UserSelectionsItemModel::NSpots,
                                             QHeaderView::Stretch);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)
}

UserSelectionTableView::~UserSelectionTableView()
{
}

QItemSelection UserSelectionTableView::userSelecionTableItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortSelectionsProxyModel->mapSelectionToSource(selected);
}

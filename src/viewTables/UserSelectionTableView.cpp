#include "UserSelectionTableView.h"

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QClipboard>
#include <QApplication>
#include <QMenu>

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

    // allow to copy the dataset name
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &UserSelectionTableView::customContextMenuRequested,
            this, &UserSelectionTableView::customMenuRequested);
}

UserSelectionTableView::~UserSelectionTableView()
{
}

QItemSelection UserSelectionTableView::userSelecionTableItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortSelectionsProxyModel->mapSelectionToSource(selected);
}

void UserSelectionTableView::customMenuRequested(const QPoint &pos)
{
    const QModelIndex index = indexAt(pos);
    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        menu->addAction(new QAction(tr("Copy name"), this));
        menu->addAction(new QAction(tr("Export"), this));
        menu->addAction(new QAction(tr("Edit"), this));
        menu->addAction(new QAction(tr("Delete"), this));
        QAction *action = menu->exec(viewport()->mapToGlobal(pos));
        if (action != nullptr) {
            const QString action_text = action->text();
            if (action_text == tr("Copy name")) {
                const QModelIndex new_index = m_sortSelectionsProxyModel->index(index.row(), UserSelectionsItemModel::Name);
                const QString selection_name = m_sortSelectionsProxyModel->data(new_index, Qt::DisplayRole).toString();
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(selection_name);
            } else if (action_text == tr("Export")) {
                emit signalSelectionExport(index);
            } else if (action_text == tr("Edit")) {
                emit signalSelectionEdit(index);
            } else if (action_text == tr("Delete")) {
                emit signalSelectionDelete(index);
            }
        }
    }
}

#include "ClusterTableView.h"
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMenu>
#include <QApplication>
#include <QColorDialog>

#include "model/ClusterItemModel.h"

ClusterTableView::ClusterTableView(QWidget *parent)
    : QTableView(parent)
    , m_sortProxyModel(nullptr)
{
    // model
    ClusterItemModel *data_model = new ClusterItemModel(this);

    // sorting model
    m_sortProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortProxyModel->setSourceModel(data_model);
    m_sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // this is important because sort proxy will use the column 0 by default
    m_sortProxyModel->setFilterKeyColumn(ClusterItemModel::Name);
    m_sortProxyModel->setSortRole(Qt::UserRole);
    setModel(m_sortProxyModel.data());

    // settings for the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(ClusterItemModel::Name, Qt::AscendingOrder);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    horizontalHeader()->setSectionResizeMode(ClusterItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(ClusterItemModel::Color, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(ClusterItemModel::Color, 50);
    horizontalHeader()->setSectionResizeMode(ClusterItemModel::Count, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(ClusterItemModel::Count, 100);
    horizontalHeader()->setSectionResizeMode(ClusterItemModel::Show, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(ClusterItemModel::Show, 50);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)

    // allow to show extra functions on the table
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ClusterTableView::customContextMenuRequested,
            this, &ClusterTableView::customMenuRequested);
}

ClusterTableView::~ClusterTableView()
{
}

QItemSelection ClusterTableView::getItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortProxyModel->mapSelectionToSource(selected);
}

void ClusterTableView::setNameFilter(const QString &str)
{
    m_sortProxyModel->setFilterFixedString(str);
}

void ClusterTableView::customMenuRequested(const QPoint &pos)
{
    const QModelIndex index = indexAt(pos);
    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        menu->addAction(new QAction(tr("Copy name"), this));
        menu->addAction(new QAction(tr("Show/Hide"), this));
        menu->addAction(new QAction(tr("Change color"), this));
        QAction *selection = menu->exec(viewport()->mapToGlobal(pos));
        if (selection != nullptr) {
            const QModelIndex correct_index = m_sortProxyModel->mapToSource(index);
            const QString action_text = selection->text();
            if (action_text == tr("Copy name")) {
                const QModelIndex new_index = getModel()->index(correct_index.row(), ClusterItemModel::Name);
                const QString spot_name = getModel()->data(new_index, Qt::DisplayRole).toString();
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(spot_name);
            } else if (action_text == tr("Show/Hide")) {
                const QModelIndex new_index = getModel()->index(correct_index.row(), ClusterItemModel::Show);
                const bool selected = getModel()->data(new_index, Qt::CheckStateRole).toBool();
                getModel()->setVisible(QItemSelection(correct_index, correct_index), !selected);
                update();
                emit signalUpdated();
            } else if (action_text == tr("Change color")) {
                // launch color selector
                const QColor color = QColorDialog::getColor(Qt::red, this, tr("Spot color"));
                getModel()->setColor(QItemSelection(correct_index, correct_index), color);
                update();
                emit signalUpdated();
            }
        }
    }
}

ClusterItemModel *ClusterTableView::getModel()
{
    ClusterItemModel *model = qobject_cast<ClusterItemModel *>(getProxyModel()->sourceModel());
    Q_ASSERT(model);
    return model;
}

QSortFilterProxyModel *ClusterTableView::getProxyModel()
{
    QSortFilterProxyModel *proxyModel
            = qobject_cast<QSortFilterProxyModel *>(model());
    Q_ASSERT(proxyModel);
    return proxyModel;
}

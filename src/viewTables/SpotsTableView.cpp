#include "SpotsTableView.h"
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMenu>
#include <QApplication>
#include <QColorDialog>

#include "model/SpotItemModel.h"

SpotsTableView::SpotsTableView(QWidget *parent)
    : QTableView(parent)
    , m_sortProxyModel(nullptr)
{
    // model
    SpotItemModel *data_model = new SpotItemModel(this);

    // sorting model
    m_sortProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortProxyModel->setSourceModel(data_model);
    m_sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // this is important because sort proxy will use the column 0 by default
    m_sortProxyModel->setFilterKeyColumn(SpotItemModel::Name);
    setModel(m_sortProxyModel.data());

    // settings for the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(SpotItemModel::Name, Qt::AscendingOrder);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::MultiSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    horizontalHeader()->setSectionResizeMode(SpotItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(SpotItemModel::Color, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(SpotItemModel::Color, 50);
    horizontalHeader()->setSectionResizeMode(SpotItemModel::Count, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(SpotItemModel::Show, 100);
    horizontalHeader()->setSectionResizeMode(SpotItemModel::Show, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(SpotItemModel::Show, 50);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)

    // allow to show extra functions on the table
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &SpotsTableView::customContextMenuRequested,
            this, &SpotsTableView::customMenuRequested);
}

SpotsTableView::~SpotsTableView()
{
}

QItemSelection SpotsTableView::getItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortProxyModel->mapSelectionToSource(selected);
}

void SpotsTableView::setNameFilter(const QString &str)
{
    m_sortProxyModel->setFilterFixedString(str);
}

void SpotsTableView::customMenuRequested(const QPoint &pos)
{
    const QModelIndex index = indexAt(pos);
    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        menu->addAction(new QAction(tr("Copy spot"), this));
        menu->addAction(new QAction(tr("Select/Unselect"), this));
        menu->addAction(new QAction(tr("Change color"), this));
        QAction *selection = menu->exec(viewport()->mapToGlobal(pos));
        if (selection != nullptr) {
            const QModelIndex correct_index = m_sortProxyModel->mapToSource(index);
            const QString action_text = selection->text();
            if (action_text == tr("Copy spot")) {
                const QModelIndex new_index = getModel()->index(correct_index.row(), SpotItemModel::Name);
                const QString spot_name = getModel()->data(new_index, Qt::DisplayRole).toString();
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(spot_name);
            } else if (action_text == tr("Select/Unselect")) {
                const QModelIndex new_index = getModel()->index(correct_index.row(), SpotItemModel::Show);
                const bool selected = getModel()->data(new_index, Qt::CheckStateRole).toBool();
                getModel()->setVisibility(QItemSelection(correct_index, correct_index), !selected);
                update();
                emit signalSpotsUpdated();
            } else if (action_text == tr("Change color")) {
                // launch color selector
                const QColor color = QColorDialog::getColor(Qt::red, this, tr("Gene color"));
                getModel()->setColor(QItemSelection(correct_index, correct_index), color);
                update();
                emit signalSpotsUpdated();
            }
        }
    }
}

SpotItemModel *SpotsTableView::getModel()
{
    SpotItemModel *spotModel = qobject_cast<SpotItemModel *>(getProxyModel()->sourceModel());
    Q_ASSERT(spotModel);
    return spotModel;
}

QSortFilterProxyModel *SpotsTableView::getProxyModel()
{
    QSortFilterProxyModel *proxyModel
            = qobject_cast<QSortFilterProxyModel *>(model());
    Q_ASSERT(proxyModel);
    return proxyModel;
}

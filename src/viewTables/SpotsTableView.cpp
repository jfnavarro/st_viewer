#include "SpotsTableView.h"
#include <QHeaderView>
#include <QSortFilterProxyModel>
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

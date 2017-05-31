#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include "model/GeneItemModel.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent)
    , m_model(nullptr)
    , m_sortProxyModel(nullptr)
{
    // model
    m_model.reset(new GeneItemModel(this));

    // sorting model
    m_sortProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortProxyModel->setSourceModel(m_model.data());
    m_sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // this is important because sort proxy will use the column 0 by default
    m_sortProxyModel->setFilterKeyColumn(GeneItemModel::Name);
    setModel(m_sortProxyModel.data());

    // settings for the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(GeneItemModel::Name, Qt::AscendingOrder);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::MultiSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    horizontalHeader()->setSectionResizeMode(GeneItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(GeneItemModel::Color, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneItemModel::Color, 50);
    horizontalHeader()->setSectionResizeMode(GeneItemModel::Show, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneItemModel::Show, 50);
    horizontalHeader()->setSectionResizeMode(GeneItemModel::CutOff, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneItemModel::Show, 50);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)
}

GenesTableView::~GenesTableView()
{
}

QItemSelection GenesTableView::getItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortProxyModel->mapSelectionToSource(selected);
}

void GenesTableView::setNameFilter(const QString &str)
{
    m_sortProxyModel->setFilterFixedString(str);
}

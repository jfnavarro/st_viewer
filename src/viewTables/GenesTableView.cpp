#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneItemModel.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent)
    , m_geneModel(nullptr)
    , m_sortGenesProxyModel(nullptr)
{
    // model
    m_geneModel.reset(new GeneItemModel(this));

    // sorting model
    m_sortGenesProxyModel.reset(new SortGenesProxyModel(this));
    m_sortGenesProxyModel->setSourceModel(m_geneModel.data());
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // this is important because sort proxy will use the column 0 by default
    m_sortGenesProxyModel->setFilterKeyColumn(GeneItemModel::Name);
    setModel(m_sortGenesProxyModel.data());

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

QItemSelection GenesTableView::geneTableItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortGenesProxyModel->mapSelectionToSource(selected);
}

void GenesTableView::setGeneNameFilter(const QString &str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}

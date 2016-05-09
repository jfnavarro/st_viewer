#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneFeatureItemModel.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent)
    , m_geneModel(nullptr)
    , m_sortGenesProxyModel(nullptr)
{
    // model
    m_geneModel.reset(new GeneFeatureItemModel(this));

    // sorting model
    m_sortGenesProxyModel.reset(new SortGenesProxyModel(this));
    m_sortGenesProxyModel->setSourceModel(m_geneModel.data());
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // this is important because sort proxy will use the column 0 by default
    m_sortGenesProxyModel->setFilterKeyColumn(GeneFeatureItemModel::Name);
    setModel(m_sortGenesProxyModel.data());

    // settings for the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(GeneFeatureItemModel::Name, Qt::AscendingOrder);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::MultiSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    horizontalHeader()->setSectionResizeMode(GeneFeatureItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(GeneFeatureItemModel::Color, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneFeatureItemModel::Color, 50);
    horizontalHeader()->setSectionResizeMode(GeneFeatureItemModel::Show, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneFeatureItemModel::Show, 50);
    horizontalHeader()->setSectionResizeMode(GeneFeatureItemModel::CutOff, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneFeatureItemModel::Show, 50);
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

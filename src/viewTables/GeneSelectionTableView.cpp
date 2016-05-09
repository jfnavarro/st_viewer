#include "GeneSelectionTableView.h"

#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneSelectionItemModel.h"

GeneSelectionTableView::GeneSelectionTableView(QWidget *parent)
    : QTableView(parent)
    , m_geneSelectionModel(nullptr)
    , m_sortGenesProxyModel(nullptr)
{
    // model
    m_geneSelectionModel.reset(new GeneSelectionItemModel(this));

    // the sorting model
    m_sortGenesProxyModel.reset(new SortGenesProxyModel(this));
    m_sortGenesProxyModel->setSourceModel(m_geneSelectionModel.data());
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    setModel(m_sortGenesProxyModel.data());

    // settings of the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(GeneSelectionItemModel::Name, Qt::AscendingOrder);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);

    horizontalHeader()->setSectionResizeMode(GeneSelectionItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(GeneSelectionItemModel::Count, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneSelectionItemModel::Count, 75);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)
}

GeneSelectionTableView::~GeneSelectionTableView()
{
}

QItemSelection GeneSelectionTableView::geneTableItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortGenesProxyModel->mapSelectionToSource(selected);
}

void GeneSelectionTableView::setGeneNameFilter(const QString &str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}

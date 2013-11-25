#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "view/controllers/GeneFeatureItemModel.h"
#include "view/delegates/BooleanItemDelegate.h"
#include "view/delegates/GeneViewDelegate.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent)
{
    // model view for genes list selector
    BooleanItemDelegate* booleanItemDelegate = new BooleanItemDelegate(this);
    GeneViewDelegate* geneViewDelegate = new GeneViewDelegate(this);
    geneModel = new GeneFeatureItemModel(this);
    QSortFilterProxyModel* sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setSourceModel(geneModel);
    sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterRegExp(QRegExp("(ambiguous)*|(^[0-9])*", Qt::CaseInsensitive)); //I do not want to show ambiguous genes or numbers

    setModel(sortProxyModel);
    setItemDelegateForColumn(GeneFeatureItemModel::Show, booleanItemDelegate);
    setItemDelegateForColumn(GeneFeatureItemModel::Color, geneViewDelegate);
    resizeColumnsToContents();
    horizontalHeader()->sectionResizeMode(QHeaderView::Interactive);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::SingleSelection);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    verticalHeader()->hide();
    model()->submit(); //support for caching (speed up)
}

GenesTableView::~GenesTableView()
{

}

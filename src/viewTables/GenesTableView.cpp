/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "model/SortGenesProxyModel.h"
#include "model/GeneFeatureItemModel.h"

#include "viewTables/BooleanItemDelegate.h"
#include "viewTables/ColorItemDelegate.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent),
      m_geneModel(nullptr),
      m_sortGenesProxyModel(nullptr)
{
    // item delegates
    BooleanItemDelegate *booleanItemDelegate = new BooleanItemDelegate(this);
    ColorItemDelegate *colorItemDelegate = new ColorItemDelegate(this);

    // model
    m_geneModel = new GeneFeatureItemModel(this);
    
    // sorting model
    m_sortGenesProxyModel = new SortGenesProxyModel(this);
    m_sortGenesProxyModel->setSourceModel(m_geneModel);
    m_sortGenesProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_sortGenesProxyModel->setFilterRegExp(QRegExp("(ambiguous)*|(^[0-9])*",
                                            Qt::CaseInsensitive)); //not show ambiguous genes or numbers
    setModel(m_sortGenesProxyModel);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSortIndicatorShown(true);

    setItemDelegateForColumn(GeneFeatureItemModel::Show, booleanItemDelegate);
    setItemDelegateForColumn(GeneFeatureItemModel::Color, colorItemDelegate);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::MultiSelection);
    setEditTriggers(QAbstractItemView::AllEditTriggers);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    resizeColumnsToContents();
    resizeRowsToContents();

    model()->submit(); //support for caching (speed up)

    verticalHeader()->hide();
}

GenesTableView::~GenesTableView()
{

}

void GenesTableView::createColorComboBoxes()
{
    int rows =  m_geneModel->rowCount();
    for (int i = 0; i < rows ; ++i) {
       auto index = m_geneModel->index(i, GeneFeatureItemModel::Color);
       Q_ASSERT(index.isValid());
       openPersistentEditor(m_sortGenesProxyModel->mapFromSource(index));
    }
}

void GenesTableView::reset()
{
    createColorComboBoxes();
}

QItemSelection GenesTableView::geneTableItemSelection() const
{
    const auto selected = selectionModel()->selection();
    return m_sortGenesProxyModel->mapSelectionToSource(selected);
}

void GenesTableView::setGeneNameFilter(QString str)
{
    m_sortGenesProxyModel->setFilterFixedString(str);
}

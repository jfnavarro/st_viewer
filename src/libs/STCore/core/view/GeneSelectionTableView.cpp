/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneSelectionTableView.h"

#include <QHeaderView>

#include "model/GeneSelectionItemModel.h"

GeneSelectionTableView::GeneSelectionTableView(QWidget *parent)
    : QTableView(parent)
{
    //model view gene selections table
    geneSelectionModel = new GeneSelectionItemModel(this);
    setModel(geneSelectionModel);
    resizeColumnsToContents();
    horizontalHeader()->sectionResizeMode(QHeaderView::Interactive);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSortingEnabled(true);
    setShowGrid(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::NoSelection);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    //horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    verticalHeader()->hide();
    model()->submit(); //support for caching (speed up)

}

GeneSelectionTableView::~GeneSelectionTableView()
{

}

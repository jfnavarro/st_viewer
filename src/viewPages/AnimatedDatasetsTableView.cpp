/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AnimatedDatasetsTableView.h"

#include <QModelIndex>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "model/DatasetItemModel.h"
#include "viewPages/DatasetsViewItemDelegate.h"

AnimatedDatasetsTableView::AnimatedDatasetsTableView(QWidget *parent)
    : QTableView(parent),
      m_animation(this),
      m_datasetModel(0)
{
    // create animation
    m_animation.setAnimation(":images/loader.gif");

    // the model
    m_datasetModel = new DatasetItemModel(this);

    //TODO fix the sorting, it seg faults now..
    //QSortFilterProxyModel* sortProxyModel = new QSortFilterProxyModel(this);
    //sortProxyModel->setSourceModel(m_datasetModel);
    //sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    //sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    setModel(m_datasetModel);
    setSortingEnabled(true);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->setSortIndicatorShown(false);
    sortByColumn(0, Qt::AscendingOrder);

    //NOTE item delegate is not finished yet
    //ui->datasets_tableview->setItemDelegate(new DatasetsViewItemDelegate(this));

    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);

    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);

    resizeColumnsToContents();
    resizeRowsToContents();

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    model()->submit(); //support for caching (speed up)
    verticalHeader()->hide();

    //notify the model I have selected a dataset TODO should be a better way to do this
    connect(this, SIGNAL(doubleClicked(QModelIndex)),
            m_datasetModel , SLOT(datasetSelected(QModelIndex)));
}

AnimatedDatasetsTableView::~AnimatedDatasetsTableView()
{

}

void AnimatedDatasetsTableView::setWaiting(const bool waiting)
{
    m_animation.setEnabled(waiting);
    m_animation.setVisible(waiting);
}

void AnimatedDatasetsTableView::paintEvent(QPaintEvent *event)
{

    QTableView::paintEvent(event);

    m_animation.paintAnimation(event);
}

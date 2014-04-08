/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AnimatedDatasetsTableView.h"

#include <QModelIndex>
#include <QHeaderView>

#include "model/DatasetItemModel.h"
#include "viewPages/DatasetsViewItemDelegate.h"

AnimatedDatasetsTableView::AnimatedDatasetsTableView(QWidget *parent)
    : QTableView(parent),
      m_animation(this),
      m_datasetModel(0)
{
    // create animation
    m_animation.setAnimation(":images/loader.gif");
    //crearte model for the table view
    m_datasetModel = new DatasetItemModel(this);
    //create selection model for table view
    setModel(m_datasetModel);

    //NOTE item delegate is not finished yet
    //ui->datasets_tableview->setItemDelegate(new DatasetsViewItemDelegate(this));

    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setStretchLastSection(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    resizeColumnsToContents();
    resizeRowsToContents();
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->setSortIndicatorShown(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    m_datasetModel->submit(); //support for caching (speed up)
    verticalHeader()->hide();
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    m_datasetModel->sort(0, Qt::AscendingOrder);

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

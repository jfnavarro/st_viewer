/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SortGenesProxyModel.h"

#include <QString>
#include <QDebug>

#include "CompareGeneNames.h"
#include "GeneNamesModel.h"

#include "data/DataProxy.h"

SortGenesProxyModel::SortGenesProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

SortGenesProxyModel::~SortGenesProxyModel()
{
}

bool SortGenesProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QString leftName;
    QString rightName;

    GeneNamesModel *geneNameModel = qobject_cast<GeneNamesModel *>(sourceModel());
    Q_ASSERT(geneNameModel);
    if ((geneNameModel->geneName(left, &leftName)) 
        && (geneNameModel->geneName(right, &rightName))) {
        return geneNameLessThan(leftName, rightName, sortCaseSensitivity(), isSortLocaleAware());
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

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
enum SortCategory {NORMAL_GENE_NAME = 2, GENE_NAME_DOES_NOT_START_WITH_LETTER = 1, AMBIGOUS_GENE_NAME = 0};

SortGenesProxyModel::SortGenesProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
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

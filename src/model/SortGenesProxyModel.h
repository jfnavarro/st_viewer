/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SORTGENESPROXYMODEL_H
#define SORTGENESPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortGenesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:

    enum SortCategory {
        ambiguousGene = 0,
        numericGene = 1,
        normalGene = 2
    };

    explicit SortGenesProxyModel(QObject *parent = 0);
    virtual ~SortGenesProxyModel();

protected:

    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:

    Q_DISABLE_COPY(SortGenesProxyModel)
};

#endif // SORTGENESPROXYMODEL_H

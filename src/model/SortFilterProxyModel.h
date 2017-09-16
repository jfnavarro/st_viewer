#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

// A custom sorting class to allow to sort
// by the check-state (visible or not) for the genes
// and spots widgets
class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:

    explicit SortFilterProxyModel(QObject *parent = 0);

protected:

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

#endif // SORTFILTERPROXYMODEL_H

#include "SortFilterProxyModel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool SortFilterProxyModel::lessThan(const QModelIndex &left,
                                    const QModelIndex &right) const
{
    if (left.column() == 0) {
        const QVariant leftData = sourceModel()->data(left, Qt::CheckStateRole);
        const QVariant rightData = sourceModel()->data(right, Qt::CheckStateRole);
        return leftData.toBool() || !rightData.toBool();
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

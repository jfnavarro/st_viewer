#include "SortFilterProxyModel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool SortFilterProxyModel::lessThan(const QModelIndex &left,
                                    const QModelIndex &right) const
{
    const QVariant leftData = sourceModel()->data(left);
    const QVariant rightData = sourceModel()->data(right);
    if (leftData.type() == QVariant::Bool) {
        return leftData.toBool() || !rightData.toBool();
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

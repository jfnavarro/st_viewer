#include "SpotItemModel.h"

SpotItemModel::SpotItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant SpotItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

bool SpotItemModel::setHeaderData(int section, Qt::Orientation orientation,
                                  const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int SpotItemModel::rowCount(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    Q_UNUSED(parent)
    if (parent.isValid())
        return 0;
    return -1;
}

int SpotItemModel::columnCount(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    Q_UNUSED(parent)
    if (parent.isValid())
        return 0;
    return -1;
}

QVariant SpotItemModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool SpotItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags SpotItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

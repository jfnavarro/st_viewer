#include "DatasetItemModel.h"

#include <QModelIndex>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QDateTime>

#include "dataModel/Dataset.h"
#include <set>

static const int COLUMN_NUMBER = 5;

DatasetItemModel::DatasetItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

DatasetItemModel::~DatasetItemModel()
{
}

bool DatasetItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QAbstractItemModel::setData(index, value, role);
}

QVariant DatasetItemModel::data(const QModelIndex &index, int role) const
{
    // early out
    if (!index.isValid() || m_datasets_reference.empty()) {
        return QVariant(QVariant::Invalid);
    }

    if (role == Qt::DisplayRole) {
        DataProxy::DatasetPtr item = m_datasets_reference.at(index.row());
        Q_ASSERT(item);
        switch (index.column()) {
        case Name:
            return item->name();
        case Tissue:
            return item->statTissue();
        case Species:
            return item->statSpecies();
        case Created:
            return QDateTime::fromMSecsSinceEpoch(item->created().toLongLong());
        case LastModified:
            return QDateTime::fromMSecsSinceEpoch(item->lastModified().toLongLong());
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Created:
        case LastModified:
            return Qt::AlignRight;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant DatasetItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::ToolTipRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Name:
            return tr("Dataset name");
        case Tissue:
            return tr("Tissue name");
        case Species:
            return tr("Species name");
        case Created:
            return tr("Created at this date");
        case LastModified:
            return tr("Last Modified at this date");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Name:
            return tr("Name");
        case Tissue:
            return tr("Tissue");
        case Species:
            return tr("Species");
        case Created:
            return tr("Created");
        case LastModified:
            return tr("Last Modified");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Name:
        case Tissue:
        case Species:
        case Created:
        case LastModified:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

int DatasetItemModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

int DatasetItemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_datasets_reference.size();
}

Qt::ItemFlags DatasetItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    return defaultFlags;
}

void DatasetItemModel::loadDatasets(const DataProxy::DatasetList &datasetList)
{
    beginResetModel();
    m_datasets_reference = datasetList;
    endResetModel();
}

DataProxy::DatasetList DatasetItemModel::getDatasets(const QItemSelection &selection)
{
    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }
    // get the datasets and return them
    DataProxy::DatasetList datasetList;
    for (const auto &row : rows) {
        auto selection = m_datasets_reference.at(row);
        datasetList.push_back(selection);
    }
    return datasetList;
}

void DatasetItemModel::clear()
{
    beginResetModel();
    m_datasets_reference.clear();
    endResetModel();
}

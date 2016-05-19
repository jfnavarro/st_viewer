#include "UserSelectionsItemModel.h"
#include "dataModel/UserSelection.h"
#include <QDebug>
#include <QItemSelection>
#include <QDateTime>
#include <QColor>
#include <set>

static const int COLUMN_NUMBER = 7;

UserSelectionsItemModel::UserSelectionsItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

UserSelectionsItemModel::~UserSelectionsItemModel()
{
}

QVariant UserSelectionsItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_userSelectionList.empty()) {
        return QVariant(QVariant::Invalid);
    }

    const auto item = m_userSelectionList.at(index.row());
    Q_ASSERT(item);
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Name:
            return item->name();
        case Dataset:
            return item->datasetName();
        case NGenes:
            return QString::number(item->totalGenes());
        case NReads:
            return QString::number(item->totalReads());
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

    if (role == Qt::CheckStateRole && index.column() == Saved) {
        return item->saved() ? Qt::Checked : Qt::Unchecked;
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Saved:
            return Qt::AlignCenter;
        case NGenes:
        case NReads:
        case Created:
        case LastModified:
            return Qt::AlignRight;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant UserSelectionsItemModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Name");
        case Dataset:
            return tr("Dataset");
        case NGenes:
            return tr("Genes");
        case NReads:
            return tr("Reads");
        case Saved:
            return tr("Saved");
        case Created:
            return tr("Created");
        case LastModified:
            return tr("Last Modified");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name:
            return tr("The name of the selection");
        case Dataset:
            return tr("The dataset name where the selection was made");
        case NGenes:
            return tr("The number of unique genes present in the selection");
        case NReads:
            return tr("The total number of reads in the selection");
        case Saved:
            return tr("Yes if the selection is saved in the database");
        case Created:
            return tr("Created at this date");
        case LastModified:
            return tr("Last Modified at this date");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Name:
        case Dataset:
        case NGenes:
        case NReads:
        case Saved:
        case Created:
        case LastModified:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags UserSelectionsItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return defaultFlags;
}

int UserSelectionsItemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_userSelectionList.size();
}

int UserSelectionsItemModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void UserSelectionsItemModel::clear()
{
    beginResetModel();
    m_userSelectionList.clear();
    endResetModel();
}

void UserSelectionsItemModel::loadUserSelections(const DataProxy::UserSelectionList selectionList)
{
    beginResetModel();
    m_userSelectionList = selectionList;
    endResetModel();
}

DataProxy::UserSelectionList UserSelectionsItemModel::getSelections(const QItemSelection &selection)
{
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    DataProxy::UserSelectionList selectionList;
    for (const auto &row : rows) {
        auto selection = m_userSelectionList.at(row);
        selectionList.push_back(selection);
    }

    return selectionList;
}

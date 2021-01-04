#include "UserSelectionsItemModel.h"
#include "data/UserSelection.h"
#include <QDebug>
#include <QItemSelection>
#include <QDateTime>
#include <QColor>
#include <set>

static const int COLUMN_NUMBER = 4;

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

    const UserSelection &item = m_userSelectionList.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Name:
            return item.name();
        case Dataset:
            return item.dataset();
        case NGenes:
            return QString::number(item.totalGenes());
        case NSpots:
            return QString::number(item.totalSpots());
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case NGenes:
        case NSpots:
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
        case NSpots:
            return tr("Spots");
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
            return tr("The number of unique genes in the selection");
        case NSpots:
            return tr("The total number of spots in the selection");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Name:
        case Dataset:
        case NGenes:
        case NSpots:
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

void UserSelectionsItemModel::loadData(const QList<UserSelection> &selectionList)
{
    beginResetModel();
    m_userSelectionList = selectionList;
    endResetModel();
}

QList<UserSelection> UserSelectionsItemModel::getSelections(const QItemSelection &selection)
{
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    QList<UserSelection> selectionList;
    for (const auto &row : rows) {
        const UserSelection &selection = m_userSelectionList.at(row);
        selectionList.push_back(selection);
    }

    return selectionList;
}

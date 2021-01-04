#include "SpotItemModel.h"
#include <set>
#include <QDebug>
#include <QModelIndex>
#include <QMimeData>
#include <QStringList>
#include <QItemSelection>

#include "data/Spot.h"
#include "data/Dataset.h"

static const int COLUMN_NUMBER = 4;

SpotItemModel::SpotItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

SpotItemModel::~SpotItemModel()
{
}

QVariant SpotItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Spot");
        case Show:
            return tr("Show");
        case Count:
            return tr("#Count");
        case Color:
            return tr("Color");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name:
            return tr("The ID of the spot");
        case Show:
            return tr("Indicates if the spot is visible");
        case Count:
            return tr("The total number of reads of the spot");
        case Color:
            return tr("Indicates the color of the spot");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Show:
            return Qt::AlignCenter;
        case Color:
            return Qt::AlignLeft;
        case Count:
            return Qt::AlignCenter;
        case Name:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

int SpotItemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items_reference.size();
}

int SpotItemModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

QVariant SpotItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_items_reference.empty()) {
        return QVariant(QVariant::Invalid);
    }

    const auto item = m_items_reference.at(index.row());

    if ((role == Qt::DisplayRole || role == Qt::UserRole) && index.column() == Name) {
        return item->name();
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if ((role == Qt::CheckStateRole || role == Qt::UserRole) && index.column() == Show) {
        return item->visible() ? Qt::Checked : Qt::Unchecked;
    }

    if ((role == Qt::DisplayRole || role == Qt::UserRole) && index.column() == Count) {
        return item->totalCount();
    }

    if (role == Qt::DecorationRole && index.column() == Color) {
        return item->color();
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Show:
            return Qt::AlignCenter;
        case Color:
            return Qt::AlignCenter;
        case Count:
            return Qt::AlignCenter;
        case Name:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags SpotItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (!index.isValid()) {
        return defaultFlags;
    }

    switch (index.column()) {
    case Name:
        return defaultFlags;
    case Show:
        return Qt::ItemIsUserCheckable | defaultFlags;
    case Count:
        return defaultFlags;
    case Color:
        return defaultFlags;
    }

    return defaultFlags;
}

void SpotItemModel::loadData(const STData::SpotListType &spots)
{
    beginResetModel();
    m_items_reference = spots;
    endResetModel();
}

void SpotItemModel::clear()
{
    beginResetModel();
    m_items_reference.clear();
    endResetModel();
}

void SpotItemModel::setVisible(const QItemSelection &selection, bool visible)
{
    if (m_items_reference.empty()) {
        return;
    }

    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    // update the spots
    for (const auto &row : rows) {
        m_items_reference.at(row)->visible(visible);
    }
}

void SpotItemModel::setColor(const QItemSelection &selection, const QColor &color)
{
    if (m_items_reference.empty()) {
        return;
    }

    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    // update the spots
    for (const auto &row : rows) {
        m_items_reference.at(row)->color(color);
    }
}

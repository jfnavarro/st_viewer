#include "GeneItemModel.h"
#include <set>
#include <QDebug>
#include <QModelIndex>
#include <QMimeData>
#include <QStringList>
#include <QItemSelection>

#include "data/Gene.h"
#include "data/Dataset.h"

static const int COLUMN_NUMBER = 4;

GeneItemModel::GeneItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

GeneItemModel::~GeneItemModel()
{
}

QVariant GeneItemModel::data(const QModelIndex &index, int role) const
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

    if (role == Qt::DecorationRole && index.column() == Color) {
        return item->color();
    }

    if ((role == Qt::DisplayRole || role == Qt::UserRole) && index.column() == Count) {
        return item->totalCount();
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Show:
            return Qt::AlignCenter;
        case Color:
            return Qt::AlignCenter;
        case Name:
            return Qt::AlignLeft;
        case Count:
            return Qt::AlignCenter;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GeneItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Gene");
        case Show:
            return tr("Show");
        case Color:
            return tr("Color");
        case Count:
            return tr("#Count");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name:
            return tr("The name of the gene");
        case Show:
            return tr("Indicates if the genes is visible on the screen");
        case Color:
            return tr("Indicates the color of the gene on the screen");
        case Count:
            return tr("The total number of transcritps of the gene");
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
        case Name:
            return Qt::AlignLeft;
        case Count:
            return Qt::AlignCenter;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

int GeneItemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items_reference.size();
}

int GeneItemModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

Qt::ItemFlags GeneItemModel::flags(const QModelIndex &index) const
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
    case Color:
        return defaultFlags;
    case Count:
        return defaultFlags;
    }

    return defaultFlags;
}

void GeneItemModel::loadData(const STData::GeneListType &genes)
{
    beginResetModel();
    m_items_reference = genes;
    endResetModel();
}

void GeneItemModel::clear()
{
    beginResetModel();
    m_items_reference.clear();
    endResetModel();
}

void GeneItemModel::setVisibility(const QItemSelection &selection, bool visible)
{
    if (m_items_reference.empty()) {
        return;
    }

    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    // update the genes
    for (const auto &row : rows) {
        m_items_reference.at(row)->visible(visible);
    }
}

void GeneItemModel::setColor(const QItemSelection &selection, const QColor &color)
{
    if (m_items_reference.empty()) {
        return;
    }

    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    // update the genes
    for (const auto &row : rows) {
        m_items_reference.at(row)->color(color);
    }
}

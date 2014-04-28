/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneSelectionItemModel.h"

#include <QDebug>
#include "utils/DebugHelper.h"
#include "dataModel/GeneSelection.h"

GeneSelectionItemModel::GeneSelectionItemModel(QObject* parent)
    : QAbstractTableModel(parent)
{

}

GeneSelectionItemModel::~GeneSelectionItemModel()
{

}

QVariant GeneSelectionItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_geneselection.empty()) {
        return QVariant(QVariant::Invalid);
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const GeneSelection& item = m_geneselection.at(index.row());
        QVariant value;
        switch (index.column()) {
        case Name:
            value = item.name();
            break;
        case Hits:
            value = item.reads();
        break;
        case NormalizedHits:
            value = QString::number(item.normalizedReads(), 'f', 2);
            break;
        default:
            return QVariant(QVariant::Invalid);
        }
        return value;
    }
    // return invalid value
    return QVariant(QVariant::Invalid);
}

QVariant GeneSelectionItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant(QVariant::Invalid);
    }
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        QVariant value;
        switch (section) {
        case Name:
            value = tr("Name");
            break;
        case Hits:
            value = tr("Reads");
            break;
        case NormalizedHits:
            value = tr("Normalized Reads");
            break;
        default:
            return QVariant(QVariant::Invalid);
        }
        return value;
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        // return row number as label
        QVariant value(QVariant::Int);
        value = section + 1;
        return value;
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags GeneSelectionItemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return defaultFlags;
}

void GeneSelectionItemModel::sort(int column, Qt::SortOrder order)
{
    QAbstractItemModel::sort(column, order);
}

int GeneSelectionItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()  ? 0 : m_geneselection.count();
}

int GeneSelectionItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void GeneSelectionItemModel::reset()
{
    beginResetModel();
    m_geneselection.clear();
    endResetModel();
}

void GeneSelectionItemModel::loadSelectedGenes(GeneRendererGL::GeneSelectedSet selection)
{
    beginResetModel();
    m_geneselection = selection;
    endResetModel();
}

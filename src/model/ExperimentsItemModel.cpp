/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ExperimentsItemModel.h"

#include <QDebug>
#include "dataModel/GeneSelection.h"

ExperimentsItemModel::ExperimentsItemModel(QObject* parent)
    : QAbstractTableModel(parent)
{

}

ExperimentsItemModel::~ExperimentsItemModel()
{

}

QVariant ExperimentsItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_geneselectionList.empty()) {
        return QVariant(QVariant::Invalid);
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const DataProxy::GeneSelectionPtr item = m_geneselectionList.at(index.row());
        QVariant value;
        switch (index.column()) {
        case Name:
            value = item->name();
            break;
        case Dataset:
            value = item->datasetId();
        break;
        case Comment:
            value = item->comment();
            break;
        default:
            return QVariant(QVariant::Invalid);
        }
        return value;
    }
    // return invalid value
    return QVariant(QVariant::Invalid);
}

QVariant ExperimentsItemModel::headerData(int section,
                                            Qt::Orientation orientation, int role) const
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
        case Dataset:
            value = tr("Dataset");
            break;
        case Comment:
            value = tr("Comment");
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

Qt::ItemFlags ExperimentsItemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return defaultFlags;
}

void ExperimentsItemModel::sort(int column, Qt::SortOrder order)
{
    QAbstractItemModel::sort(column, order);
}

int ExperimentsItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()  ? 0 : m_geneselectionList.count();
}

int ExperimentsItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void ExperimentsItemModel::reset()
{
    beginResetModel();
    m_geneselectionList.clear();
    endResetModel();
}

void ExperimentsItemModel::loadSelectedGenes()
{
    beginResetModel();
    DataProxy* dataProxy = DataProxy::getInstance();
    m_geneselectionList = dataProxy->getGeneSelections();
    endResetModel();
}


/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneSelectionItemModel.h"

#include <QDebug>
#include "utils/DebugHelper.h"
#include "math/Common.h"
GeneSelectionItemModel::GeneSelectionItemModel(QObject* parent)
    : QAbstractTableModel(parent),
      m_geneselection_reference(0)
{

}

GeneSelectionItemModel::~GeneSelectionItemModel()
{

}

QVariant GeneSelectionItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_geneselection_reference.isNull()) {
        return QVariant(QVariant::Invalid);
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        DataProxy::FeaturePtr item = m_geneselection_reference->at(index.row());
        QVariant value;
        switch (index.column()) {
        case Name:
            value = item->gene();
            break;
        case Hits:
            value = std::min(item->hits(), m_max);
        break;
        case NormalizedHits:
            value = qreal(std::min(item->hits(), m_max)) / qreal(m_max);
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
            value = tr("Hits");
            break;
        case NormalizedHits:
            value = tr("Normalized Hits");
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

void GeneSelectionItemModel::sort(int column, Qt::SortOrder order)
{
    QAbstractItemModel::sort(column, order);
}

int GeneSelectionItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() || m_geneselection_reference.isNull() ? 0 : m_geneselection_reference->count();
}

int GeneSelectionItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void GeneSelectionItemModel::reset()
{
    beginResetModel();
    m_geneselection_reference.clear(); //NOTE genelist is just a reference
    endResetModel();
}

void GeneSelectionItemModel::loadGenes(DataProxy::FeatureListPtr selection)
{
    beginResetModel();
    m_geneselection_reference.clear(); //NOTE genelist is just a reference
    m_geneselection_reference = selection;
    endResetModel();
}

void GeneSelectionItemModel::setHitCountLimits(int min, int max)
{
    beginResetModel();
    m_min = min;
    m_max = max;
    endResetModel();
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneSelectionItemModel.h"

#include <QDebug>
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
        const GeneSelection::SelectionType& item = m_geneselection.at(index.row());
        switch (index.column()) {
        case Name: return item.name;
        case Hits: return item.reads;
        case NormalizedHits: return QString::number(item.normalizedReads, 'f', 2);
        case PixelItensity: return QString::number(item.pixeIntensity, 'f', 2);
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GeneSelectionItemModel::headerData(int section,
                                            Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant(QVariant::Invalid);
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name: return tr("Name");
        case Hits: return tr("Reads");
        case NormalizedHits: return tr("Normalized Reads");
        case PixelItensity: return tr("Pixel Intensity");
        default:
            return QVariant(QVariant::Invalid);
        }
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

bool GeneSelectionItemModel::geneName(const QModelIndex &index, QString *genename) const
{
    if (!index.isValid() || m_geneselection.empty()) {
        return false;
    }

    const GeneSelection::SelectionType& item = m_geneselection.at(index.row());
    if (index.column() == Name) {
         *genename = item.name;
         return true;
    }

    return false;
}

int GeneSelectionItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()  ? 0 : m_geneselection.count();
}

int GeneSelectionItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void GeneSelectionItemModel::loadSelectedGenes(
        const GeneSelection::selectedItemsList& selectionList)
{
    beginResetModel();
    m_geneselection.clear();
    m_geneselection = selectionList;
    endResetModel();
}

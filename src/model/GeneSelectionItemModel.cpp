/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "GeneSelectionItemModel.h"

#include <QDebug>
#include "dataModel/GeneSelection.h"

static const int COLUMN_NUMBER = 5;

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

    const SelectionType& item = m_geneselection.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Name: return item.name;
        case Count: return QString::number(item.count);
        case Hits: return item.reads;
        case NormalizedHits: return QString::number(item.normalizedReads, 'f', 2);
        case PixelItensity: return QString::number(item.pixeIntensity, 'f', 2);
        default: return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Count: return Qt::AlignRight;
        case Hits: return Qt::AlignRight;
        case NormalizedHits: return Qt::AlignRight;
        case PixelItensity: return Qt::AlignRight;
        default: return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GeneSelectionItemModel::headerData(int section,
                                            Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name: return tr("Gene");
        case Count: return tr("Count");
        case Hits: return tr("Reads");
        case NormalizedHits: return tr("N. Reads");
        case PixelItensity: return tr("P. Intensity");
        default: return QVariant(QVariant::Invalid);
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return (section + 1);
    }
    else if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name: return tr("The name of the gene");
        case Count: return tr("The number of occurences of the gene");
        case Hits: return tr("The aggregated number of reads");
        case NormalizedHits: return tr("The normalized aggregated number of reads");
        case PixelItensity: return tr("The normalized pixel intensity");
        default: return QVariant(QVariant::Invalid);
        }
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

    const SelectionType& item = m_geneselection.at(index.row());
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

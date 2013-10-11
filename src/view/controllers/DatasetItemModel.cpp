/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "model/Dataset.h"

#include "DatasetItemModel.h"

DatasetItemModel::DatasetItemModel(QObject* parent)
    : QAbstractTableModel(parent)
{

}

DatasetItemModel::~DatasetItemModel()
{

}

bool DatasetItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return QAbstractItemModel::setData(index, value, role);
}

QVariant DatasetItemModel::data ( const QModelIndex& index, int role ) const
{
    // early out
    if (!index.isValid())
    {
        return QVariant(QVariant::Invalid);
    }

    DataProxy::DatasetPtr item = DataProxy::getInstance()->getDatasetMap()->values().at(index.row()); //TODO too much exposure..
    
    if (role == Qt::DisplayRole)
    {
        switch(index.column())
        {
            case Name: return item->name();
            case Created: return item->statCreated();
            case Tissue: return item->statTissue();
            case Specie: return item->statSpecie();
            case Aligned: return (item->figureStatus() & Dataset::Aligned) ? tr("Yes") : tr("No");
            case Barcodes: return item->statBarcodes();
            case Genes: return item->statGenes();
            case UBarcodes: return item->statUniqueBarcodes();
            case UGenes: return item->statUniqueGenes();
            case Comments: return item->statComments();
            default: Q_ASSERT_X(false, "DatasetItemModel", "Unknown column!");
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant DatasetItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant(QVariant::Invalid);
    }
    
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case Name :return tr("Name");
            case Created :return tr("Created");
            case Tissue :return tr("Tissue");
            case Specie : return tr("Specie");
            case Aligned : return tr("Aligned");
            case Barcodes : return tr("Reads");
            case Genes : return tr("Events");
            case UBarcodes : return tr("Detected Barcodes");
            case UGenes : return tr("Detected Genes");
            case Comments: return tr("Comments");
            default: Q_ASSERT_X(false, "DatasetItemModel", "Unknown column!");
        }
    }
    else if (orientation == Qt::Vertical)
    {
        return (section + 1);
    }

    return QVariant(QVariant::Invalid);
}

int DatasetItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : DatasetItemModel::MColumns;
}

int DatasetItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : DataProxy::getInstance()->getDatasetMap()->count();
}

Qt::ItemFlags DatasetItemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    
    if (index.isValid())
    {
        flags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return flags;
}

void DatasetItemModel::loadDatasets()
{
    beginResetModel();
    endResetModel();
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "model/Dataset.h"

#include <QModelIndex>
#include <QModelIndex>
#include <QStandardItemModel>

#include "DatasetItemModel.h"

DatasetItemModel::DatasetItemModel(QObject* parent)
    : QAbstractTableModel(parent),m_datasets_reference(0)
{

}

DatasetItemModel::~DatasetItemModel()
{

}

bool DatasetItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return QAbstractItemModel::setData(index, value, role);
}

QVariant DatasetItemModel::data(const QModelIndex& index, int role) const
{
    // early out
    if (!index.isValid() || m_datasets_reference.isNull())
    {
        return QVariant(QVariant::Invalid);
    }

    DataProxy::DatasetPtr item = m_datasets_reference->at(index.row());
    
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
    return parent.isValid() ? 0 : DatasetItemModel::COLUMN_NUMBER;
}

int DatasetItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() || m_datasets_reference.isNull() ? 0 : m_datasets_reference->count();
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
    m_datasets_reference.clear(); //NOTE m_datasets_reference is just a reference
    DataProxy* dataProxy = DataProxy::getInstance();
    m_datasets_reference = dataProxy->getDatasetList();
    endResetModel();
}

void DatasetItemModel::datasetSelected(const QModelIndex &index)
{   
    if(index.isValid())
    {
        DataProxy::DatasetPtr item = m_datasets_reference->at(index.row());
        emit datasetSelected(item);
    }
}

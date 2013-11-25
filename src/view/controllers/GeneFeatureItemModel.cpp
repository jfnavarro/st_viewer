/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneFeatureItemModel.h"

#include <QDebug>
#include <QModelIndex>
#include <QMimeData>
#include <QStringList>

#include "utils/DebugHelper.h"

const QString GeneFeatureItemModel::MIMETYPE_APPGENELIST = QStringLiteral("application/gene.list");

GeneFeatureItemModel::GeneFeatureItemModel(QObject* parent)
    : QAbstractTableModel(parent), m_genelist_reference(0)
{

}

GeneFeatureItemModel::~GeneFeatureItemModel()
{

}

QVariant GeneFeatureItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_genelist_reference.isNull()) {
        return QVariant(QVariant::Invalid);
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        DataProxy::GenePtr item = m_genelist_reference->at(index.row());

        QVariant value;
        switch (index.column()) {
        case Name:
            value = item->name();
            break;
        case Show:
            value = item->selected() ? Qt::Checked : Qt::Unchecked;
            break;
        case Color:
            value = item->color();
            break;
        default:
            return QVariant(QVariant::Invalid);
        }
        return value;
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

QVariant GeneFeatureItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case Show:
            value = tr("Show");
            break;
        case Color:
            value = tr("Color");
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

bool GeneFeatureItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (!index.isValid() || m_genelist_reference.isNull()) {
        return false;
    }

    int row = index.row();
    int column = index.column();

    if (role == Qt::EditRole) {
        DataProxy::GenePtr item = m_genelist_reference->at(index.row());

        switch (column) {
        case Show:
            if (item->selected() != value.toBool()) {
                item->selected(value.toBool());
                emit dataChanged(index, index);
                emit signalSelectionChanged(item);
                return true;
            }
            break;
        case Color: {
            const QColor color = qvariant_cast<QColor>(value);
            if (color.isValid() && item->color() != color) {
                item->color(color);
                emit dataChanged(index, index);
                emit signalColorChanged(item);
                return true;
            }
        }
        default:
            return false;
        }
    }

    return false;
}

void GeneFeatureItemModel::sort(int column, Qt::SortOrder order)
{
    QAbstractItemModel::sort(column, order);
}

int GeneFeatureItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() || m_genelist_reference.isNull() ? 0 : m_genelist_reference->count();
}

int GeneFeatureItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

Qt::ItemFlags GeneFeatureItemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (!index.isValid()) {
        return defaultFlags;
    }

    switch (index.column()) {
    case Name:
        return Qt::ItemIsDragEnabled | defaultFlags;
        break;
    case Show:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | defaultFlags;
        break;
    case Color:
        return Qt::ItemIsEditable | Qt::ItemIsDragEnabled | defaultFlags;
        break;
    default:
        Q_ASSERT(false && "[GeneFeatureItemModel] Invalid column index!");
    }

    return defaultFlags;
}

void GeneFeatureItemModel::loadGenes()
{
    beginResetModel();
    m_genelist_reference.clear(); //NOTE genelist is just a reference
    DataProxy* dataProxy = DataProxy::getInstance();
    m_genelist_reference = dataProxy->getGeneList(dataProxy->getSelectedDataset());
    endResetModel();
}

void GeneFeatureItemModel::selectAllGenesPressed(bool selected)
{
    if (m_genelist_reference.isNull()) {
        return;
    }

    const int size = m_genelist_reference->count();
    for (int i = 0; i < size; ++i) {
        DataProxy::GenePtr gene = m_genelist_reference->at(i);
        if (!gene.isNull()) {
            QModelIndex index = createIndex(i, GeneFeatureItemModel::Show);
            if (gene->selected() != selected) {
                gene->selected(selected);
                emit dataChanged(index, index);
                emit signalSelectionChanged(gene);
            }
        }
    }

}

void GeneFeatureItemModel::setColorGenes(const QColor& color)
{
    if (m_genelist_reference.isNull()) {
        return;
    }

    const int size = m_genelist_reference->count();
    for (int i = 0; i < size; ++i) {
        DataProxy::GenePtr gene = m_genelist_reference->at(i);
        if (!gene.isNull()) {
            QModelIndex index = createIndex(i, GeneFeatureItemModel::Show);
            gene->color(color);
            emit dataChanged(index, index);
            emit signalColorChanged(gene);
        }
    }
}

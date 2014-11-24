/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneFeatureItemModel.h"

#include <set>
#include <QDebug>
#include <QModelIndex>
#include <QMimeData>
#include <QStringList>
#include <QItemSelection>

static const int COLUMN_NUMBER = 3;

GeneFeatureItemModel::GeneFeatureItemModel(QObject* parent)
    : QAbstractTableModel(parent)
{

}

GeneFeatureItemModel::~GeneFeatureItemModel()
{

}

QVariant GeneFeatureItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_genelist_reference.isEmpty()) {
        return QVariant(QVariant::Invalid);
    }

    DataProxy::GenePtr item = m_genelist_reference.at(index.row());
    Q_ASSERT(!item.isNull());

    if (role == Qt::DisplayRole && index.column() == Name) {
        return item->name();
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::CheckStateRole && index.column() == Show) {
        return item->selected() ? Qt::Checked : Qt::Unchecked;
    }

    if (role == Qt::DecorationRole && index.column() == Color) {
        return item->color();
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Show: return Qt::AlignCenter;
        case Color: return Qt::AlignCenter;
        case Name: return Qt::AlignLeft;
        default: return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GeneFeatureItemModel::headerData(int section,
                                          Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name: return tr("Gene");
        case Show: return tr("Show");
        case Color: return tr("Color");
        default: return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name: return tr("The name of the gene");
        case Show: return tr("Indicates if the genes is visible on the screen");
        case Color: return tr("Indicates the color of the gene on the screen");
        default: return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Show: return Qt::AlignCenter;
        case Color: return Qt::AlignCenter;
        case Name: return Qt::AlignLeft;
        default: return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

int GeneFeatureItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_genelist_reference.count();
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
    case Name: return  defaultFlags;
    case Show: return Qt::ItemIsUserCheckable | defaultFlags;
    case Color: return  defaultFlags;
    default: Q_ASSERT("[GeneFeatureItemModel] Invalid column index!");
    }

    return defaultFlags;
}

void GeneFeatureItemModel::loadGenes(const DataProxy::GeneList &geneList)
{
    beginResetModel();
    m_genelist_reference.clear();
    m_genelist_reference = geneList;
    endResetModel();
}

void GeneFeatureItemModel::clearGenes()
{
    beginResetModel();
    m_genelist_reference.clear();
    endResetModel();
}

bool GeneFeatureItemModel::geneName(const QModelIndex &index, QString *genename) const
{
    if (!index.isValid() || m_genelist_reference.isEmpty()) {
        return false;
    }

    DataProxy::GenePtr item = m_genelist_reference.at(index.row());
    Q_ASSERT(!item.isNull());

    if (index.column() == Name) {
        *genename = item->name();
        return true;
    }

    return false;
}

void GeneFeatureItemModel::setGeneVisibility(const QItemSelection &selection, bool visible)
{
    if (m_genelist_reference.isEmpty()) {
        return;
    }

    std::set<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    DataProxy::GeneList geneList;
    for (const auto &row : rows) {
        DataProxy::GenePtr gene = m_genelist_reference.at(row);
        if (!gene.isNull() && gene->selected() != visible) {
            gene->selected(visible);
            geneList.push_back(gene);
        }
    }


    emit signalSelectionChanged(geneList);
}

void GeneFeatureItemModel::setGeneColor(const QItemSelection &selection, const QColor& color)
{
    if (m_genelist_reference.isEmpty()) {
        return;
    }

    std::set<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    DataProxy::GeneList geneList;
    for (const auto &row : rows) {
        DataProxy::GenePtr gene = m_genelist_reference.at(row);
        if (!gene.isNull() && color.isValid() && gene->color() != color) {
            gene->color(color);
            geneList.push_back(gene);
        }
    }

    emit signalColorChanged(geneList);
}


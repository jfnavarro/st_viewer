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

const QString GeneFeatureItemModel::MIMETYPE_APPGENELIST =
        QStringLiteral("application/gene.list");

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

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        Q_ASSERT(m_genelist_reference.size() > index.row());

        DataProxy::GenePtr item = m_genelist_reference.at(index.row());
        Q_ASSERT(!item.isNull());

        switch (index.column()) {
        case Name: return item->name();
        case Show: return item->selected() ? Qt::Checked : Qt::Unchecked;
        case Color: return item->color();
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

QVariant GeneFeatureItemModel::headerData(int section,
                                          Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant(QVariant::Invalid);
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name: return tr("Name");
        case Show: return tr("Show");
        case Color: return tr("Color");
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

bool GeneFeatureItemModel::setData(const QModelIndex& index,
                                   const QVariant& value, int role)
{
    if (!index.isValid() || m_genelist_reference.isEmpty()) {
        return false;
    }

    if (role == Qt::EditRole) {
        DataProxy::GenePtr item = m_genelist_reference.at(index.row());
        Q_ASSERT(!item.isNull());

        const int column = index.column();
        switch (column) {
        case Show:
            if (item->selected() != value.toBool()) {
                item->selected(value.toBool());
                emit dataChanged(index, index);
                DataProxy::GeneList geneList;
                geneList.push_back(item);
                emit signalSelectionChanged(geneList);
            }
            return true;
        case Color: {
            const QColor color = qvariant_cast<QColor>(value);
            if (color.isValid() && item->color() != color) {
                item->color(color);
                emit dataChanged(index, index);
                DataProxy::GeneList geneList;
                geneList.push_back(item);
                emit signalColorChanged(geneList);
            }
            return true;
        }
        default:
            return false;
        }
    }
    return false;
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
    case Name:
        return Qt::ItemIsDragEnabled | defaultFlags;
    case Show:
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | defaultFlags;
    case Color:
        return Qt::ItemIsEditable | Qt::ItemIsDragEnabled | defaultFlags;
    default:
        Q_ASSERT(false && "[GeneFeatureItemModel] Invalid column index!");
    }

    return defaultFlags;
}

void GeneFeatureItemModel::loadGenes()
{
    beginResetModel();
    m_genelist_reference.clear();
    DataProxy *dataProxy = DataProxy::getInstance();
    m_genelist_reference = dataProxy->getGeneList(dataProxy->getSelectedDataset());
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

namespace {

struct ContiguousSegment {
  ContiguousSegment(int start_, int end_) : start(start_), end(end_) {
  }
  int start;
  int end;
};

// This function has the the selected rows as input and returns a vector of
// contiguous rows. For instance the rows
// 2,3,4,7,8,11,15,16,17
// would yield the row segments (2,4) (7,8), (11,11), (15,17)

std::vector< ContiguousSegment > getContiguousRowSegments(const QItemSelection &selection)
{
    std::set<int> rows_set;
    for (const auto &index : selection.indexes()) {
        rows_set.insert(index.row());
    }
    std::vector<int> rows_vector;
    std::copy(rows_set.begin(), rows_set.end(), std::back_inserter(rows_vector));
    std::sort(rows_vector.begin(), rows_vector.end());
    std::vector< ContiguousSegment > contiguousSegments;
    bool firstLoop = true;
    int rectangle_start_row_number = -1;
    int last_rectangle_number = -1;
    for (const auto &row : rows_vector) {
        if (firstLoop) {
            firstLoop = false;
            rectangle_start_row_number = row;
            last_rectangle_number = row;
        } else {
            if (last_rectangle_number + 1 == row) {
	        last_rectangle_number = row;
	    } else {
                contiguousSegments.push_back(ContiguousSegment(rectangle_start_row_number, last_rectangle_number));
                rectangle_start_row_number = row;
                last_rectangle_number = row;
            }
        }
    }
    if (!rows_vector.empty()) {
         contiguousSegments.push_back(ContiguousSegment(rectangle_start_row_number, rows_vector.back()));
    }
    return contiguousSegments;
}

}

void GeneFeatureItemModel::setGeneVisibility(const QItemSelection &selection,
                                             bool visible)
{
    if (m_genelist_reference.isEmpty()) {
        return;
    }
    DataProxy::GeneList geneList;
    auto rowSegments = getContiguousRowSegments(selection);
    for (auto rowSegment : rowSegments) {
        int row = rowSegment.start;
        while (row < rowSegment.end + 1) {
            auto &gene = m_genelist_reference.at(row);
            if (!gene.isNull() && gene->selected() != visible) {
                gene->selected(visible);
                geneList.push_back(gene);
            }
            ++row;
        }
        QModelIndex topLeftIndex = index(rowSegment.start, Show, QModelIndex());
        QModelIndex bottomRightIndex = index(rowSegment.end, Show, QModelIndex());
        emit dataChanged(topLeftIndex, bottomRightIndex);
    }
    emit signalSelectionChanged(geneList);
}

void GeneFeatureItemModel::setGeneColor(const QItemSelection &selection, const QColor& color)
{
    if (m_genelist_reference.isEmpty()) {
        return;
    }
    DataProxy::GeneList geneList;
    auto rowSegments = getContiguousRowSegments(selection);
    for (auto rowSegment : rowSegments) {
        int row = rowSegment.start;
        while (row < rowSegment.end + 1) {
            auto &gene = m_genelist_reference.at(row);
            if (!gene.isNull() && color.isValid() && gene->color() != color) {
                gene->color(color);
                geneList.push_back(gene);
            }
            ++row;
        }
        QModelIndex topLeftIndex = index(rowSegment.start, Color, QModelIndex());
        QModelIndex bottomRightIndex = index(rowSegment.end, Color, QModelIndex());
        emit dataChanged(topLeftIndex, bottomRightIndex);
    }
    emit signalColorChanged(geneList);
}


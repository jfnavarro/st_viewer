#include "GeneSelectionDEAItemModel.h"

#include <QItemSelection>
#include <set>

static const int COLUMN_NUMBER = 5;

GeneSelectionDEAItemModel::GeneSelectionDEAItemModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

GeneSelectionDEAItemModel::~GeneSelectionDEAItemModel()
{
}

QVariant GeneSelectionDEAItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_combinedSelections.empty()) {
        return QVariant(QVariant::Invalid);
    }

    const AnalysisDEA::deaReads& item = m_combinedSelections.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Name:
            return item.gene;
        case HitsA:
            return item.readsA;
        case TPMA:
            return item.normalizedReadsA;
        case HitsB:
            return item.readsB;
        case TPMB:
            return item.normalizedReadsB;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Name:
            return Qt::AlignLeft;
        case HitsA:
            return Qt::AlignRight;
        case TPMA:
            return Qt::AlignRight;
        case HitsB:
            return Qt::AlignRight;
        case TPMB:
            return Qt::AlignRight;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GeneSelectionDEAItemModel::headerData(int section,
                                               Qt::Orientation orientation,
                                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Gene");
        case HitsA:
            return tr("Reads Sel. A");
        case TPMA:
            return tr("TPM Sel. A");
        case HitsB:
            return tr("Reads Sel. B");
        case TPMB:
            return tr("TPM Sel. B");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name:
            return tr("The name of the gene");
        case HitsA:
            return tr("The number of reads for this gene in selection A (0 means not expressed)");
        case TPMA:
            return tr("The TPM normalized number of the number of genes in selection A (1 means no "
                      "expressed)");
        case HitsB:
            return tr("The number of reads for this gene in selection A (0 means not expressed)");
        case TPMB:
            return tr("The TPM normalized number of the number of genes in selection A (1 means no "
                      "expressed)");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Name:
            return Qt::AlignLeft;
        case HitsA:
            return Qt::AlignLeft;
        case TPMA:
            return Qt::AlignLeft;
        case HitsB:
            return Qt::AlignLeft;
        case TPMB:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags GeneSelectionDEAItemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return defaultFlags;
}

bool GeneSelectionDEAItemModel::geneName(const QModelIndex& index, QString* genename) const
{
    if (!index.isValid() || m_combinedSelections.empty()) {
        return false;
    }

    const AnalysisDEA::deaReads& item = m_combinedSelections.at(index.row());

    if (index.column() == Name) {
        *genename = item.gene;
        return true;
    }

    return false;
}

int GeneSelectionDEAItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_combinedSelections.count();
}

int GeneSelectionDEAItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void GeneSelectionDEAItemModel::loadCombinedSelectedGenes(
    const AnalysisDEA::combinedSelectionsType& combinedSelections)
{
    beginResetModel();
    m_combinedSelections.clear();
    m_combinedSelections = combinedSelections;
    endResetModel();
}

AnalysisDEA::combinedSelectionsType GeneSelectionDEAItemModel::getSelections(
    const QItemSelection& selection)
{
    std::set<int> rows;
    for (const auto& index : selection.indexes()) {
        rows.insert(index.row());
    }

    AnalysisDEA::combinedSelectionsType selectionList;
    for (const auto& row : rows) {
        auto selection = m_combinedSelections.at(row);
        selectionList.push_back(selection);
    }

    return selectionList;
}

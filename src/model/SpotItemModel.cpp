#include "SpotItemModel.h"
#include <set>
#include <QDebug>
#include <QModelIndex>
#include <QMimeData>
#include <QStringList>
#include <QItemSelection>

#include "data/Spot.h"
#include "data/Dataset.h"

static const int COLUMN_NUMBER = 3;

SpotItemModel::SpotItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant SpotItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Spot");
        case Show:
            return tr("Show");
        case Color:
            return tr("Color");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name:
            return tr("The coordinates of the spot");
        case Show:
            return tr("Indicates if the spot is visible on the screen");
        case Color:
            return tr("Indicates the spot of the gene on the screen");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Show:
            return Qt::AlignCenter;
        case Color:
            return Qt::AlignLeft;
        case Name:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

int SpotItemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items_reference.size();
}

int SpotItemModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

QVariant SpotItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_items_reference.empty()) {
        return QVariant(QVariant::Invalid);
    }

    const auto item = m_items_reference.at(index.row());

    if (role == Qt::DisplayRole && index.column() == Name) {
        return item->name();
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::CheckStateRole && index.column() == Show) {
        return item->visible() ? Qt::Checked : Qt::Unchecked;
    }

    if (role == Qt::DecorationRole && index.column() == Color) {
        return item->color();
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Show:
            return Qt::AlignCenter;
        case Color:
            return Qt::AlignCenter;
        case Name:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags SpotItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (!index.isValid()) {
        return defaultFlags;
    }

    switch (index.column()) {
    case Name:
        return defaultFlags;
    case Show:
        return Qt::ItemIsUserCheckable | defaultFlags;
    case Color:
        return defaultFlags;
    }

    return defaultFlags;
}

void SpotItemModel::loadDataset(const Dataset &dataset)
{
    Q_UNUSED(dataset)
    beginResetModel();
    m_items_reference = dataset.data()->spots();
    endResetModel();
}

void SpotItemModel::clear()
{
    beginResetModel();
    m_items_reference.clear();
    endResetModel();
}

void SpotItemModel::setVisibility(const QItemSelection &selection, bool visible)
{
    if (m_items_reference.empty()) {
        return;
    }

    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    // update the spots
    for (const auto &row : rows) {
        auto spot = m_items_reference.at(row);
        if (spot->visible() != visible) {
            spot->visible(visible);
        }
    }
}

void SpotItemModel::setColor(const QItemSelection &selection, const QColor &color)
{
    if (m_items_reference.empty()) {
        return;
    }

    // get unique indexes from the user selection
    QSet<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    // update the spots
    for (const auto &row : rows) {
        auto spot = m_items_reference.at(row);
        if (color.isValid() && spot->color() != color) {
            spot->color(color);
        }
    }
}

bool SpotItemModel::loadSpotColorsFile(const QString &filename)
{
    QStringList color_list;
    color_list << "red" << "green" << "blue" << "cyan" << "magenta"
               << "yellow" << "black" << "grey" << "darkBlue" << "darkGreen";
    QMap<Spot::SpotType, QColor> spotMap;
    QFile file(filename);
    bool parsed = true;
    // Parse the spots map = spot -> color
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() != 2) {
                parsed = false;
                break;
            }
            const QString spot = fields.at(1);
            const QColor color = color_list.at(fields.at(0).toInt() + 1);
            fields = spot.split("x");
            if (fields.length() != 2) {
                parsed = false;
                break;
            }
            const float x = fields.at(0).toFloat();
            const float y = fields.at(1).toFloat();
            spotMap.insert(Spot::SpotType(x, y), color);
        }

        if (spotMap.empty()) {
            qDebug() << "No valid spots could be found in the spots colors file";
            parsed = false;
        }

    } else {
        qDebug() << "Could not open spots colors file";
    }

    file.close();

    // Update spot colors
    if (parsed) {
        QMap<Spot::SpotType, QColor>::const_iterator it = spotMap.constBegin();
        while (it != spotMap.constEnd()) {
            const Spot::SpotType oldspot = it.key();
            const QColor color = it.value();
            auto it_spot =
                    std::find_if(m_items_reference.begin(), m_items_reference.end(),
                                 [&](const auto spot) { return spot->coordinates() == oldspot; });
            if (it_spot != m_items_reference.end()) {
                (*it_spot)->color(color);
            }
            ++it;
        }
    }

    return parsed;
}

void SpotItemModel::loadSpotColors(const QVector<QColor> &colors)
{
    Q_ASSERT(m_items_reference.size() == colors.size());
    for (int i = 0; i < colors.size(); ++i) {
        m_items_reference[i]->color(colors.at(i));
    }
}

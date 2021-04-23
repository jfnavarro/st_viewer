#ifndef CLUSTERITEMMODEL_H
#define CLUSTERITEMMODEL_H

#include <QAbstractTableModel>

#include "data/STData.h"

class QModelIndex;
class QStringList;
class QMimeData;
class QItemSelection;
class Cluster;

// Data model for the clusters. Clusters are lists of spots with color/visible/name properties
// Allows users to interact with the clusters (visible or not visible for instance)
class ClusterItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Show = 0,
        Name = 1,
        Color = 2,
        Count = 3
    };

    explicit ClusterItemModel(QObject *parent = nullptr);
    virtual ~ClusterItemModel();

    // header
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // basic functionality
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // this function will set to visible the clusters included in the selection
    void setVisible(const QItemSelection &selection, bool visible);

    // this function will modify the color of the clusters included in the selection
    void setColor(const QItemSelection &selection, const QColor &color);

    // reload the model's data
    void loadData(const STData::ClusterListType &clusters);

    // clear and reset the model
    void clear();

public slots:

signals:

private:

    STData::ClusterListType m_items_reference;

    Q_DISABLE_COPY(ClusterItemModel)
};

#endif // CLUSTERITEMMODEL_H

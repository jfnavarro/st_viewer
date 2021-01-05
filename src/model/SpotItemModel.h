#ifndef SPOTITEMMODEL_H
#define SPOTITEMMODEL_H

#include <QAbstractTableModel>
#include "data/STData.h"

class QModelIndex;
class QStringList;
class QMimeData;
class QItemSelection;
class Dataset;

// Data model class for the spot data (specific to a dataset).
// Primarily used to enumerate the spots in the cell spots table
// and allow the user to interact with individual spots.
class SpotItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Show = 0,
        Name = 1,
        Count = 2,
        Color = 3
    };

    explicit SpotItemModel(QObject *parent = nullptr);
    virtual ~SpotItemModel();

    // header
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // basic functionality
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // this function will set to visible the spots included in the selection
    void setVisible(const QItemSelection &selection, bool visible);

    // this function will modify the color of the spots included in the selection
    void setColor(const QItemSelection &selection, const QColor &color);

    // reload the model's data from the dataset (spots)
    void loadData(const STData::SpotListType &spots);

    // clear and reset the model
    void clear();

public slots:

signals:

private:
    STData::SpotListType m_items_reference;

    Q_DISABLE_COPY(SpotItemModel)
};

#endif // SPOTITEMMODEL_H

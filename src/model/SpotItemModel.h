#ifndef SPOTITEMMODEL_H
#define SPOTITEMMODEL_H

#include <QAbstractTableModel>
#include "data/STData.h"

class QModelIndex;
class QStringList;
class QMimeData;
class QItemSelection;
class Dataset;

// Wrapper model class for the spot data (specific to a dataset).
// Primarily used to enumerate the spots in the cell view (spots table)
// and allow the user to interact with individual spots.
class SpotItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)
public:

    enum Column { Show = 0, Name = 1, Color = 2 };

    explicit SpotItemModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // this function will set to visible the spots included in the selection
    // and emit a signal with the modified spots
    void setVisibility(const QItemSelection &selection, bool visible);

    // this function will modify the color of the spots included in the selection
    // and emit a signal with the modified spots
    void setColor(const QItemSelection &selection, const QColor &color);

    // reload the model's data from the dataset (spots)
    void loadDataset(const Dataset &dataset);

    // loads a file containing colors for each spot
    bool loadSpotColors(const QString &filename);

    // clear and reset the model
    void clear();

public slots:

signals:
    // Signals to notify that any of the spot|s properties have changed
    void signalSpotSelectionChanged();
    void signalSpotColorChanged();

private:
    STData::SpotListType m_items_reference;

    Q_DISABLE_COPY(SpotItemModel)
};

#endif // SPOTITEMMODEL_H

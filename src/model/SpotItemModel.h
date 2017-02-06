#ifndef SPOTITEMMODEL_H
#define SPOTITEMMODEL_H

#include <QAbstractTableModel>

class SpotItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    enum Column { Show = 0, Name = 1, CutOff = 2, Color = 3 };

    explicit SpotItemModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
};

#endif // SPOTITEMMODEL_H

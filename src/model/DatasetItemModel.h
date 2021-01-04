#ifndef DATASETITEMMODEL_H
#define DATASETITEMMODEL_H

#include <QAbstractItemModel>

class QModelIndex;
class QStandardItemModel;
class QItemSelection;
class Dataset;

// Data model class for datasets in the Datasets table
// TODO highlight openned dataset
class DatasetItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column {
        Name = 0,
        Comments = 1
    };

    explicit DatasetItemModel(QObject *parent = nullptr);
    virtual ~DatasetItemModel();

    // Necessary to override
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Returns a list of datasets for the given items selection
    QList<Dataset> getDatasets(const QItemSelection &selection);

    // Updates the data model with the data given as input
    void loadData(const QList<Dataset> &datasetList);

    // Clear the current model
    void clear();

private:
    QList<Dataset> m_datasets_reference;

    Q_DISABLE_COPY(DatasetItemModel)
};

#endif // DATASETITEMMODEL_H

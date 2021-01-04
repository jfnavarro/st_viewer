#ifndef GENEFITEMMODEL_H
#define GENEFITEMMODEL_H

#include <QAbstractTableModel>
#include "data/STData.h"

class QModelIndex;
class QStringList;
class QMimeData;
class QItemSelection;
class Dataset;

// Data model class for the gene data (specific to a dataset).
// Primarily used to enumerate the genes in the genes table
// and allow the user to interact with individual genes.
class GeneItemModel : public QAbstractTableModel
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

    explicit GeneItemModel(QObject *parent = nullptr);
    virtual ~GeneItemModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // this function will set to visible the genes included in the selection
    void setVisibility(const QItemSelection &selection, bool visible);

    // this function will modify the color of the genes included in the selection
    void setColor(const QItemSelection &selection, const QColor &color);

    // reload the model's data from the dataset (genes)
    void loadData(const STData::GeneListType &genes);

    // clear and reset the model
    void clear();

public slots:

signals:

private:
    STData::GeneListType m_items_reference;

    Q_DISABLE_COPY(GeneItemModel)
};

#endif // GENEFITEMMODEL_H

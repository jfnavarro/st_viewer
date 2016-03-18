#ifndef EXPERIMENTSITEMMODEL_H
#define EXPERIMENTSITEMMODEL_H

#include <QtCore/QModelIndex>
#include "data/DataProxy.h"
#include <QAbstractTableModel>

class UserSelection;
class QModelIndex;
class QStringList;
class QItemSelection;

// Wrapper model for the user selections
class UserSelectionsItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column {
        Saved = 0,
        Name = 1,
        Dataset = 2,
        NGenes = 3,
        NReads = 4,
        NFeatures = 5,
        Comment = 6,
        Created = 7,
        LastModified = 8
    };

    explicit UserSelectionsItemModel(QObject* parent = 0);
    virtual ~UserSelectionsItemModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // reset current model
    void clear();

    // loads the data of the model from the given input
    void loadUserSelections(const DataProxy::UserSelectionList selectionList);

    // returns a list of selections items from the indexes given as input
    DataProxy::UserSelectionList getSelections(const QItemSelection& selection);

private:
    DataProxy::UserSelectionList m_userSelectionList;

    Q_DISABLE_COPY(UserSelectionsItemModel)
};

#endif // EXPERIMENTSITEMMODEL_H

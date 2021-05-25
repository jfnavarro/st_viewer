#ifndef USERSELECTIONITEMMODEL_H
#define USERSELECTIONITEMMODEL_H

#include <QModelIndex>
#include <QAbstractTableModel>

class UserSelection;
class QModelIndex;
class QItemSelection;

// Data model for the user selections in the selections table
class UserSelectionsItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Name = 0,
        Dataset = 1,
        NGenes = 2,
        NSpots = 3
    };

    explicit UserSelectionsItemModel(QObject *parent = nullptr);
    virtual ~UserSelectionsItemModel();

    // header
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // basic funcionality
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // returns a list of selections items from the indexes given as input
    QList<UserSelection> getSelections(const QItemSelection &selection);

    // loads the data of the model from the given input
    void loadData(const QList<UserSelection> &selectionList);

    // reset current model
    void clear();

private:

    QList<UserSelection> m_userSelectionList;

    Q_DISABLE_COPY(UserSelectionsItemModel)
};

#endif // USERSELECTIONITEMMODEL_H

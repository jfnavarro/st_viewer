#ifndef USERSELECTIONITEMMODEL_H
#define USERSELECTIONITEMMODEL_H

#include <QtCore/QModelIndex>
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
        Name = 1,
        Dataset = 2,
        NGenes = 3,
        NReads = 4,
    };

    explicit UserSelectionsItemModel(QObject *parent = 0);
    virtual ~UserSelectionsItemModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // reset current model
    void clear();

    // loads the data of the model from the given input
    void loadUserSelections(const QList<UserSelection> &selectionList);

    // returns a list of selections items from the indexes given as input
    QList<UserSelection> getSelections(const QItemSelection &selection);

private:
    QList<UserSelection> m_userSelectionList;

    Q_DISABLE_COPY(UserSelectionsItemModel)
};

#endif // USERSELECTIONITEMMODEL_H

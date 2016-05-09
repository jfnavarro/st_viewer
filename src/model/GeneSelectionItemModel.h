#ifndef GENESELECTIONITEMMODEL_H
#define GENESELECTIONITEMMODEL_H

#include <QAbstractTableModel>
#include "dataModel/UserSelection.h"
#include "data/DataProxy.h"

class QModelIndex;
class QStringList;

// This is the data model for the SelectionsWidget
// where a table with gene names and aggregated count is shown.
// the data is obtained from an user selection
class GeneSelectionItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column { Name = 0, Count = 1 };

    explicit GeneSelectionItemModel(QObject *parent = 0);
    virtual ~GeneSelectionItemModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // load the selected items given as parameters into the model
    void loadSelectedGenes(const UserSelection::geneTotalCountsVector &geneSelectionList);

    // resets and clears the model
    void clear();

public slots:
    // TODO pass reference instead for genename
    // Used to sort the genes in the table
    bool geneName(const QModelIndex &index, QString *genename) const;

private:
    UserSelection::geneTotalCountsVector m_geneselection;

    Q_DISABLE_COPY(GeneSelectionItemModel)
};

#endif // GENESELECTIONITEMMODEL_H

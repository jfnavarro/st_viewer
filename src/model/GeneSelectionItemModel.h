#ifndef GENESELECTIONITEMMODEL_H
#define GENESELECTIONITEMMODEL_H

#include <QAbstractTableModel>
#include "dataModel/UserSelection.h"
#include "data/DataProxy.h"

class QModelIndex;
class QStringList;

// The User Selection window has an option to allow to show the genes
// present in the selection by aggregating them. This model
// is used in that view based on the class UserSelection::aggregatedGene
class GeneSelectionItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column { Name = 0, Count = 1, Hits = 2 };

    explicit GeneSelectionItemModel(QObject* parent = 0);
    virtual ~GeneSelectionItemModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // load the selected items given as parameters into the model
    void loadSelectedGenes(const UserSelection::selectedGenesList& geneSelectionList);

    // resets and clears the model
    void clear();

public slots:
    // TODO pass reference instead for genename
    // Used to sort the genes in the table
    bool geneName(const QModelIndex& index, QString* genename) const;

private:
    UserSelection::selectedGenesList m_geneselection;

    Q_DISABLE_COPY(GeneSelectionItemModel)
};

#endif // GENESELECTIONITEMMODEL_H

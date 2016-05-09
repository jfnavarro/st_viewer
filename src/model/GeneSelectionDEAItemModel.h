#ifndef GENESELECTIONDEAITEMMODEL_H
#define GENESELECTIONDEAITEMMODEL_H

#include <QAbstractTableModel>

#include "analysis/AnalysisDEA.h"

class QModelIndex;
class QStringList;
class QItemSelection;

// Wrapper model class for the selection table in the DEA window
// The DEA allows to compare the genes from two user selections, for that we
// need
// a new data model that merge the two selections.
// The DEA has a table to view the data.
class GeneSelectionDEAItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column { Name = 0, HitsA = 1, HitsB = 2 };

    explicit GeneSelectionDEAItemModel(QObject *parent = 0);
    virtual ~GeneSelectionDEAItemModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // load the selected items given as parameters into the model
    // the selected items would be gene, counts in selection A and counts in
    // selection B
    void loadCombinedSelectedGenes(const AnalysisDEA::combinedSelectionType &combinedSelections);

    // returns a list of selections items from the indexes given as input
    // the selected items would be gene, counts in selection A and counts in
    // selection B
    AnalysisDEA::combinedSelectionType getSelections(const QItemSelection &selection);

public slots:
    // TODO pass reference instead for genename
    // Used to sort the genes in the table
    bool geneName(const QModelIndex &index, QString *genename) const;

private:
    AnalysisDEA::combinedSelectionType m_combinedSelections;

    Q_DISABLE_COPY(GeneSelectionDEAItemModel)
};

#endif // GENESELECTIONDEAITEMMODEL_H

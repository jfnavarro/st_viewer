/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENESELECTIONDEAITEMMODEL_H
#define GENESELECTIONDEAITEMMODEL_H

#include <QAbstractTableModel>

#include "analysis/AnalysisDEA.h"

class QModelIndex;
class QStringList;
class QItemSelection;

// Wrapper model class for the selection table in the DEA window
// The DEA allows to compare the genes from two user selections, for that we need
// a new data model that merge the two selections.
// The DEA has a table to view the data.
class GeneSelectionDEAItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column { Name = 0, HitsA = 1, TPMA = 2, HitsB = 3, TPMB = 4 };

    explicit GeneSelectionDEAItemModel(QObject* parent = 0);
    virtual ~GeneSelectionDEAItemModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // load the selected items given as parameters into the model
    void loadCombinedSelectedGenes(const AnalysisDEA::combinedSelectionsType& combinedSelections);

    // returns a list of selections items from the indexes given as input
    AnalysisDEA::combinedSelectionsType getSelections(const QItemSelection& selection);

public slots:
    // TODO pass reference instead for genename
    // Used to sort the genes in the table
    bool geneName(const QModelIndex& index, QString* genename) const;

private:
    AnalysisDEA::combinedSelectionsType m_combinedSelections;

    Q_DISABLE_COPY(GeneSelectionDEAItemModel)
};

#endif // GENESELECTIONDEAITEMMODEL_H

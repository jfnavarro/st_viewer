/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTSITEMMODEL_H
#define EXPERIMENTSITEMMODEL_H

#include <QtCore/QModelIndex>

#include "dataModel/GeneSelection.h"

#include "data/DataProxy.h"

#include <QAbstractTableModel>

#include "dataModel/GeneSelection.h"

class QModelIndex;
class QStringList;
class QItemSelection;

// Wrapper model for the genes selections table
//TODO this should not be called experiment
class ExperimentsItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Name = 0,
        Dataset = 1,
        Comment = 2,
        NGenes = 3,
        NReads = 4,
        NFeatures = 5,
        Created = 6,
        LastModified = 7
    };

    explicit ExperimentsItemModel(QObject* parent = 0);
    virtual ~ExperimentsItemModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // reset current model
    void reset();

    // loads the data of the model from DataProxys
    void loadSelectedGenes(const DataProxy::GeneSelectionList selectionList);

    // returns a list of selections items from the indexes given as input
    DataProxy::GeneSelectionList getSelections(const QItemSelection &selection);

private:

    //TODO make this a pure reference
    DataProxy::GeneSelectionList m_geneselectionList;

    Q_DISABLE_COPY(ExperimentsItemModel)
};

#endif // EXPERIMENTSITEMMODEL_H

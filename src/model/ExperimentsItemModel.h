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

class ExperimentsItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Name = 0,
        Dataset = 1,
        Comment = 2,
    };

    explicit ExperimentsItemModel(QObject* parent = 0);
    virtual ~ExperimentsItemModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    //reset current model
    void reset();
    //loads the data of the model from DataProxys
    void loadSelectedGenes();

private:

    static const int COLUMN_NUMBER = 3;

    DataProxy::GeneSelectionList m_geneselectionList;

    Q_DISABLE_COPY(ExperimentsItemModel)
};

#endif // EXPERIMENTSITEMMODEL_H

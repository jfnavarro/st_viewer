/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETITEMMODEL_H
#define DATASETITEMMODEL_H

#include "controller/data/DataProxy.h"
#include <QAbstractItemModel>

class QModelIndex;
class QStandardItemModel;

// Wrapper model class for the dataset data in the data proxy. Provides an easy
// means of enumerating all the data sets connected to a single user.
class DatasetItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Name,
        Created,
        Tissue,
        Specie,
        Aligned,
        Barcodes,
        Genes,
        UBarcodes,
        UGenes,
        Comments
    };

    explicit DatasetItemModel(QObject* parent = 0);
    virtual ~DatasetItemModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    //NOTE do not want the user to edit columns name
    inline virtual bool setHeaderData(int, Qt::Orientation,
                                      const QVariant&, int = Qt::EditRole) { return false; }

public slots:

    void loadDatasets();
    void datasetSelected(const QModelIndex&);

signals:

    void datasetSelected(DataProxy::DatasetPtr);

private:

    static const int COLUMN_NUMBER = 10;
    DataProxy::DatasetListPtr m_datasets_reference;

    Q_DISABLE_COPY(DatasetItemModel);
};

#endif // DATASETITEMMODEL_H

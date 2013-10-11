/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEFEATUREITEMMODEL_H
#define GENEFEATUREITEMMODEL_H

#include <QModelIndex>
#include <QMimeData>
#include <QStringList>
#include "controller/data/DataProxy.h"

#include <QModelIndex>

// Wrapper model class for the gene data (specific to a dataset) found in the
// data proxy. Primarily used to enumerate the genes in the cell view.
class GeneFeatureItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)
    
public:
  
    enum Column {Name = 0, Show, Color};

    explicit GeneFeatureItemModel(QObject* parent = 0);
    virtual ~GeneFeatureItemModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void loadGenes(const QString& datasetid);

signals:
    void signalSelectionChanged(QSharedPointer<Gene> gene);
    void signalColorChanged(QSharedPointer<Gene> gene);

public slots:

    void selectAllGenesPressed(bool selected);
    void setColorGenes (const QColor& color);

private:
    
    static const QString MIMETYPE_APPGENELIST;
    static const int COLUMN_NUMBER = 3;
    
    QSharedPointer<DataProxy::GeneList> genelist; //NOTE member function so we dont need to call getDatasetsbyID all the time
};

#endif // GENEFEATUREITEMMODEL_H

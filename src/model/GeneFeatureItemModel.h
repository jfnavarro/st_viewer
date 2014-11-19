/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEFEATUREITEMMODEL_H
#define GENEFEATUREITEMMODEL_H

#include "data/DataProxy.h"

#include <QAbstractTableModel>

class QModelIndex;
class QStringList;
class QMimeData;
class QItemSelection;

// Wrapper model class for the gene data (specific to a dataset) found in the
// data proxy. Primarily used to enumerate the genes in the cell view.
class GeneFeatureItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Show = 0,
        Name = 1,
        Color = 2
    };

    explicit GeneFeatureItemModel(QObject* parent = 0);
    virtual ~GeneFeatureItemModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    //this function will set to visible the genes included in the selection
    //and emit a signal with the modified genes
    void setGeneVisibility(const QItemSelection &selection, bool visible);

    //this function will modify the color of the genes included in the selection
    //and emit a signal with the modified genes
    void setGeneColor(const QItemSelection &selection, const QColor& color);

    //reload the reference to the genes from DataProxy
    void loadGenes(const DataProxy::GeneList &geneList);

    //clear and reset the model
    void clearGenes();

public slots:
    //TODO pass reference instead for genename
    bool geneName(const QModelIndex &index, QString *genename) const;

signals:

    void signalSelectionChanged(DataProxy::GeneList geneList);
    void signalColorChanged(DataProxy::GeneList geneList);

private:

    //TODO make this a pure reference
    DataProxy::GeneList m_genelist_reference;

    Q_DISABLE_COPY(GeneFeatureItemModel)
};

#endif // GENEFEATUREITEMMODEL_H

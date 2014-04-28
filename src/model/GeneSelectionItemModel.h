/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENESELECTIONITEMMODEL_H
#define GENESELECTIONITEMMODEL_H

#include <QAbstractTableModel>

#include "viewOpenGL/GeneRendererGL.h"

class QModelIndex;
class QStringList;

class GeneSelectionItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:

    enum Column {
        Name = 0,
        Hits = 1,
        NormalizedHits = 2,
    };
    explicit GeneSelectionItemModel(QObject* parent = 0);
    virtual ~GeneSelectionItemModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:

    void reset();
    void loadSelectedGenes(GeneRendererGL::GeneSelectedSet);

private:

    static const int COLUMN_NUMBER = 3;

    GeneRendererGL::GeneSelectedSet m_geneselection;

    Q_DISABLE_COPY(GeneSelectionItemModel)
};

#endif // GENESELECTIONITEMMODEL_H

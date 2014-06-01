/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENENAMESMODEL_H
#define GENENAMESMODEL_H

#include <QAbstractTableModel>

class QModelIndex;
class QString;

//abstract class for Gene Based Models
//that provides a function to retrieve the gene name
//from a model index, this function is used to sort
//the genes in a proxy model

//TODO this class could be removed and use
//QT_INVOKE_METHOD on each model when needed
class GeneNamesModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit GeneNamesModel(QObject* parent = 0);
    virtual ~GeneNamesModel();

    virtual bool geneName(const QModelIndex &index, QString *genename) const = 0;

private:

    Q_DISABLE_COPY(GeneNamesModel)
};

#endif // GENENAMESMODEL_H

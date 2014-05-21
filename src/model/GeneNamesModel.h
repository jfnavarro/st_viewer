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

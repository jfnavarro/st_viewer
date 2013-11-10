/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTSITEMMODEL_H
#define EXPERIMENTSITEMMODEL_H

#include <QtCore/QModelIndex>

#include "model/UserExperiment.h"
#include "controller/data/DataProxy.h"

// Skeleton wrapper model class intended to provide access to experiment data
// in the data proxy. Needs implementing.
class ExperimentsItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit ExperimentsItemModel(QObject* parent = 0);

    Q_DISABLE_COPY(ExperimentsItemModel);

};

#endif // EXPERIMENTSITEMMODEL_H

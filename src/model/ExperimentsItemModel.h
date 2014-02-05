/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTSITEMMODEL_H
#define EXPERIMENTSITEMMODEL_H

#include <QtCore/QModelIndex>

#include "dataModel/UserExperiment.h"
<<<<<<< HEAD
#include "core/data/DataProxy.h"
=======
#include "data/DataProxy.h"
>>>>>>> 39bfdff81cd1108639383d1d72dd0dc33bfb4925

// Skeleton wrapper model class intended to provide access to experiment data
// in the data proxy. Needs implementing.
class ExperimentsItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit ExperimentsItemModel(QObject* parent = 0);

    Q_DISABLE_COPY(ExperimentsItemModel)

};

#endif // EXPERIMENTSITEMMODEL_H

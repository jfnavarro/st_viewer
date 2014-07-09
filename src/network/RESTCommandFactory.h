/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef RESTMANAGER_H
#define RESTMANAGER_H

#include "network/NetworkCommand.h"

class Configuration;

// RESTCommandFactory provides a convenient way of generating preconfigured
// network commands for specific REST actions. Each data request is represented
// as a one or more functions.
class RESTCommandFactory
{

public:

    // authentication commands
    //static NetworkCommand* getAuthorization();
    static NetworkCommand* getAuthorizationToken(const Configuration &configuration);

    // data access commands
    static NetworkCommand* getChipByChipId(const Configuration &configuration,
                                           const QString& chipId);

    static NetworkCommand* getDatasets(const Configuration& configuration);
    static NetworkCommand* getDatasetByDatasetId(const Configuration& configuration,
                                                 const QString& datasetId);
    static NetworkCommand* updateDatsetByDatasetId(const Configuration &configuration,
                                                   const QString& datasetId);

    static NetworkCommand* getGenesByDatasetId(const Configuration& configuration,
                                               const QString& datasetId);

    static NetworkCommand* getFeatureByDatasetId(const Configuration &configuration,
                                                 const QString& datasetId);
    static NetworkCommand* getFeatureByDatasetIdAndGene(const Configuration& configuration,
                                                        const QString& datasetId,
                                                        const QString& gene);

    static NetworkCommand* getImageAlignmentById(const Configuration &configuration,
                                                 const QString& imageAlignmentId);

    static NetworkCommand* getCellTissueFigureByName(const Configuration& configuration,
                                                     const QString& name);

    static NetworkCommand* getUser(const Configuration& configuration);
    //static NetworkCommand* updateUserbyUserId(const Configuration& configuration, const QString &userId);

    static NetworkCommand* getSelections(const Configuration& configuration);
    //static NetworkCommand* getSelectionsByDatasetId(const Configuration& configuration, const QString& datasetId);
    //static NetworkCommand* upateSelectionBySelectionId(const Configuration& configuration, const QString& selectionId);
    static NetworkCommand* removeSelectionBySelectionId(const Configuration& configuration,
                                                        const QString& selectionId);
    static NetworkCommand* addSelection(const Configuration& configuration);

    //NOTE this end point does not need authorization
    static NetworkCommand* getMinVersion(const Configuration& configuration);

};

#endif // RESTMANAGER_H //

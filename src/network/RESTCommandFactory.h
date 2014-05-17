/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef RESTMANAGER_H
#define RESTMANAGER_H

#include "network/NetworkCommand.h"

// RESTCommandFactory provides a convenient way of generating preconfigured
// network commands for specific REST actions. Each data request is represented
// as a one or more functions.
class RESTCommandFactory
{

public:

    // authentication commands
    static NetworkCommand* getAuthorization();
    static NetworkCommand* getAuthorizationToken();

    // data access commands
    static NetworkCommand* getChipByChipId(const QString& chipId);

    static NetworkCommand* getDatasets();
    static NetworkCommand* getDatasetByDatasetId(const QString& datasetId);
    static NetworkCommand* updateDatsetByDatasetId(const QString& datasetId);

    static NetworkCommand* getGenesByDatasetId(const QString& datasetId);

    static NetworkCommand* getFeatureByDatasetId(const QString& datasetId);
    static NetworkCommand* getFeatureByDatasetIdAndGene(const QString& datasetId,
                                                        const QString& gene);

    static NetworkCommand* getImageAlignmentById(const QString& imageAlignmentId);

    static NetworkCommand* getCellTissueFigureByName(const QString& name);

    static NetworkCommand* getUser();

    static NetworkCommand* getSelections();
    static NetworkCommand* getSelectionsByDatasetId(const QString& datasetId);
    static NetworkCommand* upateSelectionBySelectionId(const QString& selectionId);
    static NetworkCommand* addSelection();

    //NOTE this end point does not need authorization
    static NetworkCommand* getMinVersion();
};

#endif // RESTMANAGER_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "RESTCommandFactory.h"
#include "config/Configuration.h"

NetworkCommand* RESTCommandFactory::getAuthorizationToken()
{
    Configuration config;
    const QUrl endpoint = QUrl(config.oauthEndpointToken());
    NetworkCommand* cmd = new NetworkCommand(endpoint, Globals::HttpRequestTypePost);
    cmd->addQueryItem(Globals::LBL_GRANT_TYPE, Globals::LBL_GRANT_PASSWORD);
    cmd->addQueryItem(Globals::LBL_SCOPE, config.oauthScope());
    return cmd;
}

NetworkCommand* RESTCommandFactory::getChipByChipId(const QString& chipId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointChips() + "/" + chipId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getDatasets()
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointDatasets());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getDatasetByDatasetId(const QString& datasetId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::updateDatsetByDatasetId(const QString& datasetId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypePut);
}

NetworkCommand*
RESTCommandFactory::getGenesByDatasetId(const QString& datasetId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointGenes());
    NetworkCommand* cmd = new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
    cmd->addQueryItem("dataset", datasetId);
    return cmd;
}

NetworkCommand*
RESTCommandFactory::getFeatureByDatasetId(const QString& datasetId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointFeatures());
    NetworkCommand* cmd = new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
    cmd->addQueryItem("dataset", datasetId);
    return cmd;
}

NetworkCommand*
RESTCommandFactory::getFeatureByDatasetIdAndGene(
        const QString& datasetId, const QString& gene)
{
    NetworkCommand* cmd = RESTCommandFactory::getFeatureByDatasetId(datasetId);
    cmd->addQueryItem("gene", gene);
    return cmd;
}

NetworkCommand*
RESTCommandFactory::getImageAlignmentById(const QString& imageAlignmentId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointImageAlingment() + "/" + imageAlignmentId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getCellTissueFigureByName(const QString& name)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointFigures() + "/" + name);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getUser()
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointUsers());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getSelections()
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointSelections());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::removeSelectionBySelectionId(const QString &selectionId)
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointSelections() + "/" + selectionId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeDelete);
}

NetworkCommand* RESTCommandFactory::addSelection()
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointSelections());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypePost);
}

NetworkCommand* RESTCommandFactory::getMinVersion()
{
    Configuration config;
    QUrl endpoint = QUrl(config.dataEndpointMinVersion());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

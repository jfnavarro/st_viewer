/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "RESTCommandFactory.h"

#include <config/Configuration.h>

NetworkCommand* RESTCommandFactory::getAuthorizationToken()
{
    Configuration* config = Configuration::getInstance();
    QUrl endpoint = QUrl(config->oauthEndpointToken());
    NetworkCommand* cmd = new NetworkCommand(endpoint, Globals::HttpRequestTypePost);
    cmd->addQueryItem(Globals::LBL_GRANT_TYPE, Globals::LBL_GRANT_PASSWORD);
    cmd->addQueryItem(Globals::LBL_SCOPE, config->oauthScope());
    return cmd;
}

NetworkCommand* RESTCommandFactory::getChips()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointChips());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getChipByChipId(const QString& chipId)
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointChips() + "/" + chipId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getDatasets()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointDatasets());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getDatasetByDatasetId(const QString& datasetId)
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::updateDatsetByDatasetId(const QString& datasetId)
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypePut);
}

NetworkCommand* RESTCommandFactory::getGenes()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointGenes());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getGenesByDatasetId(const QString& datasetId)
{
    NetworkCommand* cmd = RESTCommandFactory::getGenes(); // call base endpoint
    cmd->addQueryItem("dataset", datasetId);              // with dataset parameter
    return cmd;
}

NetworkCommand* RESTCommandFactory::getFeatures()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointFeatures());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}
NetworkCommand* RESTCommandFactory::getFeatureByDatasetId(const QString& datasetId)
{
    NetworkCommand* cmd = RESTCommandFactory::getFeatures(); // call base endpoint
    cmd->addQueryItem("dataset", datasetId);                 // with dataset parameter
    return cmd;
}

NetworkCommand* RESTCommandFactory::getFeatureByDatasetIdAndGene(const QString& datasetId, const QString& gene)
{
    NetworkCommand* cmd = RESTCommandFactory::getFeatureByDatasetId(datasetId); // call by-dataset endpoint
    cmd->addQueryItem("gene", gene);                                            // with additional gene parameter
    return cmd;
}

NetworkCommand* RESTCommandFactory::getHitCounts()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointHitCounts());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getHitCountByDatasetId(const QString& datasetId)
{
    NetworkCommand* cmd = RESTCommandFactory::getHitCounts(); // call base endpoint
    cmd->addQueryItem("dataset", datasetId);                  // with dataset parameter
    return cmd;
}

NetworkCommand* RESTCommandFactory::getCellTissueFigureByName(const QString& name)
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointFigures() + "/" + name);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getUser()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointUsers());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getSelections()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointUsers());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getMinVersion()
{
    QUrl endpoint = QUrl(Configuration::getInstance()->dataEndpointMinVersion());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

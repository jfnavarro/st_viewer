/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QUuid>

#include "RESTCommandFactory.h"
#include "config/Configuration.h"

NetworkCommand* RESTCommandFactory::getAuthorizationToken(const Configuration& configuration)
{
    const QUrl endpoint = QUrl(configuration.oauthEndpointToken());
    NetworkCommand* cmd = new NetworkCommand(endpoint, Globals::HttpRequestTypePost);
    cmd->addQueryItem(Globals::LBL_GRANT_TYPE, Globals::LBL_GRANT_PASSWORD);
    cmd->addQueryItem(Globals::LBL_SCOPE, configuration.oauthScope());
    return cmd;
}

NetworkCommand* RESTCommandFactory::getChipByChipId(const Configuration& configuration,
                                                    const QString& chipId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointChips() + "/" + chipId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getDatasets(const Configuration& configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getDatasetByDatasetId(const Configuration& configuration,
                                          const QString& datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::updateDatasetByDatasetId(const Configuration& configuration,
                                            const QString& datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypePut);
}

NetworkCommand*
RESTCommandFactory::removeDatasetByDatasetId(const Configuration& configuration,
                                             const QString& datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeDelete);
}


NetworkCommand*
RESTCommandFactory::getFeatureByDatasetId(const Configuration& configuration,
                                          const QString& datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointFeatures() + "/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getImageAlignmentById(const Configuration& configuration,
                                          const QString& imageAlignmentId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointImageAlingment() + "/" + imageAlignmentId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getCellTissueFigureByName(const Configuration& configuration,
                                              const QString& name)
{
    QUrl endpoint = QUrl(configuration.dataEndpointFigures() + "/" + name);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getUser(const Configuration& configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointUsers());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getSelections(const Configuration& configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::upateSelectionBySelectionId(const Configuration& configuration,
                                                                const QString& selectionId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections() + "/" + selectionId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypePut);
}

NetworkCommand* RESTCommandFactory::removeSelectionBySelectionId(const Configuration& configuration,
                                                                 const QString& selectionId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections() + "/" + selectionId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeDelete);
}

NetworkCommand* RESTCommandFactory::addSelection(const Configuration& configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypePost);
}

NetworkCommand* RESTCommandFactory::getMinVersion(const Configuration& configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointMinVersion());
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getImageLastModified(const Configuration &configuration,
                                                         const QString &imageFileName)
{
    QUrl endpoint = QUrl(configuration.dataEndpointFiguresLastModified() + "/" + imageFileName);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand* RESTCommandFactory::getFeaturesLastModified(const Configuration &configuration,
                                                            const QString &featureFileName)
{
    QUrl endpoint = QUrl(configuration.dataEndpointFeatures() + "/lastmodified/" + featureFileName);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getDatasetLastModified(const Configuration &configuration,
                                           const QString &datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/lastmodified/" + datasetId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getImageAlignmentLastModified(const Configuration &configuration,
                                                  const QString &imageAlignmentId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointImageAlingment() + "/lastmodified/" + imageAlignmentId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}

NetworkCommand*
RESTCommandFactory::getChipLastModified(const Configuration &configuration,
                                        const QString &chipId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointChips() + "/lastmodified/" + chipId);
    return new NetworkCommand(endpoint, Globals::HttpRequestTypeGet);
}


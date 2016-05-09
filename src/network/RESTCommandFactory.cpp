#include "RESTCommandFactory.h"

#include <QUuid>
#include "config/Configuration.h"
#include "SettingsNetwork.h"

using namespace Network;

QSharedPointer<NetworkCommand> RESTCommandFactory::getAuthorizationToken(
    const Configuration &configuration)
{
    const QUrl endpoint = QUrl(configuration.oauthEndpointToken());
    QSharedPointer<NetworkCommand> cmd
        = QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypePost));
    cmd->addQueryItem(LBL_GRANT_TYPE, LBL_GRANT_PASSWORD);
    cmd->addQueryItem(LBL_SCOPE, configuration.oauthScope());
    return cmd;
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getChipByChipId(
    const Configuration &configuration, const QString &chipId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointChips() + "/" + chipId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getDatasets(const Configuration &configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets());
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getDatasetByDatasetId(
    const Configuration &configuration, const QString &datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/" + datasetId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::updateDatasetByDatasetId(
    const Configuration &configuration, const QString &datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/" + datasetId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypePut));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::removeDatasetByDatasetId(
    const Configuration &configuration, const QString &datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointDatasets() + "/" + datasetId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeDelete));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getFeatureByDatasetId(
    const Configuration &configuration, const QString &datasetId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointFeatures() + "/" + datasetId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getImageAlignmentById(
    const Configuration &configuration, const QString &imageAlignmentId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointImageAlingment() + "/" + imageAlignmentId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getCellTissueFigureByName(
    const Configuration &configuration, const QString &name)
{
    QUrl endpoint = QUrl(configuration.dataEndpointFigures() + "/" + name);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getUser(const Configuration &configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointUsers());
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getSelections(const Configuration &configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections());
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::upateSelectionBySelectionId(
    const Configuration &configuration, const QString &selectionId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections() + "/" + selectionId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypePut));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::removeSelectionBySelectionId(
    const Configuration &configuration, const QString &selectionId)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections() + "/" + selectionId);
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeDelete));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::addSelection(const Configuration &configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointSelections());
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypePost));
}

QSharedPointer<NetworkCommand> RESTCommandFactory::getMinVersion(const Configuration &configuration)
{
    QUrl endpoint = QUrl(configuration.dataEndpointMinVersion());
    return QSharedPointer<NetworkCommand>(new NetworkCommand(endpoint, HttpRequestTypeGet));
}

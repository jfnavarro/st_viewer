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
    static QSharedPointer<NetworkCommand> getAuthorizationToken(const Configuration &configuration);

    // data access commands
    static QSharedPointer<NetworkCommand> getChipByChipId(const Configuration &configuration,
                                                          const QString &chipId);

    static QSharedPointer<NetworkCommand> getDatasets(const Configuration &configuration);
    static QSharedPointer<NetworkCommand> getDatasetByDatasetId(const Configuration &configuration,
                                                                const QString &datasetId);
    static QSharedPointer<NetworkCommand> updateDatasetByDatasetId(
        const Configuration &configuration, const QString &datasetId);
    static QSharedPointer<NetworkCommand> removeDatasetByDatasetId(
        const Configuration &configuration, const QString &datasetId);

    static QSharedPointer<NetworkCommand> getFeatureByDatasetId(const Configuration &configuration,
                                                                const QString &datasetId);

    static QSharedPointer<NetworkCommand> getImageAlignmentById(const Configuration &configuration,
                                                                const QString &imageAlignmentId);

    static QSharedPointer<NetworkCommand> getCellTissueFigureByName(
        const Configuration &configuration, const QString &name);

    static QSharedPointer<NetworkCommand> getUser(const Configuration &configuration);
    // static NetworkCommand* updateUserbyUserId(const Configuration&
    // configuration, const QString
    // &userId);

    static QSharedPointer<NetworkCommand> getSelections(const Configuration &configuration);
    static QSharedPointer<NetworkCommand> upateSelectionBySelectionId(
        const Configuration &configuration, const QString &selectionId);
    static QSharedPointer<NetworkCommand> addSelection(const Configuration &configuration);
    static QSharedPointer<NetworkCommand> removeSelectionBySelectionId(
        const Configuration &configuration, const QString &selectionId);
    // NOTE this end point does not need authorization
    static QSharedPointer<NetworkCommand> getMinVersion(const Configuration &configuration);
};

#endif // RESTMANAGER_H //

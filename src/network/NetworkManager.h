#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QVariant>
#include <QFlags>
#include <QPointer>

#include "config/Configuration.h"
#include "auth/TokenStorage.h"

class NetworkCommand;
class NetworkReply;
class QNetworkReply;
class Error;
class NetworkDiskCache;

// Class used to manage all network related functionalities. Creates
// an abstract layer to easily send network requests.
class NetworkManager : public QObject
{
    Q_OBJECT
    Q_FLAGS(NetworkFlags)

public:
    enum NetworkFlag {
        Empty,
        UseAuthentication,
        UseCache,
        UsePipelineMode,
        UseHighPriority,
        UseTimeOutAbort,
        Default = UseAuthentication | UseCache | UsePipelineMode | UseHighPriority | UseTimeOutAbort
    };
    Q_DECLARE_FLAGS(NetworkFlags, NetworkFlag)

    NetworkManager(QObject* parent = 0);
    virtual ~NetworkManager();

    // default use Authentication
    NetworkReply* httpRequest(NetworkCommand* cmd, NetworkFlags flags = NetworkFlag::Default);

    // clear network disk cache
    void cleanCache();

private slots:
    // if remote server requires authentication
    void provideAuthentication(QNetworkReply*, QAuthenticator*);

private:
    // internal function to add the JSON data info to the headers of the request
    // and returns the JSON data as a QbyteArray. JSON data is present in NetworkCommand
    QByteArray addJSONDatatoRequest(NetworkCommand* cmd, QNetworkRequest& request) const;

    // qt network manager object
    QPointer<QNetworkAccessManager> m_nam;
    // configuration manager instance
    Configuration m_configurationManager;
    // instance of token manager to do authorization when requested
    TokenStorage m_tokenStorage;
    // network disk cache
    QPointer<NetworkDiskCache> m_diskCache;

    Q_DISABLE_COPY(NetworkManager)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NetworkManager::NetworkFlags)

#endif // NETWORKMANAGER_H //

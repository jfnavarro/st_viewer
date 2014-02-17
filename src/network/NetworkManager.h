/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QVariant>
#include <QFlags>
#include <QPointer>

#include "utils/Singleton.h"

class NetworkCommand;
class NetworkReply;
class QNetworkReply;
class Error;

// Singleton class used to manage all network related functionality. Creates
// an abstract layer to easily send network requests.
class NetworkManager : public QObject, public Singleton<NetworkManager>
{
    Q_OBJECT
    Q_FLAGS(NetworkFlags)

public:

    enum NetworkFlag {
        Empty = 0x0,
        UseAuthentication = 0x1,
        UseCache = 0x2,
        UsePipelineMode = 0x4,
        UseHighPriority = 0x8
    };
    Q_DECLARE_FLAGS(NetworkFlags, NetworkFlag)

    explicit NetworkManager(QObject* parent = 0);
    virtual ~NetworkManager();

    void init();
    void finalize();

    //default use Authentication
    NetworkReply* httpRequest(NetworkCommand* cmd, QVariant data = QVariant(),
                              NetworkFlags flags = UseAuthentication);

private slots:
    //if remote server requires authentication
    void provideAuthentication(QNetworkReply*, QAuthenticator*);

private:

    QPointer<QNetworkAccessManager> m_nam;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(NetworkManager::NetworkFlags)

#endif // NETWORKMANAGER_H //

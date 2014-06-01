/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkManager.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>
#include <QNetworkConfigurationManager>
#include <QNetworkProxy>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QSslConfiguration>
#include <QFile>
#include <QAuthenticator>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QHostInfo>

#include "NetworkCommand.h"
#include "NetworkReply.h"
#include "auth/AuthorizationManager.h"
#include "error/Error.h"
#include "config/Configuration.h"

NetworkManager::NetworkManager(QObject* parent):
    QObject(parent),
    m_nam(nullptr)
{
    // setup network access manager
    m_nam = QPointer<QNetworkAccessManager>(new QNetworkAccessManager(this));

#if defined Q_OS_MAC
    //workaround for this : https://bugreports.qt-project.org/browse/QTBUG-22033
    QNetworkProxy proxy = m_nam->proxy();
    proxy.setHostName(" ");
    m_nam->setProxy(proxy);
#endif

    Configuration config;

    // make DND look up ahead of time
    QHostInfo::lookupHost(config.EndPointUrl(), 0, 0);

    // connect to the HTTPS TCP port ahead of time
    m_nam->connectToHostEncrypted(config.EndPointUrl());

    // add ssl support  (we need the public key)
    //TODO finish and try this (me dont like ignoring ssl errors)
    //QFile cafile(":public_key.pem");
    //cafile.open(QIODevice::ReadOnly);
    //QSslCertificate cert(&cafile);
    //QSslSocket::addDefaultCaCertificate(cert);

    // add cache support (not really useful for downloading data)
    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    diskCache->setCacheDirectory(location);
    diskCache->setMaximumCacheSize(1000 * 1024 * 1024); // 1GB
    m_nam->setCache(diskCache);

    //we want to provide Authentication to our oAuth based servers
    connect(m_nam, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
            SLOT(provideAuthentication(QNetworkReply*, QAuthenticator*)));
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::provideAuthentication(QNetworkReply *reply,
                                           QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    Configuration config;
    authenticator->setUser(config.oauthClientID());
    authenticator->setPassword(config.oauthSecret());
}

NetworkReply* NetworkManager::httpRequest(NetworkCommand* cmd,
                                          QVariant data, NetworkFlags flags)
{
    // early out
    if (cmd == nullptr) {
        qDebug() << "[NetworkManager] Error: Unable to create Network Command";
        return nullptr;
    }

    // append authentication token to network command
    AuthorizationManager *authorizationManager = AuthorizationManager::getInstance();
    if (flags.testFlag(UseAuthentication) && authorizationManager->hasAccessToken()) {
        //TODO hide auth from network manager by appending QObject that encapsulates the key value pair
        const QUuid accessToken = authorizationManager->getAccessToken();
        cmd->addQueryItem(QStringLiteral("access_token"),
                          accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"...
    }

    // keep track of reply to match command later on (async callback)
    QNetworkReply* networkReply = nullptr;

    // creating the request
    QNetworkRequest request;

    // add caching to request (only if network caching is active)
    if (flags.testFlag(UseCache)) {
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                         QNetworkRequest::PreferCache);
    }

    // add pipeline to the request
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,
                         flags.testFlag(UsePipelineMode));

    // add high priority
    if (flags.testFlag(UseHighPriority)) {
        request.setPriority(QNetworkRequest::HighPriority);
    }

    switch (cmd->type()) {
        case Globals::HttpRequestTypeGet: {
            // encode query as part of the url
            QUrl queryUrl(cmd->url());
            queryUrl.setQuery(cmd->query());
            request.setUrl(queryUrl);
            // show debug text
            qDebug() << "[NetworkManager] GET:" << request.url();
            // send request
            networkReply = m_nam->get(request);
            break;
        }
        case Globals::HttpRequestTypePost: {
            // set clean url and explicit content type
            request.setUrl(cmd->url());
            // show debug text
            qDebug() << "[NetworkManager] POST:" << request.url() << "DATA:" << cmd->getEncodedQuery();
            //setting headers
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QVariant(QStringLiteral("application/x-www-form-urlencoded")));
            // send request
            networkReply = m_nam->post(request, cmd->getEncodedQuery().toUtf8());
            break;
        }
        case Globals::HttpRequestTypePut: {
            // set clean url and explicit content type
            request.setUrl(cmd->url());
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QVariant(QStringLiteral("application/x-www-form-urlencoded")));
            // show debug text
            qDebug() << "[NetworkManager] PUT:" << request.url() << "DATA:" << cmd->getEncodedQuery();
            // send request
            networkReply = m_nam->put(request, cmd->getEncodedQuery().toUtf8());
            break;
        }
        // if not set or unknown error
        case Globals::HttpRequestTypeNone: {
            qDebug() << "[NetworkManager] Error: Unkown request type";
            break;
        }
        default:
            qDebug() << "[NetworkManager] Error: Unkown network command!";
    }

    if (networkReply == nullptr) {
        qDebug() << "[NetworkManager] Error: NetWork reply is null!!";
        return nullptr;
    }

    NetworkReply* replyWrapper = nullptr;
    replyWrapper = new NetworkReply(networkReply);
    replyWrapper->setCustomData(data);

    if (replyWrapper == nullptr) {
        // something didn't work out :/
        qDebug() << "[NetworkManager] Error: Unable to create network request!";
        return nullptr;
    }

    return replyWrapper;
}

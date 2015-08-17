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
#include <QDir>
#include <QUuid>

#include "NetworkCommand.h"
#include "NetworkReply.h"
#include "NetworkDiskCache.h"
#include "error/Error.h"

NetworkManager::NetworkManager(QObject *parent):
    QObject(parent),
    m_nam(nullptr),
    m_diskCache(nullptr)
{
    // setup network access manager
    m_nam = new QNetworkAccessManager(this);

    // add ssl support
    QFile cafile(":public_key.pem");
    cafile.open(QIODevice::ReadOnly);
    QSslCertificate cert(&cafile);
    QSslSocket::addDefaultCaCertificate(cert);

    const QString serverURL = m_configurationManager.EndPointUrl();
    // make DND look up ahead of time
    QHostInfo::lookupHost(serverURL, 0, 0);
    // connect to the HTTPS TCP port ahead of time
    m_nam->connectToHostEncrypted(serverURL);

    // add cache support
    m_diskCache = new NetworkDiskCache(this);
    Q_ASSERT(!m_diskCache.isNull());
    QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    qDebug() << "Network disk cache location " << location;
    m_diskCache->setCacheDirectory(location + QDir::separator() + "data");
    const quint64 cacheSizeinGB = 5368709120; // 1024*1024*1024*5 5GB
    //TODO some HD space check should be added here
    m_diskCache->setMaximumCacheSize(cacheSizeinGB);
    m_nam->setCache(m_diskCache);

    // we want to provide Authentication to our OAuth based servers
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
    authenticator->setUser(m_configurationManager.oauthClientID());
    authenticator->setPassword(m_configurationManager.oauthSecret());
}

NetworkReply* NetworkManager::httpRequest(NetworkCommand *cmd, NetworkFlags flags)
{
    // early out
    if (cmd == nullptr) {
        qDebug() << "[NetworkManager] Error: Unable to create Network Command";
        return nullptr;
    }

    // do a connection check here (TODO seems to only work in MAC)
#ifdef Q_OS_MAC
    if (m_nam->networkAccessible() == QNetworkAccessManager::NotAccessible) {
        qDebug() << "[NetworkManager] Error: Unable to connect to the network";
        return nullptr;
    }
#endif

    // check if authentication is needed
    if (flags.testFlag(UseAuthentication)) {
        // append access token
        const QUuid accessToken = m_tokenStorage.getAccessToken();
        Q_ASSERT(!accessToken.isNull());
        // QUuid encloses its uuids in "{}"
        cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    }

    // create the qt network request
    QNetworkRequest request;

    // add caching to request
    if (flags.testFlag(UseCache)) {
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                             QNetworkRequest::PreferCache);
        request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    }

    // add pipeline option to the request
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute,
                         flags.testFlag(UsePipelineMode));

    // add high priority
    if (flags.testFlag(UseHighPriority)) {
        request.setPriority(QNetworkRequest::HighPriority);
    }

    // keep track of reply to match command later on (async callback)
    QNetworkReply *networkReply = nullptr;

    // encode query as part of the url in the request
    QUrl queryUrl(cmd->url());
    queryUrl.setQuery(cmd->query());
    request.setUrl(queryUrl);

    switch (cmd->type()) {
    case Globals::HttpRequestTypeGet: {
        qDebug() << "[NetworkManager] GET:" << request.url();
        networkReply = m_nam->get(request);
        break;
    } case Globals::HttpRequestTypePost: {
        qDebug() << "[NetworkManager] POST:" << request.url();
        // POST methods need a special request header
        QByteArray jsonData = addJSONDatatoRequest(cmd, request);
        networkReply = m_nam->post(request, jsonData);
        break;
    } case Globals::HttpRequestTypePut: {
        qDebug() << "[NetworkManager] PUT:" << request.url();
        // PUT methods need a special request header
        QByteArray jsonData = addJSONDatatoRequest(cmd, request);
        networkReply = m_nam->put(request, jsonData);
        break;
    } case Globals::HttpRequestTypeDelete: {
        qDebug() << "[NetworkManager] DELETE: " << request.url();
        networkReply = m_nam->deleteResource(request);
        break;
    } case Globals::HttpRequestTypeNone: {
        qDebug() << "[NetworkManager] Error: Unkown request type";
        break;
    } default:
        qDebug() << "[NetworkManager] Error: Unkown network command type";
    }

    if (networkReply == nullptr) {
        qDebug() << "[NetworkManager] Error: network reply is null";
        return nullptr;
    }

    NetworkReply *replyWrapper = new NetworkReply(networkReply);
    if (replyWrapper == nullptr) {
        qDebug() << "[NetworkManager] Error: Unable to create network request";
        return nullptr;
    }

    return replyWrapper;
}

QByteArray NetworkManager::addJSONDatatoRequest(NetworkCommand *cmd,
                                                QNetworkRequest &request) const
{
    QByteArray jsonData = cmd->body();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, jsonData.size());
    return jsonData;
}

void NetworkManager::cleanCache()
{
    m_diskCache->clear();
}

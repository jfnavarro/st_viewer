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

#include "error/Error.h"

NetworkManager::NetworkManager(const Configuration &configurationManager, QObject *parent):
    QObject(parent),
    m_nam(nullptr),
    m_configurationManager(configurationManager)
{
    // setup network access manager
    m_nam = new QNetworkAccessManager(this);

#if defined Q_OS_MAC
    //workaround for this : https://bugreports.qt-project.org/browse/QTBUG-22033
    //it is not working for HTTPS sites even though they say it is fixed... :(
    QNetworkProxy proxy = m_nam->proxy();
    proxy.setHostName(" ");
    m_nam->setProxy(proxy);
#endif

    // make DND look up ahead of time
    QHostInfo::lookupHost(m_configurationManager.EndPointUrl(), 0, 0);

    // connect to the HTTPS TCP port ahead of time
    m_nam->connectToHostEncrypted(m_configurationManager.EndPointUrl());

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
    m_nam->deleteLater();
    m_nam = nullptr;
}

void NetworkManager::provideAuthentication(QNetworkReply *reply,
                                           QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    authenticator->setUser(m_configurationManager.oauthClientID());
    authenticator->setPassword(m_configurationManager.oauthSecret());
}

NetworkReply* NetworkManager::httpRequest(NetworkCommand *cmd,
                                          QVariant data, NetworkFlags flags)
{
    // early out
    if (cmd == nullptr) {
        qDebug() << "[NetworkManager] Error: Unable to create Network Command";
        return nullptr;
    }

    // check if accces_token is appended
    if (flags.testFlag(UseAuthentication) && cmd->getQueryItem("access_token").isEmpty()) {
        qDebug() << "[NetworkManager] Error: Access token is empty";
        return nullptr;
    }

    // create the qt network request
    QNetworkRequest request;

    // add caching to request
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

    // keep track of reply to match command later on (async callback)
    QNetworkReply *networkReply = nullptr;

    switch (cmd->type()) {
    case Globals::HttpRequestTypeGet: {
        // encode query as part of the url
        QUrl queryUrl(cmd->url());
        queryUrl.setQuery(cmd->query());
        request.setUrl(queryUrl);
        qDebug() << "[NetworkManager] GET:" << request.url();
        // send request
        networkReply = m_nam->get(request);
        break;
    } case Globals::HttpRequestTypePost: {
        // encode query as part of the url
        QUrl queryUrl(cmd->url());
        queryUrl.setQuery(cmd->query());
        request.setUrl(queryUrl);
        // get json data
        QByteArray jsonData = cmd->jsonQuery();
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader, jsonData.size());
        qDebug() << "[NetworkManager] POST:" << request.url();
        // send request
        networkReply = m_nam->post(request, jsonData);
        break;
    } case Globals::HttpRequestTypePut: {
        // encode query as part of the url
        QUrl queryUrl(cmd->url());
        queryUrl.setQuery(cmd->query());
        request.setUrl(queryUrl);
        // get json data
        QByteArray jsonData = cmd->jsonQuery();
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader, jsonData.size());
        qDebug() << "[NetworkManager] PUT:" << request.url();
        // send request
        networkReply = m_nam->put(request, jsonData);
        break;
    } case Globals::HttpRequestTypeDelete: {
        // encode query as part of the url
        QUrl queryUrl(cmd->url());
        queryUrl.setQuery(cmd->query());
        request.setUrl(queryUrl);
        qDebug() << "[NetworkManager] DELETE: " << request.url();
        // send request
        networkReply = m_nam->deleteResource(request);
        break;
    }// if not set or unknown error
    case Globals::HttpRequestTypeNone: {
        qDebug() << "[NetworkManager] Error: Unkown request type";
        break;
    } default:
        qDebug() << "[NetworkManager] Error: Unkown network command!";
    }

    if (networkReply == nullptr) {
        qDebug() << "[NetworkManager] Error: NetWork reply is null!!";
        return nullptr;
    }

    NetworkReply *replyWrapper = new NetworkReply(networkReply);
    if (replyWrapper == nullptr) {
        // something didn't work out :/
        qDebug() << "[NetworkManager] Error: Unable to create network request!";
        return nullptr;
    }

    replyWrapper->setCustomData(data);
    return replyWrapper;
}

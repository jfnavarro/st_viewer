/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

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

#include "utils/DebugHelper.h"
#include "NetworkCommand.h"
#include "NetworkReply.h"
#include "NetworkManager.h"
#include "controller/auth/AuthorizationManager.h"
#include "controller/error/Error.h"
#include "utils/config/Configuration.h"

NetworkManager::NetworkManager(QObject* parent): QObject(parent), m_nam(0)
{

}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::init()
{
    // setup network access manager
    m_nam = QPointer<QNetworkAccessManager>(new QNetworkAccessManager(this));
    
    #if defined Q_OS_MAC
        //workaround for this https://bugreports.qt-project.org/browse/QTBUG-22033
        QNetworkProxy proxy = m_nam->proxy();
        proxy.setHostName(" ");
        m_nam->setProxy(proxy);
    #endif

    //add ssl support  (we need the public key) //TODO finish and try this (me dunno like ignoring ssl errors)
    //QFile cafile(":public_key.pem");
    //cafile.open(QIODevice::ReadOnly);
    //QSslCertificate cert(&cafile);
    //QSslSocket::addDefaultCaCertificate(cert);

    //add cache support (not really useful for downloading data)
//     QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
//     QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
//     diskCache->setCacheDirectory(location);
//     diskCache->setMaximumCacheSize(1000*1024*1024); // 1GB
//     m_nam->setCache(diskCache);
    
    //add https proxy (could be useful to avoid the authentication)
//     Configuration* config = Configuration::getInstance();
//     QNetworkProxy proxy(QNetworkProxy::HttpProxy," ",0,
//                         config->oauthClientID(),
//                         config->oauthSecret());
//     m_nam->setProxy(proxy);  
    
    //we want to provide Authentication to our oAuth based servers       
    connect(m_nam, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), 
            SLOT(provideProxyAuthentication(const QNetworkProxy &, QAuthenticator *)));
    connect(m_nam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            SLOT(provideAuthentication(QNetworkReply*,QAuthenticator*)));
}

void NetworkManager::finalize()
{
    if(!m_nam.isNull())
    {
        m_nam.clear();
    }
}

void NetworkManager::provideProxyAuthentication(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Configuration* config = Configuration::getInstance();
    authenticator->setUser(config->oauthClientID());
    authenticator->setPassword(config->oauthSecret());
}

void NetworkManager::provideAuthentication(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Configuration* config = Configuration::getInstance();
    authenticator->setUser(config->oauthClientID());
    authenticator->setPassword(config->oauthSecret());
}

NetworkReply* NetworkManager::httpRequest(NetworkCommand* cmd, QVariant data, NetworkFlags flags)
{
    DEBUG_FUNC_NAME
  
    // early out
    if (cmd == 0)
    {
        //NOTE something went wrong, return NULL
        qDebug() << "[NetworkManager] Error: Unable to create Network Command";
        return 0;
    }

    // append authentication token to network command
    AuthorizationManager* authorizationManager = AuthorizationManager::getInstance();
    if (flags.testFlag(UseAuthentication) && authorizationManager->hasAccessToken())
    {
        //TODO hide auth from network manager by appending QObject that encapsulates the key value pair
        const QUuid accessToken = authorizationManager->getAccessToken();
        cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1,36)); // QUuid encloses its uuids in "{}"...
    }

    QNetworkReply* networkReply = 0; // keep track of reply to match command later on (async callback)

    //creating the request
    QNetworkRequest request;
    
    //NOTE add caching to request (only if network caching is active)
    //request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    switch (cmd->type())
    {
        case Globals::HttpRequestTypeGet:
        {
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
        case Globals::HttpRequestTypePost:
        {
            // set clean url and explicit content type
            request.setUrl(cmd->url());
            // show debug text
            qDebug() << "[NetworkManager] POST:" << request.url() << "DATA:" << cmd->getEncodedQuery();
            //setting headers
            request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QStringLiteral("application/x-www-form-urlencoded")));
            // send request
            networkReply = m_nam->post(request, cmd->getEncodedQuery().toUtf8());
            break;
        }
        case Globals::HttpRequestTypePut:
        {
            // set clean url and explicit content type
            request.setUrl(cmd->url());
            request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QStringLiteral("application/x-www-form-urlencoded")));
            // show debug text
            qDebug() << "[NetworkManager] PUT:" << request.url() << "DATA:" << cmd->getEncodedQuery();
            // send request
            networkReply = m_nam->put(request, cmd->getEncodedQuery().toUtf8());
            break;
        }
        // if not set or unknown error
        case Globals::HttpRequestTypeNone:
        {
            qDebug() << "[NetworkManager] Error: Unkown request type";
            break;
        }
        default:
            qDebug() << "[NetworkManager] Error: Unkown network command!";
    }

    if (networkReply == 0)
    {
        qDebug() << "[NetworkManager] Error: NetWork reply is null!!";
        return 0;
    }
    
    NetworkReply* replyWrapper = 0;
    replyWrapper = new NetworkReply(networkReply, this); 
    replyWrapper->setCustomData(data);

    if (replyWrapper == 0)
    {
        // something didn't work out :/
        qDebug() << "[NetworkManager] Error: Unable to create network request!";
        return 0;
    }

    return replyWrapper;
}
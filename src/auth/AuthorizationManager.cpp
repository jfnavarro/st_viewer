/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AuthorizationManager.h"

#include "auth/OAuth2.h"
#include "data/DataProxy.h"

AuthorizationManager::AuthorizationManager(QPointer<DataProxy> dataProxy,
                                           QObject* parent)
    : QObject(parent),
      m_oAuth2(nullptr),
      m_dataProxy(dataProxy)
{

}

AuthorizationManager::~AuthorizationManager()
{

}

void AuthorizationManager::startAuthorization()
{
    //lazy init
    if (m_oAuth2.isNull()) {
        m_oAuth2 = new OAuth2(m_dataProxy, this);
        connect(m_oAuth2, SIGNAL(signalLoginDone(const QUuid&, int, const QUuid&)),
                this, SLOT(slotLoginDone(const QUuid&, int, const QUuid&)));
        connect(m_oAuth2, SIGNAL(signalError(QSharedPointer<Error>)),
                this, SIGNAL(signalError(QSharedPointer<Error>)));
    }
    // check if we already have been authorized and have access token saved
    // initialize authentication on valid token storage
    if (isAuthenticated()) {
        emit signalAuthorize();
    } else if (m_tokenStorage.hasRefreshToken()) {
        m_oAuth2->startQuietLogin(m_tokenStorage.getRefreshToken());
    } else {
        m_oAuth2->startInteractiveLogin();
    }
}

void AuthorizationManager::cleanAccesToken()
{
    m_tokenStorage.cleanAll();
}

void AuthorizationManager::slotLoginDone(const QUuid& accessToken, int expiresIn,
                                         const QUuid& refreshToken)
{
    m_tokenStorage.setAccessToken(accessToken, expiresIn);
    m_tokenStorage.setRefreshToken(refreshToken);
    emit signalAuthorize();
}

bool AuthorizationManager::isAuthenticated() const
{
    return m_tokenStorage.hasAccessToken() && !m_tokenStorage.isExpired();
}

bool AuthorizationManager::hasAccessToken() const
{
    return m_tokenStorage.hasAccessToken();
}

QUuid AuthorizationManager::getAccessToken() const
{
    return m_tokenStorage.getAccessToken();
}

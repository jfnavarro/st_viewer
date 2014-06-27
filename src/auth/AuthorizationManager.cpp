/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AuthorizationManager.h"

#include "auth/OAuth2.h"

AuthorizationManager::AuthorizationManager(QObject* parent)
    : QObject(parent),
      m_oAuth2(nullptr),
      m_tokenStorage(nullptr)
{

}

AuthorizationManager::~AuthorizationManager()
{

}

void AuthorizationManager::finalize()
{
    //m_oAuth2 and m_tokenStorage are smart pointers
}

void AuthorizationManager::init()
{
    m_tokenStorage.reset(new TokenStorage());
}

void AuthorizationManager::start()
{
    //lazy init
    if (m_oAuth2.isNull()) {
        m_oAuth2 = new OAuth2(this);
        connect(m_oAuth2.data(), SIGNAL(signalLoginDone(const QUuid&, int, const QUuid&)),
                this, SLOT(slotLoginDone(const QUuid&, int, const QUuid&)));
        connect(m_oAuth2.data(), SIGNAL(signalLoginAborted()), this, SIGNAL(signalLoginAborted()));
        connect(m_oAuth2.data(), SIGNAL(signalError(QSharedPointer<Error>)),
                this, SIGNAL(signalError(QSharedPointer<Error>)));
    }
    // check if we already have been authorized and have access token saved
    // initialize authentication on valid token storage
    if (isAuthenticated()) {
        emit signalAuthorize();
    } else {
        forceAuthentication();
    }
}

void AuthorizationManager::cleanAccesToken()
{
    m_tokenStorage->cleanAll();
}

void AuthorizationManager::forceAuthentication()
{
    if (m_tokenStorage->hasRefreshToken()) {
        m_oAuth2->startQuietLogin(m_tokenStorage->getRefreshToken());
    } else {
        m_oAuth2->startInteractiveLogin();
    }
}

void AuthorizationManager::slotLoginDone(const QUuid& accessToken, int expiresIn,
        const QUuid& refreshToken)
{
    m_tokenStorage->setAccessToken(accessToken, expiresIn);
    m_tokenStorage->setRefreshToken(refreshToken);
    emit signalAuthorize();
}

bool AuthorizationManager::isAuthenticated() const
{
   return m_tokenStorage->hasAccessToken()
          && !m_tokenStorage->isExpired();
}

bool AuthorizationManager::hasAccessToken() const
{
    return m_tokenStorage->hasAccessToken();
}

QUuid AuthorizationManager::getAccessToken() const
{
    return m_tokenStorage->getAccessToken();
}

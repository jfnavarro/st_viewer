/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"
#include "AuthorizationManager.h"

AuthorizationManager::AuthorizationManager(QObject* parent)
    : QObject(parent), m_oAuth2(0), m_tokenStorage(0)
{

}

AuthorizationManager::~AuthorizationManager()
{    

}

void AuthorizationManager::finalize()
{
    //m_oAuth2 will be deleted when parent container is deleted but 
    //if only if parent container has been passed
    if(!m_oAuth2.isNull())
    {
        m_oAuth2.clear();
    }

    if(!m_tokenStorage.isNull())
    {
        m_tokenStorage.clear();
    }
}


void AuthorizationManager::init()
{
    m_tokenStorage = QPointer<TokenStorage>(new TokenStorage());
}

void AuthorizationManager::start(QWidget* parentContainer)
{
    //lazy init
    if(m_oAuth2.isNull())
    {
        m_oAuth2 = QPointer<OAuth2>(new OAuth2(this,parentContainer));
        
        connect(m_oAuth2.data(), SIGNAL(signalLoginDone(const QUuid&, int, const QUuid&)),
                this, SLOT(slotLoginDone(const QUuid&, int, const QUuid&)));
        connect(m_oAuth2.data(), SIGNAL(signalLoginAborted()), this, SIGNAL(signalLoginAborted()));
        connect(m_oAuth2.data(), SIGNAL(signalError(Error*)), this, SIGNAL(signalError(Error*)));
    }
    // check if we already have been authorized and have access token saved
    // initialize authentication on valid token storage
    if (isAuthenticated())
    {
        emit signalAuthorize();
    }
    else
    {
        forceAuthentication();
    }
}

void AuthorizationManager::cleanAccesToken()
{
    qDebug() << "[AuthorizationManager] Cleaning access token/refresh token...";
    m_tokenStorage->cleanAll();
}

void AuthorizationManager::forceAuthentication()
{   
    if (m_tokenStorage->hasRefreshToken())
    {
        m_oAuth2->startQuietLogin(m_tokenStorage->getRefreshToken());
    }
    else
    {
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
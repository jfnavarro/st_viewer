/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef AUTHORIZATIONMANAGER_H
#define	AUTHORIZATIONMANAGER_H

#include <QDebug> 
#include <QObject>
#include <QUuid>
#include <QPointer>

#include "utils/Singleton.h"
#include "controller/auth/OAuth2.h"
#include "controller/auth/TokenStorage.h"

class QWidget; 

//  This class implements singleton pattern. It gives an interface for oAuth2 authorization 
//  trough the objects OAuth2 and TokenStorage.
//  It emits signals for error, authorized and abortion.
//  It allows for local storage of access token.


class AuthorizationManager : public QObject, public Singleton<AuthorizationManager>
{
    Q_OBJECT
    
public:
    
    explicit AuthorizationManager(QObject* parent = 0);
    virtual ~AuthorizationManager();

    void finalize();
    void init();
    
    //initilize the log in over a QWidget container
    void start(QWidget* parentContainer = 0);
    
    //clean access token
    void cleanAccesToken();
    
    inline const bool isAuthenticated() const { return m_tokenStorage->hasAccessToken() 
                                                && !m_tokenStorage->isExpired(); }
    void forceAuthentication();

    inline const bool hasAccessToken() const { return m_tokenStorage->hasAccessToken(); }
    inline const QUuid getAccessToken() const { return m_tokenStorage->getAccessToken(); }
    
signals:
    
    void signalLoginAborted();
    void signalAuthorize();
    void signalError(Error* error);
     
private slots:
    
    void slotLoginDone(const QUuid& accessToken, int expiresIn, const QUuid& refreshToken);

private:
    
    QPointer<OAuth2> m_oAuth2;
    QPointer<TokenStorage> m_tokenStorage;

};

#endif	/* AUTHORIZATIONMANAGER_H */


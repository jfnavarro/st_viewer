/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef AUTHORIZATIONMANAGER_H
#define AUTHORIZATIONMANAGER_H

#include <QObject>
#include <QPointer>
#include <QUuid>

#include "auth/TokenStorage.h"
#include "utils/Singleton.h"

class OAuth2;
class QWidget;
class Error;

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

    //singleton initialization and destroying methos
    void finalize();
    void init();

    //initilize the log in on the QApp (modal)
    void start();

    //clean access token
    void cleanAccesToken();

    //true if the user is already authenticated
    bool isAuthenticated() const;

    //force to log out and clean cache
    void forceAuthentication();

    //user acces token methods to check
    //if the user is already logged in
    bool hasAccessToken() const;
    QUuid getAccessToken() const;

signals:

    //the login was canceled
    void signalLoginAborted();
    //the login is succesful
    void signalAuthorize();
    //there was an error trying to log in
    void signalError(QSharedPointer<Error> error);

private slots:

    //internal slot to process login object from OAuth2
    void slotLoginDone(const QUuid& accessToken, int expiresIn,
                       const QUuid& refreshToken);

private:

    QScopedPointer<OAuth2> m_oAuth2;
    QScopedPointer<TokenStorage> m_tokenStorage;

    Q_DISABLE_COPY(AuthorizationManager)
};

#endif  /* AUTHORIZATIONMANAGER_H */


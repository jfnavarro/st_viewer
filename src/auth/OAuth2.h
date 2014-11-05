/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef OAUTH2_H
#define OAUTH2_H

#include <QObject>
#include <QPointer>

#include "data/DataProxy.h"

class LoginDialog;
class Error;
class QUuid;
class QString;

// simple class that handles OAuth2 authorization requests
// it contains a Login widget to let the user input the credentials
// it implements basic error handling and two modes of authorization (interactive and silent)
class OAuth2 : public QObject
{
    Q_OBJECT

public:

    //TODO duplicated in DataProxy
    typedef QPair<QString, QString> StringPair;

    OAuth2(QPointer<DataProxy> dataProxy, QObject* parent = 0);
    virtual ~OAuth2();

    // shows login dialog
    void startInteractiveLogin();

    // try to log in with stored access token
    void startQuietLogin(const QUuid& refreshToken);

signals:

    void signalLoginDone(const QUuid& accessToken,
                         int expiresIn, const QUuid& refreshToken);
    void signalError(QSharedPointer<Error> error);

private slots:

    //user enters log in (called from log in component) can be used to
    //try log in with hardcoded credentials
    void slotEnterDialog(const QString&, const QString&);

    //used to be notified when access token has been downloaded from network
    //status contains the status of the operation (ok, abort, error)
    void slotAccessTokenDownloaded(DataProxy::DownloadStatus status);

private:

    // make the authorization network request
    void requestToken(const StringPair& accessType, const StringPair& accessCode);

    // login dialog component
    QPointer<LoginDialog> m_loginDialog;

    //reference to dataProxy;
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(OAuth2)
};

#endif // OAUTH2_H

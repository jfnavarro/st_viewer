/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef OAUTH2_H
#define OAUTH2_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QDebug>
#include <QPointer>

class LoginDialog;
class Error;

// simple class that handles OAuth2 authorization requests
// it contains a LogIn widget to let the user input the credentials
// it implements basic error handling and two modes of authorization (interactive and silent)

class OAuth2 : public QObject
{
    Q_OBJECT

public:
    
    typedef QPair<QString,QString> StringPair;
    
    //parent container should be qwidget
    OAuth2(QObject* parent, QWidget* parentContainer = 0);
    
    virtual ~OAuth2();
    
    // shows login dialog
    void startInteractiveLogin();
    // try logs in with stored access token
    void startQuietLogin(const QUuid& refreshToken);
    
signals:
    
    void signalLoginDone(const QUuid& accessToken, int expiresIn, const QUuid& refreshToken);
    void signalLoginAborted();
    void signalError(Error* error);

private slots:
    
    //handles the authorization network reply
    void slotNetworkReply(QVariant code, QVariant data);

    //user enters log in (called from log in component) can be used to try log in with hardcoded credentials
    void slotEnterDialog(QString,QString);

private:
    
    // make the authorization network request
    void requestToken(const StringPair& accessType, const StringPair& accessCode);

    // login dialog component
    QPointer<LoginDialog> m_loginDialog;
    
    // parent container for login dialog
    QWidget *m_parentContainer;
};

#endif // OAUTH2_H

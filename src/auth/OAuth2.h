#ifndef OAUTH2_H
#define OAUTH2_H

#include <QObject>
#include <QPointer>

#include "data/DataProxy.h"

class LoginDialog;
class Error;
class QUuid;
class QString;

// Simple class that handles OAuth2 authorization requests
// it contains a log-in widget to let the user input the credentials
// it implements basic error handling and two modes of authorization
// (interactive and silent)
class OAuth2 : public QObject
{
    Q_OBJECT

public:
    OAuth2(QSharedPointer<DataProxy> dataProxy, QObject *parent = 0);
    virtual ~OAuth2();

    // Shows login dialog
    void startInteractiveLogin();

    // Try to log in with stored access token
    void startQuietLogin(const QUuid &refreshToken);

signals:
    // To communicate the status of the log-in
    void signalLoginDone(const QUuid &accessToken, const int expiresIn, const QUuid &refreshToken);
    void signalError(QSharedPointer<Error> error);

private slots:
    // User has entereded log in credentials (called from log in component) It can
    // also be used to
    // try to log in with hardcoded credentials
    void slotEnterDialog(const QString &, const QString &);

private:
    // Make the authorization network request
    void requestToken(const std::pair<QString, QString> &accessType,
                      const std::pair<QString, QString> &accessCode);

    // Login dialog widget
    QScopedPointer<LoginDialog> m_loginDialog;

    // Reference to dataProxy
    QSharedPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(OAuth2)
};

#endif // OAUTH2_H

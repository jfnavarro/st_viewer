#include "OAuth2.h"

#include <QDebug>
#include <QJsonDocument>
#include <QString>
#include <QUuid>
#include <QApplication>
#include <QDesktopWidget>

#include "dialogs/LoginDialog.h"
#include "error/ServerError.h"
#include "dataModel/ErrorDTO.h"
#include "dataModel/OAuth2TokenDTO.h"
#include "data/ObjectParser.h"
#include "SettingsNetwork.h"

using namespace Network;

OAuth2::OAuth2(QSharedPointer<DataProxy> dataProxy, QObject *parent)
    : QObject(parent)
    , m_loginDialog(nullptr)
    , m_dataProxy(dataProxy)
{
}

OAuth2::~OAuth2()
{
}

void OAuth2::startQuietLogin(const QUuid &refreshToken)
{
    // QUuid encloses its uuids in "{}"...
    requestToken(std::pair<QString, QString>(LBL_GRANT_TYPE, SettingsRefreshToken),
                 std::pair<QString, QString>(SettingsRefreshToken,
                                             refreshToken.toString().mid(1, 36)));
}

void OAuth2::startInteractiveLogin()
{
    // Lazy init
    if (m_loginDialog.isNull()) {
        QWidget *mainWidget = QApplication::desktop()->screen();
        m_loginDialog.reset(new LoginDialog(mainWidget, Qt::WindowStaysOnTopHint));
        m_loginDialog->setWindowFlags(m_loginDialog->windowFlags()
                                      & (Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint
                                         | Qt::CustomizeWindowHint));
        connect(m_loginDialog.data(),
                SIGNAL(acceptLogin(const QString &, const QString &)),
                this,
                SLOT(slotEnterDialog(const QString &, const QString &)));
    }
    // Launch login dialog
    m_loginDialog->clear();
    m_loginDialog->show();
}

void OAuth2::slotEnterDialog(const QString &username, const QString &password)
{
    // Request token based on password/username
    requestToken(std::pair<QString, QString>(LBL_ACCESS_TOKEN_USERNAME, username),
                 std::pair<QString, QString>(LBL_ACCESS_TOKEN_PASSWORD, password));
}

void OAuth2::requestToken(const std::pair<QString, QString> &requestUser,
                          const std::pair<QString, QString> &requestPassword)
{
    if (m_dataProxy->loadAccessToken(requestUser, requestPassword)) {
        const OAuth2TokenDTO &dto = m_dataProxy->getAccessToken();
        const QUuid &accessToken(dto.accessToken());
        const int expiresIn = dto.expiresIn();
        const QUuid &refreshToken(dto.refreshToken());
        // Check if access token is valid and not expired
        if (!accessToken.isNull() && expiresIn >= 0 && !refreshToken.isNull()) {
            emit signalLoginDone(accessToken, expiresIn, refreshToken);
        } else {
            emit signalError(QSharedPointer<ServerError>(
                new ServerError(tr("Log in Error"), tr("Access token is expired"), this)));
        }
    } else {
        emit signalError(QSharedPointer<ServerError>(
            new ServerError(tr("Log in Error"), tr("Wrong credentials"))));
    }
}

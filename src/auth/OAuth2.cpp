/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "OAuth2.h"

#include <QDebug>
#include <QJsonDocument>
#include <QString>
#include <QUuid>
#include <QApplication>

#include "dialogs/LoginDialog.h"
#include "error/OAuth2Error.h"
#include "dataModel/ErrorDTO.h"
#include "dataModel/OAuth2TokenDTO.h"
#include "data/ObjectParser.h"
#include "data/DataProxy.h"
#include "utils/Utils.h"
#include "network/DownloadManager.h"

OAuth2::OAuth2(QPointer<DataProxy> dataProxy, QObject* parent)
    : QObject(parent),
      m_loginDialog(nullptr),
      m_dataProxy(dataProxy)
{

}

OAuth2::~OAuth2()
{
    m_loginDialog->deleteLater();
    m_loginDialog = nullptr;
}

void OAuth2::startQuietLogin(const QUuid& refreshToken)
{
    // QUuid encloses its uuids in "{}"...
    requestToken(StringPair(Globals::LBL_GRANT_TYPE, Globals::SettingsRefreshToken),
                 StringPair(Globals::SettingsRefreshToken, refreshToken.toString().mid(1, 36)));
}

void OAuth2::startInteractiveLogin()
{
    // lazy init
    if (m_loginDialog.isNull()) {
        //TODO get MainWindow central widget and give it as parent
        m_loginDialog = new LoginDialog();
        connect(m_loginDialog, SIGNAL(acceptLogin(const QString&, const QString&)), this,
                SLOT(slotEnterDialog(const QString&, const QString&)));
    }
    //launch login dialog
    m_loginDialog->clear();
    m_loginDialog->show();
    m_loginDialog->raise();
    m_loginDialog->activateWindow();
}

void OAuth2::slotEnterDialog(const QString &username, const QString &password)
{
    //request token based on password//username
    requestToken(StringPair(Globals::LBL_ACCESS_TOKEN_USERNAME, username),
                 StringPair(Globals::LBL_ACCESS_TOKEN_PASSWORD, password));
}

void OAuth2::requestToken(const StringPair& requestUser, const StringPair& requestPassword)
{
    async::DataRequest request = m_dataProxy->loadAccessToken(requestUser, requestPassword);

    if (!request.isSuccessFul()) {
        //TODO get error from the request
        QSharedPointer<OAuth2Error>
               error(new OAuth2Error("Log in Error", "Error retrieving access token", this));
        emit signalError(error);
        return;
    }

    OAuth2TokenDTO dto = m_dataProxy->getAccessToken();
    const QUuid accessToken(dto.accessToken());
    const int expiresIn = dto.expiresIn();
    const QUuid refreshToken(dto.refreshToken());

    if (!accessToken.isNull() && expiresIn >= 0 && !refreshToken.isNull()) {
        emit signalLoginDone(accessToken, expiresIn, refreshToken);
    } else {
         QSharedPointer<OAuth2Error>
                error(new OAuth2Error("Log in Error", "Access token is expired", this));
        emit signalError(error);
    }
}

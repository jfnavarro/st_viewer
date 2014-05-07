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

#include "utils/DebugHelper.h"

#include "dialogs/LoginDialog.h"

#include "network/RESTCommandFactory.h"
#include "network/NetworkManager.h"
#include "network/NetworkReply.h"
#include "error/OAuth2Error.h"

#include "dataModelDTO/ErrorDTO.h"
#include "dataModelDTO/OAuth2TokenDTO.h"
#include "dataModel/ObjectParser.h"

#include "utils/Utils.h"

OAuth2::OAuth2(QObject* parent)
    : QObject(parent),
      m_loginDialog(nullptr)
{

}

OAuth2::~OAuth2()
{
    //m_loginDialog is smart pointer
}

void OAuth2::startQuietLogin(const QUuid& refreshToken)
{
    DEBUG_FUNC_NAME
    // request token based on valid refresh token
    const StringPair requestType(Globals::LBL_GRANT_TYPE, Globals::SettingsRefreshToken);
    const StringPair requestData(Globals::SettingsRefreshToken,
                                 refreshToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"...
    requestToken(requestType, requestData);
}

void OAuth2::startInteractiveLogin()
{
    DEBUG_FUNC_NAME
    // lazy init
    if (m_loginDialog.isNull()) {
        m_loginDialog = new LoginDialog();
        connect(m_loginDialog.data(), SIGNAL(exitLogin()), this, SIGNAL(signalLoginAborted()));
        connect(m_loginDialog.data(), SIGNAL(acceptLogin(const QString&, const QString&)), this,
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
    qDebug() << "[OAuth2] Trying to log in with = " << username << " " << password;
    //request token based on password//username
    requestToken(StringPair(Globals::LBL_ACCESS_TOKEN_USERNAME, username),
                 StringPair(Globals::LBL_ACCESS_TOKEN_PASSWORD, password));
}

void OAuth2::requestToken(const StringPair& requestUser, const StringPair& requestPassword)
{
    NetworkCommand* cmd = RESTCommandFactory::getAuthorizationToken();
    cmd->addQueryItem(requestUser.first, requestUser.second);
    cmd->addQueryItem(requestPassword.first, requestPassword.second);
    // send empty flags to ensure access token is not appended to request
    NetworkManager *m_networkManager = NetworkManager::getInstance();
    NetworkReply* request =
            m_networkManager->httpRequest(cmd, QVariant(QVariant::Invalid), NetworkManager::Empty);
    //check reply is correct
    if (request == nullptr) {
        OAuth2Error* error = new OAuth2Error("Log in Error", "Connection Problem", this);
        emit signalError(error);
    } else {
        connect(request, SIGNAL(signalFinished(QVariant, QVariant)),
                this, SLOT(slotNetworkReply(QVariant, QVariant)));
    }
    //clean up
    cmd->deleteLater();
}

void OAuth2::slotNetworkReply(QVariant code, QVariant data)
{
    Q_UNUSED(data);
    Q_UNUSED(code);

    // get reference to network reply from sender object
    NetworkReply* reply = dynamic_cast<NetworkReply*>(sender());
    // null reply, prob no connection
    if (reply == nullptr) {
        OAuth2Error* error = new OAuth2Error("Log in Error", "Connection Problem", this);
        emit signalError(error);
        return;
    }
    const NetworkReply::ReturnCode returnCode =
            static_cast<NetworkReply::ReturnCode>(reply->return_code());
    if (returnCode == NetworkReply::CodeError) {
        OAuth2Error* error = new OAuth2Error("Log in Error", "Authorization Failed", this);
        emit signalError(error);
        reply->deleteLater();
        return;
    }
    //parse the reply
    const QJsonDocument document = reply->getJSON();
    const QVariant result = document.toVariant();
    //no errors, good
    if (!reply->hasErrors()) {
        OAuth2TokenDTO dto;
        ObjectParser::parseObject(result, &dto);
        const QUuid accessToken = QUuid(dto.accessToken());
        const int expiresIn = dto.expiresIn();
        const QUuid refreshToken = QUuid(dto.refreshToken());
        if (!accessToken.isNull() && (expiresIn >= 0) && !refreshToken.isNull()) {
            emit signalLoginDone(accessToken, expiresIn, refreshToken);
        }
    } else {
        Error *error = reply->parseErrors();
        emit signalError(error);
    }
    //clean up
    reply->deleteLater();
}

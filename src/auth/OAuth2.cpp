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
#include <QDesktopWidget>

#include "dialogs/LoginDialog.h"
#include "error/OAuth2Error.h"
#include "dataModel/ErrorDTO.h"
#include "dataModel/OAuth2TokenDTO.h"
#include "data/ObjectParser.h"
#include "utils/Utils.h"

OAuth2::OAuth2(QPointer<DataProxy> dataProxy, QObject *parent)
    : QObject(parent),
      m_loginDialog(nullptr),
      m_dataProxy(dataProxy)
{
    //connect data proxy signals
    connect(m_dataProxy.data(),
            SIGNAL(signalAccessTokenDownloaded(DataProxy::DownloadStatus)),
            this, SLOT(slotAccessTokenDownloaded(DataProxy::DownloadStatus)));
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
        QWidget *mainWidget = QApplication::desktop()->screen();
        m_loginDialog = new LoginDialog(mainWidget, Qt::WindowStaysOnTopHint);
        m_loginDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint
                                      | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        connect(m_loginDialog, SIGNAL(acceptLogin(const QString&, const QString&)), this,
                SLOT(slotEnterDialog(const QString&, const QString&)));
    }
    //launch login dialog
    m_loginDialog->clear();
    m_loginDialog->show();
}

void OAuth2::slotEnterDialog(const QString &username, const QString &password)
{
    //request token based on password/username
    requestToken(StringPair(Globals::LBL_ACCESS_TOKEN_USERNAME, username),
                 StringPair(Globals::LBL_ACCESS_TOKEN_PASSWORD, password));
}

void OAuth2::requestToken(const StringPair& requestUser, const StringPair& requestPassword)
{
    //TODO maybe should block and wait for this?
    m_dataProxy->loadAccessToken(requestUser, requestPassword);
    m_dataProxy->activateCurrentDownloads();
}

void OAuth2::slotAccessTokenDownloaded(DataProxy::DownloadStatus status)
{
    if (status == DataProxy::Success) {
        OAuth2TokenDTO dto = m_dataProxy->getAccessToken();
        const QUuid accessToken(dto.accessToken());
        const int expiresIn = dto.expiresIn();
        const QUuid refreshToken(dto.refreshToken());
        //check if access token is valid and not expired
        if (!accessToken.isNull() && expiresIn >= 0 && !refreshToken.isNull()) {
            emit signalLoginDone(accessToken, expiresIn, refreshToken);
        } else {
            QSharedPointer<OAuth2Error>
                    error(new OAuth2Error(tr("Log in Error"), tr("Access token is expired"), this));
            emit signalError(error);
        }
    }
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>

#include "utils/DebugHelper.h"

#include "view/components/LoginDialog.h"

#include "controller/network/RESTCommandFactory.h"
#include "controller/network/NetworkManager.h"
#include "controller/network/NetworkReply.h"
#include "controller/error/OAuth2Error.h"

#include "model/dto/ErrorDTO.h"
#include "model/dto/OAuth2TokenDTO.h"
#include "model/ObjectParser.h"

#include "utils/config/Configuration.h"
#include "utils/Utils.h"

#include "OAuth2.h"

OAuth2::OAuth2(QObject* parent, QWidget* parentContainer) : 
    QObject(parent), 
    m_loginDialog(0), 
    m_parentContainer(parentContainer)
{

}

OAuth2::~OAuth2()
{
    //m_loginDialog will be destroyed when parent is destroyed but if parent is not given...
    if(!m_loginDialog.isNull())
    {
        m_loginDialog.clear();
    }
    
    //no need to delete parentContainer since it is a qwidget part of the mainwindow
}

void OAuth2::startQuietLogin(const QUuid& refreshToken)
{
    DEBUG_FUNC_NAME

    // request token based on valid refresh token
    const StringPair requestType(Globals::LBL_GRANT_TYPE, Globals::SettingsRefreshToken);
    const StringPair requestData(Globals::SettingsRefreshToken, refreshToken.toString().mid(1,36)); // QUuid encloses its uuids in "{}"...
    requestToken(requestType, requestData);
}

void OAuth2::startInteractiveLogin()
{
    DEBUG_FUNC_NAME

    Configuration* config = Configuration::getInstance();
    
    if(config->oauthClientID() == QStringLiteral("YOUR_CLIENT_ID_HERE") ||
       config->oauthClientID().isEmpty() ||
       config->oauthSecret() == QStringLiteral("YOUR_CLIENT_SECRET_HERE") ||
       config->oauthSecret().isEmpty())
    {
        qDebug() << "[OAuth2] Error: To work with application you need to register "
                 <<  "your own application in SpatialTranscriptomics.";
        return;
    }

    // lazy init 
    if(m_loginDialog.isNull())
    {
        m_loginDialog = QPointer<LoginDialog>(new LoginDialog(m_parentContainer));
        connect(m_loginDialog.data(), SIGNAL(exitLogin()), this, SIGNAL(signalLoginAborted()));
        connect(m_loginDialog.data(), SIGNAL(acceptLogin(QString,QString)), this, SLOT(slotEnterDialog(QString,QString)));
    }
    
    m_loginDialog->clear();
    m_loginDialog->show();
    m_loginDialog->raise();
    m_loginDialog->activateWindow();
}

void OAuth2::slotEnterDialog(QString username, QString password)
{   
    qDebug() << "[OAuth2] Trying to log in with = " << username << " " << password;
    
    //request token based on password//username
    requestToken(StringPair(Globals::LBL_ACCESS_TOKEN_USERNAME,username),
                 StringPair(Globals::LBL_ACCESS_TOKEN_PASSWORD,password));
    
}

void OAuth2::requestToken(const StringPair& requestUser, const StringPair& requestPassword)
{
    NetworkCommand* cmd = RESTCommandFactory::getAuthorizationToken();
    cmd->addQueryItem(requestUser.first,requestUser.second);
    cmd->addQueryItem(requestPassword.first,requestPassword.second);

    // send empty flags to ensure access token is not appended to request
    NetworkManager *m_networkManager = NetworkManager::getInstance();
    NetworkReply* request = m_networkManager->httpRequest(cmd, QVariant(QVariant::Invalid), NetworkManager::Empty);
    
    if(request == 0)
    {
        qDebug() << "[OAuth2] Network Manager errror";
        OAuth2Error* error = new OAuth2Error("Log in Error", "Connection Problem", this);
        emit signalError(error);
    }
    else
    {
        connect(request, SIGNAL(signalFinished(QVariant, QVariant)), this, SLOT(slotNetworkReply(QVariant, QVariant)));
    }
    
    delete cmd;
}

void OAuth2::slotNetworkReply(QVariant code, QVariant data)
{
    // get reference to network reply from sender object
    NetworkReply* reply = dynamic_cast<NetworkReply*>(sender());
    
    if(reply == 0)
    {
        qDebug() << "[OAuth2] Network Manager errror";
        OAuth2Error* error = new OAuth2Error("Log in Error", "Connection Problem", this);
        emit signalError(error);
        return;
    }

    // early out
    int returnCode = qvariant_cast<int>(code);
    if (returnCode == NetworkReply::CodeError)
    {
        qDebug() << "[OAuth2] Network Manager errror";
        OAuth2Error* error = new OAuth2Error("Log in Error", "Authorization Failed", this);
        emit signalError(error);
        reply->deleteLater();
        return;
    }

    QJsonDocument document = reply->getJSON();
    QVariant result = document.toVariant();
    
    if (!reply->hasErrors())
    {
        OAuth2TokenDTO dto;
        ObjectParser::parseObject(result, &dto);
        
        QUuid accessToken = QUuid(dto.accessToken());
        int expiresIn = dto.expiresIn();
        QUuid refreshToken = QUuid(dto.refreshToken());
        
//         //add the acepted username to the list of valid users
//         m_loginDialog->setUsername(m_loginDialog->getCurrentUser());
        
        if (!accessToken.isNull() && (expiresIn >= 0) && !refreshToken.isNull())
        {
            emit signalLoginDone(accessToken, expiresIn, refreshToken);
        }
    }
    else
    {   
        const NetworkReply::ErrorList &errors = reply->errors();
        
        if(errors.count() > 1)
        {
            QString errortext;
            
            foreach(Error *e, errors)
            {
                qDebug() << "[OAuth2] Network Reply Error " << e->name() << " : " << e->description();
                errortext += (e->name() + " : " + e->description()) + "\n";
            }
            
            //NOTE need to emit a standard Error that packs all the errors descriptions
            Error* error = new Error("Multiple Authorization Error", errortext, this);
            emit signalError(error);
        }
        else
        {
            ErrorDTO dto;
            ObjectParser::parseObject(result, &dto);
            QString errorName = dto.errorName();
            QString errorDescription = dto.errorDescription();
            qDebug() << "[OAuth2] Network Reply Error " << errorName << " : " << errorDescription;;        
            Error* error = new OAuth2Error(errorName, errorDescription, this);
            emit signalError(error);
        }
    }
    
    reply->deleteLater();
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkReply.h"

#include <QApplication>
#include <QStringList>
#include <QDebug>
#include "utils/DebugHelper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "network/NetworkCommand.h"
#include "error/Error.h"
#include "error/JSONError.h"
#include "error/NetworkError.h"
#include "error/SSLNetworkError.h"

#include "dataModelDTO/ErrorDTO.h"
#include "dataModel/ObjectParser.h"

ContentType::ContentType(QObject* parent)
    : QObject(parent),
      m_mime()
{

}

ContentType::ContentType(const QString& contentType, QObject* parent)
    : QObject(parent),
      m_mime()
{
    // parse the content type header
    header(contentType);
}

ContentType::~ContentType()
{

}

void ContentType::header(const QString& value)
{
    //NOTE currently we only parse the mime type but charset might also be worth exposing
    //     use meta properties to assign key/value pairs if so
    m_mime = value.split(';')[0];
}

NetworkReply::NetworkReply(QNetworkReply* networkReply, QObject* parent)
    :  m_reply(networkReply), m_contentType(0)
{
    Q_UNUSED(parent);
    Q_ASSERT_X(networkReply != 0, "NetworkReply", "Null-pointer assertion error!");

    networkReply->setReadBufferSize(0); //try to download as fast as possible

    // construct empty content type object
    m_contentType = new ContentType(this); //this is ugly

    // connect signals
    connect(networkReply, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(networkReply, SIGNAL(metaDataChanged()), this, SLOT(slotMetaDataChanged()));
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(networkReply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
}

NetworkReply::~NetworkReply()
{
    //reply will be deleted by the sender once data is parsed or error handled (data proxy most likely)
}

QJsonDocument NetworkReply::getJSON()
{
    QByteArray rawJSON = m_reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawJSON, &parseError);
    // trigger error signals on error
    if (parseError.error != QJsonParseError::NoError) {
        Error* error = new JSONError(parseError.error, this);
        registerError(error);
    }
    return doc;
}

QString NetworkReply::getText()
{
    return QString::fromUtf8(m_reply->readAll());
}

QByteArray NetworkReply::getRaw()
{
    return m_reply->readAll();
}

void NetworkReply::slotAbort()
{
    // abort network operation
    m_reply->abort();
}

void NetworkReply::slotFinished()
{
    // determine return code
    int ret = CodeSuccess;
    switch (m_reply->error()) {
    case QNetworkReply::NoError:
        ret = CodeSuccess;
        break;
    case QNetworkReply::OperationCanceledError:
        ret = CodeAbort;
        break;
    default:
        ret = CodeError;
    }
    emit signalFinished(QVariant::fromValue<int>(ret), m_data);
}

void NetworkReply::slotMetaDataChanged()
{
    QString contentTypeHeader = m_reply->header(QNetworkRequest::ContentTypeHeader).toString();
    m_contentType->header(contentTypeHeader);
}

void NetworkReply::slotError(QNetworkReply::NetworkError networkError)
{
    // create and register error only if the error was not an abort
    if (networkError != QNetworkReply::OperationCanceledError
        && networkError != QNetworkReply::NoError) {
        Error* error = new NetworkError(networkError, this);
        registerError(error);
    }
}

void NetworkReply::slotSslErrors(QList<QSslError> sslErrorList)
{
    //TODO ignoring ssl errors for now to make it the request works with https
    //but we should add a flag for this or add the certificate public key to the client
    //alternatively we could ask the user to accept the certificate
    m_reply->ignoreSslErrors(sslErrorList);
}

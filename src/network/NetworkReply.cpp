/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkReply.h"

#include <QApplication>
#include <QStringList>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "network/NetworkCommand.h"
#include "error/Error.h"
#include "error/JSONError.h"
#include "error/NetworkError.h"
#include "error/SSLNetworkError.h"
#include "error/ServerError.h"

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

const QString ContentType::mime() const
{
    return m_mime;
}

void ContentType::header(const QString& value)
{
    //NOTE currently we only parse the mime type but charset might also be worth exposing
    //     use meta properties to assign key/value pairs if so
    m_mime = value.split(';')[0];
}

NetworkReply::NetworkReply(QNetworkReply* networkReply)
    :  m_reply(networkReply),
      m_contentType(nullptr)
{
    Q_ASSERT_X(networkReply != nullptr, "NetworkReply", "Null-pointer assertion error!");
    //try to download as fast as possible
    networkReply->setReadBufferSize(0);

    // construct empty content type object
    m_contentType = new ContentType(this);

    // connect signals
    connect(networkReply, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(networkReply, SIGNAL(metaDataChanged()), this, SLOT(slotMetaDataChanged()));
    connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this,
            SLOT(slotError(QNetworkReply::NetworkError)));
    connect(networkReply, SIGNAL(sslErrors(QList<QSslError>)), this,
            SLOT(slotSslErrors(QList<QSslError>)));
}

NetworkReply::~NetworkReply()
{
    //reply will be deleted by the sender once data is parsed or error handled (data proxy most likely)
}

const QVariant NetworkReply::customData() const
{
    return m_data;
}

void NetworkReply::setCustomData(QVariant data)
{
    m_data = data;
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

QString NetworkReply::getText() const
{
    return QString::fromUtf8(m_reply->readAll());
}

QByteArray NetworkReply::getRaw() const
{
    return m_reply->readAll();
}

const ContentType* NetworkReply::contentType() const
{
    return m_contentType;
}

bool NetworkReply::isType(const QString &mime) const
{
    return m_contentType->mime() == mime;
}

bool NetworkReply::isFinished() const
{
    return m_reply->isFinished();
}

bool NetworkReply::hasErrors() const
{
    return !m_errors.isEmpty();
}

const NetworkReply::ErrorList& NetworkReply::errors() const
{
    return m_errors;
}

void NetworkReply::slotAbort()
{
    // abort network operation
    m_reply->abort();
}

void NetworkReply::slotFinished()
{
    // determine return code
    ReturnCode ret = CodeSuccess;
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
    m_code = ret;
    emit signalFinished(QVariant::fromValue<int>(ret), m_data);
}

void NetworkReply::slotMetaDataChanged()
{
    QString contentTypeHeader =
            m_reply->header(QNetworkRequest::ContentTypeHeader).toString();
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

void NetworkReply::registerError(Error *error)
{
    m_errors += error;
}

Error *NetworkReply::parseErrors()
{
    Error *error = nullptr;
    if (m_errors.count() > 1) {
        QString errortext;
        foreach(Error *e, m_errors) {
            errortext += QString("%1 : %2 \n").arg(e->name()).arg(e->description());
        }
        //NOTE need to emit a standard Error that packs all the errors descriptions
        error = new Error("Multiple Data Error", errortext, nullptr);
    } else {
        const QJsonDocument doc = getJSON();
        QVariant var = doc.toVariant();
        ErrorDTO dto;
        ObjectParser::parseObject(var, &dto);
        error = new ServerError(dto.errorName(), dto.errorDescription());
    }

    return error;
}

NetworkReply::ReturnCode NetworkReply::return_code() const
{
    return m_code;
}

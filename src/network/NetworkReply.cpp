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
#include <QJsonArray>
#include <QTimer>
#include <QNetworkRequest>

#include "network/NetworkCommand.h"
#include "error/Error.h"
#include "error/JSONError.h"
#include "error/NetworkError.h"
#include "error/SSLNetworkError.h"
#include "error/ServerError.h"

#include "dataModel/ErrorDTO.h"
#include "data/ObjectParser.h"

NetworkReply::NetworkReply(QNetworkReply* networkReply)
    : m_reply(networkReply)
{
    Q_ASSERT_X(networkReply != nullptr, "NetworkReply", "Null-pointer assertion error!");

    // set read buffer to 0 to try to download as fast as possible
    networkReply->setReadBufferSize(0);

    // connect signals
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(slotMetaDataChanged()));
    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(slotError(QNetworkReply::NetworkError)));
    connect(m_reply,
            SIGNAL(sslErrors(QList<QSslError>)),
            this,
            SLOT(slotSslErrors(QList<QSslError>)));
}

NetworkReply::~NetworkReply()
{
    m_reply->deleteLater();
    m_reply = nullptr;
}

QJsonDocument NetworkReply::getJSON()
{
    QByteArray rawJSON = m_reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(rawJSON, &parseError);

    // trigger error signals on error
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing JSON " << parseError.errorString();
        QSharedPointer<Error> error(new JSONError(parseError.error, this));
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
    switch (m_reply->error()) {
    case QNetworkReply::NoError:
        m_code = CodeSuccess;
        break;
    case QNetworkReply::OperationCanceledError:
        m_code = CodeAbort;
        break;
    default:
        m_code = CodeError;
    }

    // send a signal with the return code and the meta data
    emit signalFinished(QVariant::fromValue<int>(m_code));
}

void NetworkReply::slotMetaDataChanged()
{
    QString contentTypeHeader = m_reply->header(QNetworkRequest::ContentTypeHeader).toString();
    m_mime = contentTypeHeader.split(';')[0];
}

void NetworkReply::slotError(QNetworkReply::NetworkError networkError)
{
    // create and register error only if the error was not an abort
    if (networkError != QNetworkReply::OperationCanceledError
        && networkError != QNetworkReply::NoError) {
        QSharedPointer<Error> error(new NetworkError(networkError, this));
        registerError(error);
    }
}

void NetworkReply::slotSslErrors(QList<QSslError> sslErrorList)
{
    foreach (QSslError error, sslErrorList) {
        QSharedPointer<Error> sslerror(new SSLNetworkError(error, this));
        registerError(sslerror);
    }
}

void NetworkReply::registerError(QSharedPointer<Error> error)
{
    m_errors += error;
}

QSharedPointer<Error> NetworkReply::parseErrors()
{
    QSharedPointer<Error> error;

    if (m_errors.count() > 1) {
        // if we have more than one error we aggregate them
        QString errortext;
        foreach (QSharedPointer<Error> error, m_errors) {
            errortext += QString("%1 : %2 \n").arg(error->name()).arg(error->description());
        }

        error = QSharedPointer<Error>(new Error(tr("Multiple Network Error"), errortext, this));
    } else if (m_errors.count() == 1) {
        const QJsonDocument doc = getJSON();
        // error could happen parsing the JSON content
        if (doc.isEmpty() || doc.isNull()) {
            // this means it was a Network error so we return the network error (must be the 1st)
            error = m_errors.first();
        } else {
            // if error was wrapped in JSON must be a server error
            QVariant var = doc.toVariant();
            ErrorDTO dto;
            data::parseObject(var, &dto);
            error = QSharedPointer<Error>(new ServerError(dto.errorName(), dto.errorDescription()));
        }
    }

    return error;
}

bool NetworkReply::wasCached() const
{
    return m_reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool();
}

NetworkReply::ReturnCode NetworkReply::return_code() const
{
    return m_code;
}

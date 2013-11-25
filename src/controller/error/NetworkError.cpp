/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkError.h"

#include "utils/Utils.h"
#include <QApplication>

const char* NetworkError::LOC_CONTEXT = "NetworkError";

NetworkError::NetworkError(QObject* parent)
    : Error(parent)
{
    init(QNetworkReply::NoError);
}

NetworkError::NetworkError(QNetworkReply::NetworkError error, QObject* parent)
    : Error(parent)
{
    init(error);
}

NetworkError::~NetworkError()
{

}

void NetworkError::init(QNetworkReply::NetworkError error)
{
    uint type;
    QString name;
    QString description;

    switch (error) {
    case QNetworkReply::NoError:
        type = NetworkError::NoError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "NoError:Description");
        break;
    case QNetworkReply::ConnectionRefusedError:
        type = NetworkError::ConnectionRefusedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ConnectionRefusedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ConnectionRefusedError:Description");
        break;
    case QNetworkReply::RemoteHostClosedError:
        type = NetworkError::RemoteHostClosedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "RemoteHostClosedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "RemoteHostClosedError:Description");
        break;
    case QNetworkReply::HostNotFoundError:
        type = NetworkError::HostNotFoundError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "HostNotFoundError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "HostNotFoundError:Description");
        break;
    case QNetworkReply::TimeoutError:
        type = NetworkError::TimeoutError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "TimeoutError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "TimeoutError:Description");
        break;
    case QNetworkReply::OperationCanceledError:
        type = NetworkError::OperationCanceledError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "OperationCanceledError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "OperationCanceledError:Description");
        break;
    case QNetworkReply::SslHandshakeFailedError:
        type = NetworkError::SslHandshakeFailedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "SslHandshakeFailedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "SslHandshakeFailedError:Description");
        break;
    case QNetworkReply::TemporaryNetworkFailureError:
        type = NetworkError::TemporaryNetworkFailureError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "TemporaryNetworkFailureError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "TemporaryNetworkFailureError:Description");
        break;
    case QNetworkReply::NetworkSessionFailedError:
        type = NetworkError::NetworkSessionFailedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "NetworkSessionFailedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "NetworkSessionFailedError:Description");
        break;
    case QNetworkReply::BackgroundRequestNotAllowedError:
        type = NetworkError::BackgroundRequestNotAllowedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "BackgroundRequestNotAllowedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "BackgroundRequestNotAllowedError:Description");
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        type = NetworkError::ProxyConnectionRefusedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyConnectionRefusedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyConnectionRefusedError:Description");
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        type = NetworkError::ProxyConnectionClosedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyConnectionClosedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyConnectionClosedError:Description");
        break;
    case QNetworkReply::ProxyNotFoundError:
        type = NetworkError::ProxyNotFoundError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyNotFoundError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyNotFoundError:Description");
        break;
    case QNetworkReply::ProxyTimeoutError:
        type = NetworkError::ProxyTimeoutError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyTimeoutError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyTimeoutError:Description");
        break;
    case QNetworkReply::ProxyAuthenticationRequiredError:
        type = NetworkError::ProxyAuthenticationRequiredError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyAuthenticationRequiredError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProxyAuthenticationRequiredError:Description");
        break;
    case QNetworkReply::ContentAccessDenied:
        type = NetworkError::ContentAccessDenied;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentAccessDenied:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentAccessDenied:Description");
        break;
    case QNetworkReply::ContentOperationNotPermittedError:
        type = NetworkError::ContentOperationNotPermittedError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentOperationNotPermittedError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentOperationNotPermittedError:Description");
        break;
    case QNetworkReply::ContentNotFoundError:
        type = NetworkError::ContentNotFoundError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentNotFoundError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentNotFoundError:Description");
        break;
    case QNetworkReply::AuthenticationRequiredError:
        type = NetworkError::AuthenticationRequiredError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "AuthenticationRequiredError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "AuthenticationRequiredError:Description");
        break;
    case QNetworkReply::ContentReSendError:
        type = NetworkError::ContentReSendError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentReSendError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ContentReSendError:Description");
        break;
    case QNetworkReply::ProtocolUnknownError:
        type = NetworkError::ProtocolUnknownError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProtocolUnknownError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProtocolUnknownError:Description");
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        type = NetworkError::ProtocolInvalidOperationError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProtocolInvalidOperationError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProtocolInvalidOperationError:Description");
        break;
    case QNetworkReply::UnknownNetworkError:
        type = NetworkError::UnknownNetworkError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownNetworkError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownNetworkError:Description");
        break;
    case QNetworkReply::UnknownProxyError:
        type = NetworkError::UnknownProxyError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownProxyError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownProxyError:Description");
        break;
    case QNetworkReply::UnknownContentError:
        type = NetworkError::UnknownContentError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownContentError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownContentError:Description");
        break;
    case QNetworkReply::ProtocolFailure:
        type = NetworkError::ProtocolFailure;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "ProtocolFailure:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "ProtocolFailure:Description");
        break;
    default:
        type = NetworkError::UnknownError;
        name = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownError:Name");
        description = QApplication::translate(NetworkError::LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

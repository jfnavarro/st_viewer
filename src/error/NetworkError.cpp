#include "NetworkError.h"

#include <QApplication>

static const char *LOC_CONTEXT = "NetworkError";

NetworkError::NetworkError(QObject *parent)
    : Error(parent)
{
    init(QNetworkReply::NoError);
}

NetworkError::NetworkError(QNetworkReply::NetworkError error, QObject *parent)
    : Error(parent)
{
    init(error);
}

NetworkError::~NetworkError()
{
}

void NetworkError::init(QNetworkReply::NetworkError error)
{
    QString name;
    QString description;

    switch (error) {
    case QNetworkReply::NoError:
        name = QApplication::translate(LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(LOC_CONTEXT, "NoError:Description");
        break;
    case QNetworkReply::ConnectionRefusedError:
        name = QApplication::translate(LOC_CONTEXT, "ConnectionRefusedError:Name");
        description = QApplication::translate(LOC_CONTEXT, "ConnectionRefusedError:Description");
        break;
    case QNetworkReply::RemoteHostClosedError:
        name = QApplication::translate(LOC_CONTEXT, "RemoteHostClosedError:Name");
        description = QApplication::translate(LOC_CONTEXT, "RemoteHostClosedError:Description");
        break;
    case QNetworkReply::HostNotFoundError:
        name = QApplication::translate(LOC_CONTEXT, "HostNotFoundError:Name");
        description = QApplication::translate(LOC_CONTEXT, "HostNotFoundError:Description");
        break;
    case QNetworkReply::TimeoutError:
        name = QApplication::translate(LOC_CONTEXT, "TimeoutError:Name");
        description = QApplication::translate(LOC_CONTEXT, "TimeoutError:Description");
        break;
    case QNetworkReply::OperationCanceledError:
        name = QApplication::translate(LOC_CONTEXT, "OperationCanceledError:Name");
        description = QApplication::translate(LOC_CONTEXT, "OperationCanceledError:Description");
        break;
    case QNetworkReply::SslHandshakeFailedError:
        name = QApplication::translate(LOC_CONTEXT, "SslHandshakeFailedError:Name");
        description = QApplication::translate(LOC_CONTEXT, "SslHandshakeFailedError:Description");
        break;
    case QNetworkReply::TemporaryNetworkFailureError:
        name = QApplication::translate(LOC_CONTEXT, "TemporaryNetworkFailureError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "TemporaryNetworkFailureError:Description");
        break;
    case QNetworkReply::NetworkSessionFailedError:
        name = QApplication::translate(LOC_CONTEXT, "NetworkSessionFailedError:Name");
        description = QApplication::translate(LOC_CONTEXT, "NetworkSessionFailedError:Description");
        break;
    case QNetworkReply::BackgroundRequestNotAllowedError:
        name = QApplication::translate(LOC_CONTEXT, "BackgroundRequestNotAllowedError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "BackgroundRequestNotAllowedError:Description");
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        name = QApplication::translate(LOC_CONTEXT, "ProxyConnectionRefusedError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "ProxyConnectionRefusedError:Description");
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        name = QApplication::translate(LOC_CONTEXT, "ProxyConnectionClosedError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "ProxyConnectionClosedError:Description");
        break;
    case QNetworkReply::ProxyNotFoundError:
        name = QApplication::translate(LOC_CONTEXT, "ProxyNotFoundError:Name");
        description = QApplication::translate(LOC_CONTEXT, "ProxyNotFoundError:Description");
        break;
    case QNetworkReply::ProxyTimeoutError:
        name = QApplication::translate(LOC_CONTEXT, "ProxyTimeoutError:Name");
        description = QApplication::translate(LOC_CONTEXT, "ProxyTimeoutError:Description");
        break;
    case QNetworkReply::ProxyAuthenticationRequiredError:
        name = QApplication::translate(LOC_CONTEXT, "ProxyAuthenticationRequiredError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "ProxyAuthenticationRequiredError:Description");
        break;
    case QNetworkReply::ContentAccessDenied:
        name = QApplication::translate(LOC_CONTEXT, "ContentAccessDenied:Name");
        description = QApplication::translate(LOC_CONTEXT, "ContentAccessDenied:Description");
        break;
    case QNetworkReply::ContentOperationNotPermittedError:
        name = QApplication::translate(LOC_CONTEXT, "ContentOperationNotPermittedError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "ContentOperationNotPermittedError:Description");
        break;
    case QNetworkReply::ContentNotFoundError:
        name = QApplication::translate(LOC_CONTEXT, "ContentNotFoundError:Name");
        description = QApplication::translate(LOC_CONTEXT, "ContentNotFoundError:Description");
        break;
    case QNetworkReply::AuthenticationRequiredError:
        name = QApplication::translate(LOC_CONTEXT, "AuthenticationRequiredError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "AuthenticationRequiredError:Description");
        break;
    case QNetworkReply::ContentReSendError:
        name = QApplication::translate(LOC_CONTEXT, "ContentReSendError:Name");
        description = QApplication::translate(LOC_CONTEXT, "ContentReSendError:Description");
        break;
    case QNetworkReply::ProtocolUnknownError:
        name = QApplication::translate(LOC_CONTEXT, "ProtocolUnknownError:Name");
        description = QApplication::translate(LOC_CONTEXT, "ProtocolUnknownError:Description");
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        name = QApplication::translate(LOC_CONTEXT, "ProtocolInvalidOperationError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "ProtocolInvalidOperationError:Description");
        break;
    case QNetworkReply::UnknownNetworkError:
        name = QApplication::translate(LOC_CONTEXT, "UnknownNetworkError:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnknownNetworkError:Description");
        break;
    case QNetworkReply::UnknownProxyError:
        name = QApplication::translate(LOC_CONTEXT, "UnknownProxyError:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnknownProxyError:Description");
        break;
    case QNetworkReply::UnknownContentError:
        name = QApplication::translate(LOC_CONTEXT, "UnknownContentError:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnknownContentError:Description");
        break;
    case QNetworkReply::ProtocolFailure:
        name = QApplication::translate(LOC_CONTEXT, "ProtocolFailure:Name");
        description = QApplication::translate(LOC_CONTEXT, "ProtocolFailure:Description");
        break;
    default:
        name = QApplication::translate(LOC_CONTEXT, "UnknownError:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

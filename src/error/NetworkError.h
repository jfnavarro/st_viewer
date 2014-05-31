/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef NETWORKERROR_H
#define NETWORKERROR_H

#include <QNetworkReply>

#include "error/Error.h"
#include "network/NetworkCommand.h"

class NetworkError : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type {
        NoError = 0xa1fb320d,
        ConnectionRefusedError = 0xa06e9510,
        RemoteHostClosedError = 0xa0e7c2d4,
        HostNotFoundError = 0x7c8a3fd7,
        TimeoutError = 0x2d5e37a1,
        OperationCanceledError = 0x856d0cc2,
        SslHandshakeFailedError = 0xcf105cc6,
        TemporaryNetworkFailureError = 0x80aa8855,
        NetworkSessionFailedError = 0x5f81c5e9,
        BackgroundRequestNotAllowedError = 0x947fb16c,
        ProxyConnectionRefusedError = 0xd4c6d56a,
        ProxyConnectionClosedError = 0x40635d6a,
        ProxyNotFoundError = 0x59a27a05,
        ProxyTimeoutError = 0x2be0407b,
        ProxyAuthenticationRequiredError = 0xd1b3a83d,
        ContentAccessDenied = 0xdfc4517e,
        ContentOperationNotPermittedError = 0xb41dfafb,
        ContentNotFoundError = 0x0890235a,
        AuthenticationRequiredError = 0x3f4fc057,
        ContentReSendError = 0x8db82b2e,
        ProtocolUnknownError = 0x69776710,
        ProtocolInvalidOperationError = 0x280c3bc6,
        UnknownNetworkError = 0xd1f0ca2a,
        UnknownProxyError = 0x25d82dea,
        UnknownContentError = 0x04aea6bf,
        ProtocolFailure = 0xecdc6718,
        UnknownError = 0xcaebc258
    };

    explicit NetworkError(QObject* parent = 0);
    NetworkError(QNetworkReply::NetworkError error, QObject* parent = 0);
    virtual ~NetworkError();

private:

    void init(QNetworkReply::NetworkError error);
};

#endif // NETWORKERROR_H //

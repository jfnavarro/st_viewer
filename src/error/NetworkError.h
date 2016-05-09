#ifndef NETWORKERROR_H
#define NETWORKERROR_H

#include <QNetworkReply>

#include "error/Error.h"
#include "network/NetworkCommand.h"

class NetworkError : public Error
{
    Q_OBJECT

public:
    explicit NetworkError(QObject *parent = 0);
    NetworkError(QNetworkReply::NetworkError error, QObject *parent = 0);
    virtual ~NetworkError();

private:
    void init(QNetworkReply::NetworkError error);
};

#endif // NETWORKERROR_H //

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SSLNETWORKERROR_H
#define SSLNETWORKERROR_H

#include <QObject>
#include <QSslError>

#include "error/Error.h"
#include "network/NetworkCommand.h"

class SSLNetworkError : public Error
{
    Q_OBJECT

public:
    explicit SSLNetworkError(QObject* parent = 0);
    SSLNetworkError(const QSslError& sslError, QObject* parent = 0);
    virtual ~SSLNetworkError();

private:
    void init(const QSslError& sslError);
};

#endif // SSLNETWORKERROR_H //

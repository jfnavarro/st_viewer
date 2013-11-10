/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SERVERERROR_H
#define SERVERERROR_H

#include "Error.h"

class ServerError : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    
    enum Type
    {
        NoError = 0xd366e2a2,
        BadRequest = 0x641d3f0f,
        ResourceNotFound = 0x3e9ccb22,
        UnknownError = 0x179026e3
    };
    
    explicit ServerError(QObject* parent = 0);
    ServerError(const QString& serverErrorName, const QString& serverErrorDescription, QObject* parent = 0);
    virtual ~ServerError();

private:
    
    // precomputed hash values of string error codes (uses the global QT function "qHash(QString)")
    enum InternalHash
    {
        _NoError = 0x00000000,
        // Server error codes
        _BadRequest = 0x1af9dd54,
        _ResourceNotFound = 0xbb041683
    };

    // localization context
    static const char* LOC_CONTEXT;

    void init(uint error);

    // store internal variables
    QString m_serverErrorName;
    QString m_serverErrorDescription;
};

#endif // SERVERERROR_H //

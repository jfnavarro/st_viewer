/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef OAUTH2ERROR_H
#define OAUTH2ERROR_H

#include "controller/error/Error.h"

class OAuth2Error : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)

public:

    enum Type {
        NoError = 0x2326bdba,
        InvalidRequest = 0xdeb1e445,
        InvalidClient = 0x594ff355,
        UnauthorizedClient = 0x7e00330c,
        RedirectUriMismatch = 0x74fea251,
        AccessDenied = 0x530ea90c,
        UnsupportedResponseType = 0x9f9310e3,
        UnsupportedGrantType = 0xf49f08c7, //NOTE this is not added to InternalHash
        InvalidGrant = 0xcd7780d4, //NOTE this is not added to InternalHash
        InvalidScope = 0x1443258a,
        EmptyToken = 0xc14f8ad9,
        UnknownError = 0xa5757acb,
    };

    explicit OAuth2Error(QObject* parent = 0);
    OAuth2Error(const QString& errorName, const QString& errorDescription, QObject* parent = 0);
    virtual ~OAuth2Error();

private:
    // precomputed hash values of string error codes (uses the global QT function "qHash(QString)")
    enum InternalHash {
        _NoError = 0x00000000,
        // OAuth2 error codes
        _InvalidRequest = 0x7E34A447,
        _InvalidClient = 0xDA542B13,
        _UnauthorizedClient = 0x29A5CFBB,
        _RedirectUriMismatch = 0x3BCB6EC4,
        _AccessDenied = 0xE57FAAD6,
        _UnsupportedResponseType = 0xEC2F27EE,
        _InvalidScope = 0xCE19FE0C,
        // Custom error codes
        _EmptyToken = 0x5AFDB487
    };
    // localization contex
    static const char* LOC_CONTEXT;
    void init(uint error);
    // store internal variables
    QString m_internalName;
    QString m_internalDescription;
};

#endif // OAUTH2ERROR_H //

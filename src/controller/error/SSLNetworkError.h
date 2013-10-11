/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SSLNETWORKERROR_H
#define SSLNETWORKERROR_H

#include <QObject>
#include <QSslError>

#include "controller/error/Error.h"
#include "controller/network/NetworkCommand.h"

class SSLNetworkError : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)
    
public:
    
    enum Type
    {
        NoError = 0x54449ba1,
        UnableToGetIssuerCertificate = 0x41d8c03c,
        UnableToDecryptCertificateSignature = 0xb6f95fba,
        UnableToDecodeIssuerPublicKey = 0xd746635b,
        CertificateSignatureFailed = 0x309e5b64,
        CertificateNotYetValid = 0x59fe15b6,
        CertificateExpired = 0x0c301274,
        InvalidNotBeforeField = 0xd5232719,
        InvalidNotAfterField = 0x8cdad260,
        SelfSignedCertificate = 0x5020f449,
        SelfSignedCertificateInChain = 0x5f173813,
        UnableToGetLocalIssuerCertificate = 0x9a243895,
        UnableToVerifyFirstCertificate = 0x4cb01e34,
        CertificateRevoked = 0x9bcc2ded,
        InvalidCaCertificate = 0x704e9908,
        PathLengthExceeded = 0xdeb794c4,
        InvalidPurpose = 0x746def0d,
        CertificateUntrusted = 0xd915ebcd,
        CertificateRejected = 0xc8af130f,
        SubjectIssuerMismatch = 0xf7d1954d,
        AuthorityIssuerSerialNumberMismatch = 0x13970921,
        NoPeerCertificate = 0xd3dc852e,
        HostNameMismatch = 0xb70681a7,
        NoSslSupport = 0x0b71022a,
        CertificateBlacklisted = 0xc0f38e4b,
        UnspecifiedError = 0x22a87f77,
        UnknownError = 0x47a93544
    };

    explicit SSLNetworkError(QObject* parent = 0);
    SSLNetworkError(const QSslError& sslError, NetworkCommand* cmd, QObject* parent = 0);
    virtual ~SSLNetworkError();

    inline const NetworkCommand* networkCommand() const { return m_cmd; }
    
private:
    // localization context
    static const char* LOC_CONTEXT;

    void init(const QSslError& sslError);
    
    NetworkCommand* m_cmd;
    QSslError m_sslError;
};

#endif // SSLNETWORKERROR_H //

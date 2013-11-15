/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>

#include "SSLNetworkError.h"

const char* SSLNetworkError::LOC_CONTEXT = "SSLNetworkError";

SSLNetworkError::SSLNetworkError(QObject* parent)
    : Error(parent), m_cmd(0), m_sslError()
{
    init(m_sslError);
}

SSLNetworkError::SSLNetworkError(const QSslError& sslError, NetworkCommand* cmd, QObject* parent)
    : Error(parent), m_cmd(cmd), m_sslError(sslError)
{
    init(sslError);
}

SSLNetworkError::~SSLNetworkError()
{

}

void SSLNetworkError::init(const QSslError& sslError)
{
    uint type;
    QString name;
    QString description;

    QSslError::SslError error = sslError.error();
    switch (error)
    {
    case QSslError::NoError:
        type = SSLNetworkError::NoError;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "NoError:Description");
        break;
    case QSslError::UnableToGetIssuerCertificate:
        type = SSLNetworkError::UnableToGetIssuerCertificate;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToGetIssuerCertificate:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToGetIssuerCertificate:Description");
        break;
    case QSslError::UnableToDecryptCertificateSignature:
        type = SSLNetworkError::UnableToDecryptCertificateSignature;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToDecryptCertificateSignature:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToDecryptCertificateSignature:Description");
        break;
    case QSslError::UnableToDecodeIssuerPublicKey:
        type = SSLNetworkError::UnableToDecodeIssuerPublicKey;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToDecodeIssuerPublicKey:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToDecodeIssuerPublicKey:Description");
        break;
    case QSslError::CertificateSignatureFailed:
        type = SSLNetworkError::CertificateSignatureFailed;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateSignatureFailed:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateSignatureFailed:Description");
        break;
    case QSslError::CertificateNotYetValid:
        type = SSLNetworkError::CertificateNotYetValid;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateNotYetValid:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateNotYetValid:Description");
        break;
    case QSslError::CertificateExpired:
        type = SSLNetworkError::CertificateExpired;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateExpired:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateExpired:Description");
        break;
    case QSslError::InvalidNotBeforeField:
        type = SSLNetworkError::InvalidNotBeforeField;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidNotBeforeField:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidNotBeforeField:Description");
        break;
    case QSslError::InvalidNotAfterField:
        type = SSLNetworkError::InvalidNotAfterField;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidNotAfterField:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidNotAfterField:Description");
        break;
    case QSslError::SelfSignedCertificate:
        type = SSLNetworkError::SelfSignedCertificate;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "SelfSignedCertificate:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "SelfSignedCertificate:Description");
        break;
    case QSslError::SelfSignedCertificateInChain:
        type = SSLNetworkError::SelfSignedCertificateInChain;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "SelfSignedCertificateInChain:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "SelfSignedCertificateInChain:Description");
        break;
    case QSslError::UnableToGetLocalIssuerCertificate:
        type = SSLNetworkError::UnableToGetLocalIssuerCertificate;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToGetLocalIssuerCertificate:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToGetLocalIssuerCertificate:Description");
        break;
    case QSslError::UnableToVerifyFirstCertificate:
        type = SSLNetworkError::UnableToVerifyFirstCertificate;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToVerifyFirstCertificate:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnableToVerifyFirstCertificate:Description");
        break;
    case QSslError::CertificateRevoked:
        type = SSLNetworkError::CertificateRevoked;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateRevoked:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateRevoked:Description");
        break;
    case QSslError::InvalidCaCertificate:
        type = SSLNetworkError::InvalidCaCertificate;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidCaCertificate:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidCaCertificate:Description");
        break;
    case QSslError::PathLengthExceeded:
        type = SSLNetworkError::PathLengthExceeded;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "PathLengthExceeded:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "PathLengthExceeded:Description");
        break;
    case QSslError::InvalidPurpose:
        type = SSLNetworkError::InvalidPurpose;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidPurpose:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "InvalidPurpose:Description");
        break;
    case QSslError::CertificateUntrusted:
        type = SSLNetworkError::CertificateUntrusted;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateUntrusted:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateUntrusted:Description");
        break;
    case QSslError::CertificateRejected:
        type = SSLNetworkError::CertificateRejected;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateRejected:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateRejected:Description");
        break;
    case QSslError::SubjectIssuerMismatch:
        type = SSLNetworkError::SubjectIssuerMismatch;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "SubjectIssuerMismatch:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "SubjectIssuerMismatch:Description");
        break;
    case QSslError::AuthorityIssuerSerialNumberMismatch:
        type  = SSLNetworkError::AuthorityIssuerSerialNumberMismatch;
        name  = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "AuthorityIssuerSerialNumberMismatch:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "AuthorityIssuerSerialNumberMismatch:Description");
        break;
    case QSslError::NoPeerCertificate:
        type = SSLNetworkError::NoPeerCertificate;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "NoPeerCertificate:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "NoPeerCertificate:Description");
        break;
    case QSslError::HostNameMismatch:
        type = SSLNetworkError::HostNameMismatch;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "HostNameMismatch:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "HostNameMismatch:Description");
        break;
    case QSslError::NoSslSupport:
        type = SSLNetworkError::NoSslSupport;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "NoSslSupport:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "NoSslSupport:Description");
        break;
    case QSslError::CertificateBlacklisted:
        type = SSLNetworkError::CertificateBlacklisted;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateBlacklisted:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "CertificateBlacklisted:Description");
        break;
    case QSslError::UnspecifiedError:
        type = SSLNetworkError::UnspecifiedError;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnspecifiedError:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnspecifiedError:Description");
        break;
    default:
        type = SSLNetworkError::UnknownError;
        name = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnknownError:Name");
        description = QApplication::translate(SSLNetworkError::LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

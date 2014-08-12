/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SSLNetworkError.h"

#include <QApplication>

static const char* LOC_CONTEXT = "SSLNetworkError";

SSLNetworkError::SSLNetworkError(QObject* parent)
    : Error(parent)
{
    init(QSslError());
}

SSLNetworkError::SSLNetworkError(const QSslError& sslError, QObject* parent)
    : Error(parent)
{
    init(sslError);
}

SSLNetworkError::~SSLNetworkError()
{

}

void SSLNetworkError::init(const QSslError& sslError)
{
    QString name;
    QString description;

    QSslError::SslError error = sslError.error();
    switch (error) {
    case QSslError::NoError:
        name = QApplication::translate(LOC_CONTEXT,
                                       "NoError:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "NoError:Description");
        break;
    case QSslError::UnableToGetIssuerCertificate:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnableToGetIssuerCertificate:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnableToGetIssuerCertificate:Description");
        break;
    case QSslError::UnableToDecryptCertificateSignature:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnableToDecryptCertificateSignature:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnableToDecryptCertificateSignature:Description");
        break;
    case QSslError::UnableToDecodeIssuerPublicKey:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnableToDecodeIssuerPublicKey:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnableToDecodeIssuerPublicKey:Description");
        break;
    case QSslError::CertificateSignatureFailed:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateSignatureFailed:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateSignatureFailed:Description");
        break;
    case QSslError::CertificateNotYetValid:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateNotYetValid:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateNotYetValid:Description");
        break;
    case QSslError::CertificateExpired:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateExpired:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateExpired:Description");
        break;
    case QSslError::InvalidNotBeforeField:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidNotBeforeField:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidNotBeforeField:Description");
        break;
    case QSslError::InvalidNotAfterField:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidNotAfterField:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidNotAfterField:Description");
        break;
    case QSslError::SelfSignedCertificate:
        name = QApplication::translate(LOC_CONTEXT,
                                       "SelfSignedCertificate:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "SelfSignedCertificate:Description");
        break;
    case QSslError::SelfSignedCertificateInChain:
        name = QApplication::translate(LOC_CONTEXT,
                                       "SelfSignedCertificateInChain:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "SelfSignedCertificateInChain:Description");
        break;
    case QSslError::UnableToGetLocalIssuerCertificate:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnableToGetLocalIssuerCertificate:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnableToGetLocalIssuerCertificate:Description");
        break;
    case QSslError::UnableToVerifyFirstCertificate:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnableToVerifyFirstCertificate:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnableToVerifyFirstCertificate:Description");
        break;
    case QSslError::CertificateRevoked:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateRevoked:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateRevoked:Description");
        break;
    case QSslError::InvalidCaCertificate:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidCaCertificate:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidCaCertificate:Description");
        break;
    case QSslError::PathLengthExceeded:
        name = QApplication::translate(LOC_CONTEXT,
                                       "PathLengthExceeded:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "PathLengthExceeded:Description");
        break;
    case QSslError::InvalidPurpose:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidPurpose:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidPurpose:Description");
        break;
    case QSslError::CertificateUntrusted:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateUntrusted:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateUntrusted:Description");
        break;
    case QSslError::CertificateRejected:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateRejected:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateRejected:Description");
        break;
    case QSslError::SubjectIssuerMismatch:
        name = QApplication::translate(LOC_CONTEXT,
                                       "SubjectIssuerMismatch:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "SubjectIssuerMismatch:Description");
        break;
    case QSslError::AuthorityIssuerSerialNumberMismatch:
        name  = QApplication::translate(LOC_CONTEXT,
                                        "AuthorityIssuerSerialNumberMismatch:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "AuthorityIssuerSerialNumberMismatch:Description");
        break;
    case QSslError::NoPeerCertificate:
        name = QApplication::translate(LOC_CONTEXT,
                                       "NoPeerCertificate:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "NoPeerCertificate:Description");
        break;
    case QSslError::HostNameMismatch:
        name = QApplication::translate(LOC_CONTEXT,
                                       "HostNameMismatch:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "HostNameMismatch:Description");
        break;
    case QSslError::NoSslSupport:
        name = QApplication::translate(LOC_CONTEXT,
                                       "NoSslSupport:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "NoSslSupport:Description");
        break;
    case QSslError::CertificateBlacklisted:
        name = QApplication::translate(LOC_CONTEXT,
                                       "CertificateBlacklisted:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "CertificateBlacklisted:Description");
        break;
    case QSslError::UnspecifiedError:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnspecifiedError:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnspecifiedError:Description");
        break;
    default:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnknownError:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnknownError:Description").arg(error);
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QString>
#include <QApplication>

#include "OAuth2Error.h"

const char* OAuth2Error::LOC_CONTEXT = "OAuth2Error";

OAuth2Error::OAuth2Error(QObject* parent)
    : Error(parent), m_internalName(), m_internalDescription()
{
    init(OAuth2Error::_NoError);
}

OAuth2Error::OAuth2Error(const QString& errorName, const QString& errorDescription, QObject* parent)
    : Error(parent), m_internalName(errorName), m_internalDescription(errorDescription)
{
    // can't switch on QString so compute hash
    uint error = qHash(errorName);
    init(error);
}

OAuth2Error::~OAuth2Error()
{

}

void OAuth2Error::init(uint error)
{
    uint type;
    QString name;
    QString description;

    switch (error)
    {
    case OAuth2Error::_NoError:
        type = OAuth2Error::NoError;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "NoError:Description");
        break;
    case OAuth2Error::_InvalidRequest:
        type = OAuth2Error::InvalidRequest;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidRequest:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidRequest:Description");
        break;
    case OAuth2Error::_InvalidClient:
        type = OAuth2Error::InvalidClient;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidClient:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidClient:Description");
        break;
    case OAuth2Error::_UnauthorizedClient:
        type = OAuth2Error::UnauthorizedClient;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnauthorizedClient:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnauthorizedClient:Description");
        break;
    case OAuth2Error::_RedirectUriMismatch:
        type = OAuth2Error::RedirectUriMismatch;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "RedirectUriMismatch:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "RedirectUriMismatch:Description");
        break;
    case OAuth2Error::_AccessDenied:
        type = OAuth2Error::AccessDenied;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "AccessDenied:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "AccessDenied:Description");
        break;
    case OAuth2Error::_UnsupportedResponseType:
        type = OAuth2Error::UnsupportedResponseType;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnsupportedResponseType:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnsupportedResponseType:Description");
        break;
    case OAuth2Error::_InvalidScope:
        type = OAuth2Error::InvalidScope;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidScope:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidScope:Description");
        break;
    case OAuth2Error::_EmptyToken:
        type = OAuth2Error::EmptyToken;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "EmptyToken:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "EmptyToken:Description");
        break;
    case OAuth2Error::UnsupportedGrantType:
        type = OAuth2Error::UnsupportedGrantType;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnsupportedGrantType:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnsupportedGrantType:Description");
        break;
    case OAuth2Error::InvalidGrant:
        type = OAuth2Error::InvalidGrant;
        name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidGrant:Name");
        description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "InvalidGrant:Description");
        break;
    default:
        type = OAuth2Error::UnknownError;
        name = m_internalName;
        description = m_internalDescription;
        //name = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnknownError:Name");
        //description = QApplication::translate(OAuth2Error::LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    //Error::type(type);
    Error::name(name);
    Error::description(description);
}

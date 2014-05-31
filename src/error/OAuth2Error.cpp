/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "OAuth2Error.h"

#include <QString>
#include <QApplication>

static const char* LOC_CONTEXT = "OAuth2Error";

OAuth2Error::OAuth2Error(QObject* parent)
    : Error(parent)
{
    init(OAuth2Error::_NoError);
}

OAuth2Error::OAuth2Error(const QString& errorName,
                         const QString& errorDescription, QObject* parent)
    : Error(parent)
{
    // can't switch on QString so compute hash
    uint error = qHash(errorName);
    init(error, errorName, errorDescription);
}

OAuth2Error::~OAuth2Error()
{

}

void OAuth2Error::init(uint error, const QString& errorName,
                       const QString& errorDescription)
{
    QString name;
    QString description;

    switch (error) {
    case OAuth2Error::_NoError:
        name = QApplication::translate(LOC_CONTEXT,
                                       "NoError:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "NoError:Description");
        break;
    case OAuth2Error::_InvalidRequest:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidRequest:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidRequest:Description");
        break;
    case OAuth2Error::_InvalidClient:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidClient:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidClient:Description");
        break;
    case OAuth2Error::_UnauthorizedClient:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnauthorizedClient:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnauthorizedClient:Description");
        break;
    case OAuth2Error::_RedirectUriMismatch:
        name = QApplication::translate(LOC_CONTEXT,
                                       "RedirectUriMismatch:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "RedirectUriMismatch:Description");
        break;
    case OAuth2Error::_AccessDenied:
        name = QApplication::translate(LOC_CONTEXT,
                                       "AccessDenied:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "AccessDenied:Description");
        break;
    case OAuth2Error::_UnsupportedResponseType:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnsupportedResponseType:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnsupportedResponseType:Description");
        break;
    case OAuth2Error::_InvalidScope:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidScope:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidScope:Description");
        break;
    case OAuth2Error::_EmptyToken:
        name = QApplication::translate(LOC_CONTEXT,
                                       "EmptyToken:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "EmptyToken:Description");
        break;
    case OAuth2Error::UnsupportedGrantType:
        name = QApplication::translate(LOC_CONTEXT,
                                       "UnsupportedGrantType:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "UnsupportedGrantType:Description");
        break;
    case OAuth2Error::InvalidGrant:
        name = QApplication::translate(LOC_CONTEXT,
                                       "InvalidGrant:Name");
        description = QApplication::translate(LOC_CONTEXT,
                                              "InvalidGrant:Description");
        break;
    default:
        //show the original error message when it is a unknown error
        name = errorName;
        description = errorDescription;
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

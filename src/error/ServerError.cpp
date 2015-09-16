/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ServerError.h"

#include <QApplication>

static const char* LOC_CONTEXT = "ServerError";

ServerError::ServerError(const QString& serverErrorName,
                         const QString& serverErrorDescription,
                         QObject* parent)
    : Error(parent)
{
    init(serverErrorName, serverErrorDescription);
}

ServerError::~ServerError()
{
}

void ServerError::init(const QString& errorName, const QString& errorDescription)
{
    QString name;
    QString description;

    if (errorName == "invalid_request") {
        name = QApplication::translate(LOC_CONTEXT, "InvalidRequest:Name");
        description = QApplication::translate(LOC_CONTEXT, "InvalidRequest:Description");
    } else if (errorName == "invalid_client") {
        name = QApplication::translate(LOC_CONTEXT, "InvalidClient:Name");
        description = QApplication::translate(LOC_CONTEXT, "InvalidClient:Description");
    } else if (errorName == "unauthorized_client") {
        name = QApplication::translate(LOC_CONTEXT, "UnauthorizedClient:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnauthorizedClient:Description");
    } else if (errorName == "redirect_uri_mismatch") {
        name = QApplication::translate(LOC_CONTEXT, "RedirectUriMismatch:Name");
        description = QApplication::translate(LOC_CONTEXT, "RedirectUriMismatch:Description");
    } else if (errorName == "access_denied") {
        name = QApplication::translate(LOC_CONTEXT, "AccessDenied:Name");
        description = QApplication::translate(LOC_CONTEXT, "AccessDenied:Description");
    } else if (errorName == "unsupported_response_type") {
        name = QApplication::translate(LOC_CONTEXT, "UnsupportedResponseType:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnsupportedResponseType:Description");
    } else if (errorName == "invalid_scope") {
        name = QApplication::translate(LOC_CONTEXT, "InvalidScope:Name");
        description = QApplication::translate(LOC_CONTEXT, "InvalidScope:Description");
    } else if (errorName == "invalid_token") {
        name = QApplication::translate(LOC_CONTEXT, "EmptyToken:Name");
        description = QApplication::translate(LOC_CONTEXT, "EmptyToken:Description");
    } else if (errorName == "unsupported_grant_type") {
        // name = QApplication::translate(LOC_CONTEXT,
        //                               "UnsupportedGrantType:Name");
        name = QApplication::translate("ServerError", "BadRequest:Name");
        description = QApplication::translate(LOC_CONTEXT, "UnsupportedGrantType:Description");
    } else if (errorName == "invalid_grant") {
        name = QApplication::translate(LOC_CONTEXT, "InvalidGrant:Name");
        description = QApplication::translate(LOC_CONTEXT, "InvalidGrant:Description");
    } else if (errorName == "invalid_credentials") {
        // do nothing
    } else if (errorName == "Bad request") {
        name = QApplication::translate(LOC_CONTEXT, "BadRequest:Name");
        description = QApplication::translate(LOC_CONTEXT, "BadRequest:Description");
    } else if (errorName == "Internal server error") {
        // do nothing
    } else if (errorName == "Resource not found") {
        name = QApplication::translate(LOC_CONTEXT, "ResourceNotFound:Name");
        description = QApplication::translate(LOC_CONTEXT, "ResourceNotFound:Description");
    } else if (errorName == "Not modified") {
        // do nothing
    } else {
        // show the original error message when it is a unknown error
        name = errorName;
        description = errorDescription;
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

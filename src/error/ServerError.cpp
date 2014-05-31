/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ServerError.h"

#include <QApplication>

static const char* LOC_CONTEXT = "ServerError";

ServerError::ServerError(QObject* parent)
    :  Error(parent)
{
    init(ServerError::_NoError);
}

ServerError::ServerError(const QString& serverErrorName,
                         const QString& serverErrorDescription, QObject* parent)
    : Error(parent)
{
    // can't switch on QString so compute hash
    uint error = qHash(serverErrorName);
    init(error, serverErrorName, serverErrorDescription);
}

ServerError::~ServerError()
{

}

void ServerError::init(uint error, const QString& serverErrorName,
                       const QString& serverErrorDescription)
{
    QString name;
    QString description;

    switch (error) {
    case ServerError::_NoError:
        name = QApplication::translate(LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(LOC_CONTEXT, "NoError:Description");
        break;
    case ServerError::_BadRequest:
        name = QApplication::translate(LOC_CONTEXT,
                                       "BadRequest:Name").arg(serverErrorName);
        description = QApplication::translate(LOC_CONTEXT,
                                              "BadRequest:Description").arg(serverErrorDescription);
        break;
    case ServerError::_ResourceNotFound:
        name = QApplication::translate(LOC_CONTEXT,
                                       "ResourceNotFound:Name").arg(serverErrorName);
        description = QApplication::translate(LOC_CONTEXT,
                                              "ResourceNotFound:Description").arg(serverErrorDescription);
        break;
    default:
        //show the original error message when it is a unknown error
        name = serverErrorName;
        description = serverErrorDescription;
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

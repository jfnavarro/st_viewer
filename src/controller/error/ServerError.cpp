/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>

#include "ServerError.h"


const char* ServerError::LOC_CONTEXT = "ServerError";

ServerError::ServerError(QObject* parent)
    : Error(parent), m_serverErrorName(), m_serverErrorDescription()
{
    init(ServerError::_NoError);
}

ServerError::ServerError(const QString& serverErrorName, const QString& serverErrorDescription, QObject* parent)
    : Error(parent), m_serverErrorName(serverErrorName), m_serverErrorDescription(serverErrorDescription)
{
    // can't switch on QString so compute hash
    uint error = qHash(serverErrorName);
    init(error);
}

ServerError::~ServerError()
{

}

void ServerError::init(uint error)
{
    uint type;
    QString name;
    QString description;

    switch (error)
    {
        case ServerError::_NoError:
            type = ServerError::NoError;
            name = QApplication::translate(ServerError::LOC_CONTEXT, "NoError:Name");
            description = QApplication::translate(ServerError::LOC_CONTEXT, "NoError:Description");
            break;
        case ServerError::_BadRequest:
            type = ServerError::BadRequest;
            name = QApplication::translate(ServerError::LOC_CONTEXT, "BadRequest:Name").arg(m_serverErrorName);
            description = QApplication::translate(ServerError::LOC_CONTEXT, "BadRequest:Description").arg(m_serverErrorDescription);
            break;
        case ServerError::_ResourceNotFound:
            type = ServerError::ResourceNotFound;
            name = QApplication::translate(ServerError::LOC_CONTEXT, "ResourceNotFound:Name").arg(m_serverErrorName);
            description = QApplication::translate(ServerError::LOC_CONTEXT, "ResourceNotFound:Description").arg(m_serverErrorDescription);
            break;
        default:
            type = ServerError::UnknownError;
            name = m_serverErrorName;
            description = m_serverErrorDescription;
//             name  = QApplication::translate(ServerError::LOC_CONTEXT, "UnknownError:Name");
//             description = QApplication::translate(ServerError::LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    //Error::type(type);
    Error::name(name);
    Error::description(description);
}

/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>

#include "ApplicationError.h"

const char* ApplicationError::LOC_CONTEXT = "ApplicationError";

ApplicationError::ApplicationError(QObject* parent)
    : Error(parent)
{
    init(ApplicationError::NoError);
}

ApplicationError::ApplicationError(ApplicationError::Type errorType, QObject* parent)
    : Error(parent)
{
    init(errorType);
}

ApplicationError::~ApplicationError()
{

}

void ApplicationError::init(ApplicationError::Type errorType)
{
    uint type;
    QString name;
    QString description;

    switch (errorType)
    {
    case ApplicationError::NoError:
        type = ApplicationError::NoError; // because because!
        name  = QApplication::translate(ApplicationError::LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(ApplicationError::LOC_CONTEXT, "NoError:Description");
        break;
        //NOTE well this is awkward... the error handling system relies on localization to show the error
        //     message but if there was an error initializing the localization system... yeeeeaaaah... *sigh*
    case ApplicationError::LocalizationError:
        type = ApplicationError::LocalizationError;
        name = QApplication::translate(ApplicationError::LOC_CONTEXT, "LocalizationError:Name");
        description = QApplication::translate(ApplicationError::LOC_CONTEXT, "LocalizationError:Description");
        break;
    default:
        type = ApplicationError::UnknownError;
        name = QApplication::translate(ApplicationError::LOC_CONTEXT, "UnknownError:Name");
        description = QApplication::translate(ApplicationError::LOC_CONTEXT, "UnknownError:Description").arg(errorType);
    }

    // assign name and description
    //Error::type(type);
    Error::name(name);
    Error::description(description);
}

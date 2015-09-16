/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ApplicationError.h"

#include <QApplication>

static const char* LOC_CONTEXT = "ApplicationError";

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
    QString name;
    QString description;

    switch (errorType) {
    case ApplicationError::NoError:
        name = QApplication::translate(LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(LOC_CONTEXT, "NoError:Description");
        break;
    case ApplicationError::LocalizationError:
        name = QApplication::translate(LOC_CONTEXT, "LocalizationError:Name");
        description = QApplication::translate(LOC_CONTEXT, "LocalizationError:Description");
        break;
    default:
        name = QApplication::translate(LOC_CONTEXT, "UnknownError:Name");
        description
            = QApplication::translate(LOC_CONTEXT, "UnknownError:Description").arg(errorType);
    }

    // assign name and description
    Error::name(name);
    Error::description(description);
}

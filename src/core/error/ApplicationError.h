/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef APPLICATIONERROR_H
#define APPLICATIONERROR_H

#include "controller/error/Error.h"

class ApplicationError : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type {
        NoError = 0x216fc9af,
        LocalizationError = 0x8dacd5d7,
        UnknownError = 0x942e9ff6
    };

    explicit ApplicationError(QObject* parent = 0);
    ApplicationError(ApplicationError::Type errorType, QObject* parent = 0);
    virtual ~ApplicationError();

private:
    // localization context
    static const char* LOC_CONTEXT;
    void init(ApplicationError::Type errorType);
};

#endif // APPLICATIONERROR_H //

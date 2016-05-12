#ifndef APPLICATIONERROR_H
#define APPLICATIONERROR_H

#include "Error.h"

class ApplicationError : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type { NoError = 0x216fc9af, LocalizationError = 0x8dacd5d7, UnknownError = 0x942e9ff6 };

    explicit ApplicationError(QObject *parent = 0);
    ApplicationError(ApplicationError::Type errorType, QObject *parent = 0);
    virtual ~ApplicationError();

private:
    void init(ApplicationError::Type errorType);
};

#endif // APPLICATIONERROR_H //

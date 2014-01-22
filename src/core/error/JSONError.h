/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef JSONERROR_H
#define JSONERROR_H

#include <QJsonParseError>

#include "controller/error/Error.h"

class JSONError : public Error
{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type {
        NoError = 0xd24d1f67, // qHash(NetworkError::<ErrorName>)
        UnterminatedObject = 0xf90b353b,
        MissingNameSeparator = 0xf6864234,
        UnterminatedArray = 0x8329b0dd,
        MissingValueSeparator = 0x76d9261a,
        IllegalValue = 0x95ca10fb,
        TerminationByNumber = 0x287dc844,
        IllegalNumber = 0x16eb4cff,
        IllegalEscapeSequence = 0x995ba718,
        IllegalUTF8String = 0xd8254d98,
        UnterminatedString = 0x00dff20d,
        MissingObject = 0x8f484ee5,
        DeepNesting = 0x7a22d29e,
        UnknownError = 0x2b5a1d3e
    };

    explicit JSONError(QObject* parent = 0);
    JSONError(QJsonParseError::ParseError error, QObject* parent = 0);
    virtual ~JSONError();

private:
    // localization contex
    static const char* LOC_CONTEXT;
    void init(QJsonParseError::ParseError error);
};

#endif // JSONERROR_H //

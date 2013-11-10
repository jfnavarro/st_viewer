/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>

#include "JSONError.h"

const char* JSONError::LOC_CONTEXT = "JSONError";

JSONError::JSONError(QObject* parent)
    : Error(parent), m_rawJSON()
{
    init(QJsonParseError::NoError);
}

JSONError::JSONError(QJsonParseError::ParseError error, const QByteArray& rawJSON, QObject* parent)
    : Error(parent), m_rawJSON(rawJSON)
{
    init(error);
}

JSONError::~JSONError()
{

}

void JSONError::init(QJsonParseError::ParseError error)
{
    uint type;
    QString name;
    QString description;

    switch (error)
    {
    case QJsonParseError::NoError:
        type = JSONError::NoError;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "NoError:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "NoError:Description");
        break;
    case QJsonParseError::UnterminatedObject:
        type = JSONError::UnterminatedObject;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "UnterminatedObject:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "UnterminatedObject:Description");
        break;
    case QJsonParseError::MissingNameSeparator:
        type = JSONError::MissingNameSeparator;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "MissingNameSeparator:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "MissingNameSeparator:Description");
        break;
    case QJsonParseError::UnterminatedArray:
        type = JSONError::UnterminatedArray;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "UnterminatedArray:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "UnterminatedArray:Description");
        break;
    case QJsonParseError::MissingValueSeparator:
        type = JSONError::MissingValueSeparator;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "MissingValueSeparator:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "MissingValueSeparator:Description");
        break;
    case QJsonParseError::IllegalValue:
        type = JSONError::IllegalValue;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalValue:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalValue:Description");
        break;
    case QJsonParseError::TerminationByNumber:
        type = JSONError::TerminationByNumber;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "TerminationByNumber:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "TerminationByNumber:Description");
        break;
    case QJsonParseError::IllegalNumber:
        type = JSONError::IllegalNumber;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalNumber:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalNumber:Description");
        break;
    case QJsonParseError::IllegalEscapeSequence:
        type = JSONError::IllegalEscapeSequence;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalEscapeSequence:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalEscapeSequence:Description");
        break;
    case QJsonParseError::IllegalUTF8String:
        type = JSONError::IllegalUTF8String;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalUTF8String:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "IllegalUTF8String:Description");
        break;
    case QJsonParseError::UnterminatedString:
        type = JSONError::UnterminatedString;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "UnterminatedString:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "UnterminatedString:Description");
        break;
    case QJsonParseError::MissingObject:
        type = JSONError::MissingObject;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "MissingObject:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "MissingObject:Description");
        break;
    case QJsonParseError::DeepNesting:
        type = JSONError::DeepNesting;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "DeepNesting:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "DeepNesting:Description");
        break;
    default:
        type = JSONError::UnknownError;
        name = QApplication::translate(JSONError::LOC_CONTEXT, "UnknownError:Name");
        description = QApplication::translate(JSONError::LOC_CONTEXT, "UnknownError:Description").arg(error);
    }

    // assign name and description
    //Error::type(type);
    Error::name(name);
    Error::description(description);
}
